// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef DependencyRule_H
#define DependencyRule_H

#include <chrono>
#include <functional>
#include <initializer_list>
#include <vector>


enum class AnimationCurve
{
	None,
	Delay,
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut
};


class DependencyBase
{
	friend class DependencyHost;

	static std::vector<DependencyBase*> _dependencyRules;

protected:
	std::vector<DependencyBase*> _dependencies;
	AnimationCurve _animationCurve{};
	float _duration{};
	float _elapsed{};
	bool _value_updated{};

protected:
	DependencyBase();
	virtual ~DependencyBase();

	static float AnimationCurveFactor(float t, AnimationCurve animationCurve);

public:
	DependencyBase(const DependencyBase&) = delete;
	DependencyBase& operator=(const DependencyBase&) = delete;

private:
	virtual void TryUpdateValue(float secondsSinceLastUpdate) = 0;
};


template <class T>
class DependencyRule : public DependencyBase
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

	DependencyRule<T>& SetTarget(std::function<void(T)> output)
	{
		_output = output;
		TryUpdateValue(0);
		return *this;
	}

	DependencyRule<T>& SetSource(std::function<T()> input)
	{
		_animationCurve = AnimationCurve::None;
		if (input)
			_value = input();
		_input = input;
		TryUpdateValue(0);
		return *this;
	}

	DependencyRule<T>& SetValue(T value)
	{
		_animationCurve = AnimationCurve::None;
		_value = value;
		_input = nullptr;
		TryUpdateValue(0);
		return *this;
	}

	DependencyRule<T>& Animate(T value1, T value2, float duration, AnimationCurve animationCurve)
	{
		_animationCurve = animationCurve;
		_duration = duration;
		_elapsed = 0;
		_value = value1;
		_value1 = value1;
		_value2 = value2;
		_input = nullptr;
		TryUpdateValue(0);
		return *this;
	}

private:
	void TryUpdateValue(float secondsSinceLastUpdate) override
	{
		_value_updated = false;
		if (_animationCurve != AnimationCurve::None)
		{
			_elapsed += secondsSinceLastUpdate;
			if (_elapsed < _duration)
			{
				float k = AnimationCurveFactor(_elapsed / _duration, _animationCurve);
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
				_animationCurve = AnimationCurve::None;
			}
		}
		else if (_input)
		{
			if (_dependencies.empty() || std::any_of(_dependencies.begin(), _dependencies.end(), [](DependencyBase* a) { return a->_value_updated; }))
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


class DependencyHost
{
	static std::chrono::system_clock::time_point _lastUpdate;
	std::vector<DependencyBase*> _dependencyRules;

public:
	DependencyHost();
	virtual ~DependencyHost();

	static void UpdateAll();

	template <class T>
	DependencyRule<T>& AddDependencyRule(std::initializer_list<DependencyBase*> dependencies = std::initializer_list<DependencyBase*>())
	{
		DependencyRule<T>* dependenecyRule = new DependencyRule<T>();
		_dependencyRules.push_back(dependenecyRule);
		return *dependenecyRule;
	}
};


#endif
