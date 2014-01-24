// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ANIMATION_H
#define ANIMATION_H

//#import <Foundation/Foundation.h>

#include <algorithm>
#include <functional>
#include <map>
#include <vector>
#include <glm/glm.hpp>


struct Animation
{
	static int _objectCount;

	Animation();
	virtual ~Animation();

	virtual Animation* Clone() const = 0;

	virtual float GetDuration() const = 0;
};


/***/


template <class T>
struct AnimationValue : public Animation
{
	T _value;
	float _duration;

	AnimationValue(const T& value, float duration) :
	_value(value),
	_duration(duration)
	{
		//NSLog(@"%p AnimationValue(const T& value, float duration)", this);
	}

	AnimationValue(const AnimationValue& other) :
	_value(other._value),
	_duration(other._duration)
	{
		//NSLog(@"%p AnimationValue(const AnimationValue& other)", this);
	}

	virtual ~AnimationValue()
	{
		//NSLog(@"%p ~AnimationValue()", this);
	}

	virtual Animation* Clone() const
	{
		return new AnimationValue(*this);
	}

	virtual float GetDuration() const
	{
		return _duration;
	}

	T GetValue(float time)
	{
		return (time / _duration) * _value;
	}
};


/***/


struct AnimationAction : public Animation
{
	std::function<void()> _action;

	AnimationAction(std::function<void()> action) :
	_action(action)
	{
		//NSLog(@"%p AnimationAction(std::function<void()> action)", this);
	}

	AnimationAction(const AnimationAction& other) :
	_action(other._action)
	{
		//NSLog(@"%p AnimationAction(const AnimationAction& other)", this);
	}

	virtual ~AnimationAction()
	{
		//NSLog(@"%p ~AnimationAction()", this);
	}

	virtual Animation* Clone() const
	{
		return new AnimationAction(*this);
	}

	virtual float GetDuration() const
	{
		return 0;
	}
};


/***/


struct AnimationRepeat : public Animation
{
	Animation* _animation;
	int _count;

	AnimationRepeat(Animation* animation, int count) :
	_animation(animation),
	_count(count)
	{
		//NSLog(@"%p AnimationRepeat(Animation* animation, int count)", this);
	}

	AnimationRepeat(const AnimationRepeat& other) :
	_animation(other._animation->Clone()),
	_count(other._count)
	{
		//NSLog(@"%p AnimationRepeat(const AnimationRepeat& other)", this);
	}

	virtual ~AnimationRepeat()
	{
		//NSLog(@"%p ~AnimationRepeat()", this);
		delete _animation;
	}

	virtual Animation* Clone() const
	{
		return new AnimationRepeat(*this);
	}

	virtual float GetDuration() const
	{
		return _animation->GetDuration() * _count;
	}
};


/***/


struct AnimationSequence : public Animation
{
	std::vector<Animation*> _animations;

public:
	AnimationSequence(Animation* a1, Animation* a2)
	{
		//NSLog(@"%p AnimationSequence(Animation* a1, Animation* a2)", this);
		_animations.push_back(a1);
		_animations.push_back(a2);
	}

	AnimationSequence(const AnimationSequence& other)
	{
		//NSLog(@"%p AnimationSequence(const AnimationSequence& other)", this);
		for (Animation* animation : other._animations)
			_animations.push_back(animation->Clone());
	}

	virtual ~AnimationSequence()
	{
		//NSLog(@"%p ~AnimationSequence()", this);
		for (Animation* animation : _animations)
			delete animation;
	}

	virtual Animation* Clone() const
	{
		return new AnimationSequence(*this);
	}

	virtual float GetDuration() const
	{
		float result = 0;
		for (Animation* animation : _animations)
				result += animation->GetDuration();
		return result;
	}

};


/***/


class Animator
{
	static int _objectCount;
	static std::map<void*, Animator*> _activeAnimators;
	static std::vector<Animator*> _stoppedAnimators;

public:
	static void StartAnimation(void* key, Animator* animator);
	static void StopAnimation(void* key);

	static void AnimateAll(float deltaTime);

protected:
	Animation* _animation;

	Animator(Animation* animation);
	virtual ~Animator();

	virtual float Animate(float deltaTime) = 0;
};


template <class T>
class PropertyAnimator : public Animator
{
	std::function<T()> _getter;
	std::function<void(const T&)> _setter;
	AnimationValue<T>* _animationValue;
	AnimationAction* _animationAction;
	AnimationRepeat* _animationRepeat;
	AnimationSequence* _animationSequence;
	PropertyAnimator<T>* _animator;
	int _index;
	float _time;
	bool _started;
	T _startValue;

public:
	PropertyAnimator(Animation* animation, std::function<T()> getter, std::function<void(const T&)> setter) : Animator(animation),
	_getter(getter),
	_setter(setter),
	_animationValue(dynamic_cast<AnimationValue<T>*>(animation)),
	_animationAction(dynamic_cast<AnimationAction*>(animation)),
	_animationRepeat(dynamic_cast<AnimationRepeat*>(animation)),
	_animationSequence(dynamic_cast<AnimationSequence*>(animation)),
	_animator(nullptr),
	_index(0),
	_time(0),
	_started(false),
	_startValue()
	{
		//NSLog(@"%p PropertyAnimator(...)", this);
	}

protected:
	~PropertyAnimator()
	{
		//NSLog(@"%p ~PropertyAnimator()", this);
		delete _animator;
	}

	virtual float Animate(float deltaTime)
	{
		if (!_started)
		{
			if (_animationValue != nullptr)
				_startValue = _getter();

			if (_animationAction != nullptr)
				_animationAction->_action();

			_started = true;
		}

		if (_animationValue != nullptr)
		{
			float duration = _animationValue->_duration;
			_time += deltaTime;
			if (_time > duration)
			{
				deltaTime -= _time - duration;
				_time = duration;
			}
			else
			{
				deltaTime = 0;
			}

			float k = _time / duration;
			_setter((1.0f - k) * _startValue + k * _animationValue->_value);
		}

		if (_animationRepeat != nullptr)
		{
			while (_index < _animationRepeat->_count && deltaTime > 0)
			{
				if (_animator == nullptr)
					_animator = new PropertyAnimator<T>(_animationRepeat->_animation, _getter, _setter);

				deltaTime = _animator->Animate(deltaTime);
				if (deltaTime > 0)
				{
					delete _animator;
					_animator = nullptr;
					++_index;
				}

				_started = true;
			}
		}

		if (_animationSequence != nullptr)
		{
			while (_index < (int)_animationSequence->_animations.size() && deltaTime > 0)
			{
				if (_animator == nullptr)
					_animator = new PropertyAnimator<T>(_animationSequence->_animations[_index], _getter, _setter);

				deltaTime = _animator->Animate(deltaTime);
				if (deltaTime > 0)
				{
					delete _animator;
					_animator = nullptr;
					++_index;
				}
			}
		}

		return deltaTime;
	}


};


/***/


template <class T>
class animation
{
public:
	Animation* _animation;

private:
	animation(Animation* a) : _animation(a)
	{
	}

public:
	animation(const T& value, float duration) :
	_animation(new AnimationValue<T>(value, duration))
	{
	}

	animation(const animation<T>& other) :
	_animation(other._animation->Clone())
	{
	}

	~animation()
	{
		delete _animation;
	}

	animation<T>& operator=(const animation<T>& other)
	{
		if (&other != this)
			_animation = other._animation->Clone();
		return *this;
	}

	animation<T> operator+(const animation<T>& a) const
	{
		return animation<T>(new AnimationSequence(_animation->Clone(), a._animation->Clone()));

	}

	animation<T> operator+(std::function<void()> action) const
	{
		return animation<T>(new AnimationSequence(_animation->Clone(), new AnimationAction(action)));
	}

	animation<T> repeat(int count = 0) const
	{
		return animation<T>(new AnimationRepeat(_animation, count));
	}
};


#endif
