// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Property_H
#define Property_H

#include <functional>
#include "Animation.h"


template <class T>
class Property
{
	std::function<T()> _getter;
	std::function<void(const T&)> _setter;
	std::function<void()> _observer;

public:
	Property(std::function<T()> getter, std::function<void(const T&)> setter) :
	_getter(getter),
	_setter(setter)
	{
	}

	~Property()
	{
		Animator::StopAnimation(this);
	}

	void AddObserver(std::function<void()> observer)
	{
		_observer = observer;
	}

	void CallObserver()
	{
		if (_observer)
			_observer();
	}

	T value() const
	{
		return _getter();
	}

	operator T() const
	{
		return _getter();
	}

	Property<T>& operator=(const Property<T>& other)
	{
		Animator::StopAnimation(this);
		_setter(other._getter());
		return *this;
	}

	Property<T>& operator=(const T& v)
	{
		Animator::StopAnimation(this);
		_setter(v);
		return *this;
	}

	Property<T>& operator=(const animation<T>& a)
	{
		Animator::StartAnimation(this, new PropertyAnimator<T>(a._animation->Clone(), _getter, _setter));
		return *this;
	}
};


#endif
