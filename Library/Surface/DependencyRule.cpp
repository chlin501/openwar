// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "DependencyRule.h"
#include <algorithm>


std::vector<DependencyBase*> DependencyBase::_dependencyRules;
std::chrono::system_clock::time_point DependencyHost::_lastUpdate;


DependencyBase::DependencyBase()
{
	_dependencyRules.push_back(this);
}


DependencyBase::~DependencyBase()
{
	_dependencyRules.erase(
		std::remove(_dependencyRules.begin(), _dependencyRules.end(), this),
		_dependencyRules.end());
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
	if (DependencyBase::_dependencyRules.empty())
		_lastUpdate = std::chrono::system_clock::now();
}


DependencyHost::~DependencyHost()
{
	for (DependencyBase* animator : _dependencyRules)
		animator->_dependencies.clear();

	for (DependencyBase* animator : _dependencyRules)
		delete animator;
}



void DependencyHost::UpdateAll()
{
	auto now = std::chrono::system_clock::now();

	float secondsSinceLastUpdate = 0.001f * std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastUpdate).count();
	if (secondsSinceLastUpdate < 0)
		secondsSinceLastUpdate = 0;
	if (secondsSinceLastUpdate > 1)
		secondsSinceLastUpdate = 1;

	for (DependencyBase* animator : DependencyBase::_dependencyRules)
		animator->TryUpdateValue(secondsSinceLastUpdate);

	_lastUpdate = now;
}
