// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Animator_H
#define Animator_H

#include <chrono>
#include <functional>
#include <initializer_list>
#include <vector>


enum class Animation
{
	None,
	Delay,
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut
};


class AnimatorBase
{
	friend class AnimatorHost;

	static std::vector<AnimatorBase*> _animators;

protected:
	std::vector<AnimatorBase*> _dependency;
	Animation _animation{};
	float _duration{};
	float _elapsed{};
	bool _value_updated{};

protected:
	AnimatorBase();
	virtual ~AnimatorBase();

	static float AnimationFactor(float k, Animation animation);

public:
	AnimatorBase(const AnimatorBase&) = delete;
	AnimatorBase& operator=(const AnimatorBase&) = delete;

private:
	virtual void TryUpdateValue(float secondsSinceLastUpdate) = 0;
};


template <class T>
class Animator : public AnimatorBase
{
	T _value{};
	T _value1{};
	T _value2{};
	std::function<T()> _input{};
	std::function<void(T)> _output{};

public:
	T GetValue() const
	{
		return _value;
	}

	Animator<T>& SetInput(T value)
	{
		_animation = Animation::None;
		_value = value;
		_input = nullptr;
		TryUpdateValue(0);
		return *this;
	}

	Animator<T>& SetInput(std::function<T()> input)
	{
		_animation = Animation::None;
		if (input)
			_value = input();
		_input = input;
		TryUpdateValue(0);
		return *this;
	}

	Animator<T>& SetInput(T value1, T value2, float duration, Animation animation)
	{
		_animation = animation;
		_duration = duration;
		_elapsed = 0;
		_value = value1;
		_value1 = value1;
		_value2 = value2;
		_input = nullptr;
		TryUpdateValue(0);
		return *this;
	}

	Animator<T>& SetOutput(std::function<void(T)> output)
	{
		_output = output;
		TryUpdateValue(0);
		return *this;
	}

private:
	void TryUpdateValue(float secondsSinceLastUpdate) override
	{
		_value_updated = false;
		if (_animation != Animation::None)
		{
			_elapsed += secondsSinceLastUpdate;
			if (_elapsed < _duration)
			{
				float k = AnimationFactor(_elapsed / _duration, _animation);
				T value = (1.0f - k) * _value1 + k * _value2;
				if (value != _value)
				{
					_value = value;
					_value_updated = true;
				}
			}
			else
			{
				if (_value2 != _value)
				{
					_value = _value2;
					_value_updated = true;
				}
				_animation = Animation::None;
			}
		}
		else if (_input)
		{
			if (_dependency.empty() || std::any_of(_dependency.begin(), _dependency.end(), [](AnimatorBase* a) { return a->_value_updated; }))
			{
				T value = _input();
				if (value != _value)
				{
					_value = value;
					_value_updated = true;
				}
			}
		}

		if (_value_updated && _output)
			_output(_value);
	}
};


class AnimatorHost
{
	static std::chrono::system_clock::time_point _lastUpdate;
	std::vector<AnimatorBase*> _animators;

public:
	AnimatorHost();
	virtual ~AnimatorHost();

	static void UpdateAll();

	template <class T>
	Animator<T>& AddAnimator(std::initializer_list<AnimatorBase*> dependency = std::initializer_list<AnimatorBase*>())
	{
		Animator<T>* animator = new Animator<T>();
		_animators.push_back(animator);
		return *animator;
	}
};


#endif
