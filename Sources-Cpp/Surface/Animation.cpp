// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Animation.h"

#include <algorithm>


std::vector<DependencyBase*> DependencyBase::_all_dependencies;

std::vector<AnimationHost*> AnimationHost::_animation_hosts;
std::chrono::system_clock::time_point AnimationHost::_last_tick;


DependencyBase::DependencyBase()
{
	_all_dependencies.push_back(this);
}


DependencyBase::~DependencyBase()
{
	_all_dependencies.erase(
		std::remove(_all_dependencies.begin(), _all_dependencies.end(), this),
		_all_dependencies.end());
}


float DependencyBase::AnimationCurveFactor(float t, AnimationCurve animationCurve)
{
	switch (animationCurve)
	{
		case AnimationCurve::None:
			return 1.0f;

		case AnimationCurve::Delay:
			return t < 1.0f ? 0.0f : 1.0f;

		case AnimationCurve::Linear:
			return t;

		case AnimationCurve::EaseIn:
			return t * t;

		case AnimationCurve::EaseOut:
			t = 1.0f - t;
			return 1.0f - t * t;

		case AnimationCurve::EaseInOut:
			return t * t * (3.0f - 2.0f * t);
	}
}


AnimationHost::AnimationHost()
{
	if (_animation_hosts.empty())
		_last_tick = std::chrono::system_clock::now();

	_animation_hosts.push_back(this);
}


AnimationHost::~AnimationHost()
{
	for (DependencyBase* dependency : _dependencies)
		dependency->_dependencies.clear();

	for (DependencyBase* dependency : _dependencies)
		delete dependency;

	_animation_hosts.erase(
		std::remove(_animation_hosts.begin(), _animation_hosts.end(), this),
		_animation_hosts.end());

}


void AnimationHost::Tick()
{
	auto now = std::chrono::system_clock::now();

	float secondsSinceLastTick = 0.001f * std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_tick).count();
	if (secondsSinceLastTick < 0)
		secondsSinceLastTick = 0;
	if (secondsSinceLastTick > 1)
		secondsSinceLastTick = 1;

	for (DependencyBase* animator : DependencyBase::_all_dependencies)
		animator->TryUpdateValue(secondsSinceLastTick);

	for (size_t i = 0; i < _animation_hosts.size(); ++i)
		_animation_hosts[i]->Animate(secondsSinceLastTick);

	_last_tick = now;
}
