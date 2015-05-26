// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Dependency.h"

#include <algorithm>


std::vector<DependencyBase*> DependencyBase::_all_dependencies;
std::chrono::system_clock::time_point DependencyBase::_last_update;


DependencyBase::DependencyBase()
{
	if (_all_dependencies.empty())
		_last_update = std::chrono::system_clock::now();

	_all_dependencies.push_back(this);
}


DependencyBase::~DependencyBase()
{
	_all_dependencies.erase(
		std::remove(_all_dependencies.begin(), _all_dependencies.end(), this),
		_all_dependencies.end());
}




void DependencyBase::UpdateAll()
{
	auto now = std::chrono::system_clock::now();

	float secondsSinceLastUpdate = 0.001f * std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_update).count();
	if (secondsSinceLastUpdate < 0)
		secondsSinceLastUpdate = 0;
	if (secondsSinceLastUpdate > 1)
		secondsSinceLastUpdate = 1;

	for (DependencyBase* animator : DependencyBase::_all_dependencies)
		animator->TryUpdateValue(secondsSinceLastUpdate);

	_last_update = now;
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


DependencyHost::DependencyHost()
{
}


DependencyHost::~DependencyHost()
{
	for (DependencyBase* animator : _dependencies)
		animator->_dependencies.clear();

	for (DependencyBase* animator : _dependencies)
		delete animator;
}
