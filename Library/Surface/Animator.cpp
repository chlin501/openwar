// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Animator.h"
#include <algorithm>


std::vector<AnimatorBase*> AnimatorBase::_animators;
std::chrono::system_clock::time_point AnimatorHost::_lastUpdate;


AnimatorBase::AnimatorBase()
{
	_animators.push_back(this);
}


AnimatorBase::~AnimatorBase()
{
	_animators.erase(
		std::remove(_animators.begin(), _animators.end(), this),
		_animators.end());
}


float AnimatorBase::AnimationFactor(float k, Animation animation)
{
	switch (animation)
	{
		case Animation::None:
			return 1.0f;

		case Animation::Delay:
			return k < 1.0f ? 0.0f : 1.0f;

		case Animation::Linear:
			return k;

		case Animation::EaseIn:
			return k * k;

		case Animation::EaseOut:
			k = 1.0f - k;
			return 1.0f - k * k;

		case Animation::EaseInOut:
			return k * k * (3.0f - 2.0f * k);
	}
}


AnimatorHost::AnimatorHost()
{
	if (AnimatorBase::_animators.empty())
		_lastUpdate = std::chrono::system_clock::now();
}


AnimatorHost::~AnimatorHost()
{
	for (AnimatorBase* animator : _animators)
		animator->_dependency.clear();

	for (AnimatorBase* animator : _animators)
		delete animator;
}



void AnimatorHost::UpdateAll()
{
	auto now = std::chrono::system_clock::now();

	float secondsSinceLastUpdate = 0.001f * std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastUpdate).count();
	if (secondsSinceLastUpdate < 0)
		secondsSinceLastUpdate = 0;
	if (secondsSinceLastUpdate > 1)
		secondsSinceLastUpdate = 1;

	for (AnimatorBase* animator : AnimatorBase::_animators)
		animator->TryUpdateValue(secondsSinceLastUpdate);

	_lastUpdate = now;
}
