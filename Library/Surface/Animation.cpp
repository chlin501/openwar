// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Animation.h"
#include "Property.h"


int Animation::_objectCount = 0;
int Animator::_objectCount = 0;

std::map<void*, Animator*> Animator::_activeAnimators;
std::vector<Animator*> Animator::_stoppedAnimators;


Animation::Animation()
{
	++_objectCount;
	//NSLog(@"AnimationCount = %d", _objectCount);
}


Animation::~Animation()
{
	--_objectCount;
	//NSLog(@"AnimationCount = %d", _objectCount);
}


Animator::Animator(Animation* animation) :
_animation(animation)
{
	++_objectCount;
	//NSLog(@"AnimatorCount = %d", _objectCount);
}


Animator::~Animator()
{
	--_objectCount;
	//NSLog(@"AnimatorCount = %d", _objectCount);
}


void Animator::StartAnimation(void* key, Animator* animator)
{
	StopAnimation(key);
	_activeAnimators[key] = animator;
}


void Animator::StopAnimation(void* key)
{
	auto i = _activeAnimators.find(key);
	if (i != _activeAnimators.end())
	{
		_stoppedAnimators.push_back((*i).second);
		_activeAnimators.erase(i);
	}
}


void Animator::AnimateAll(float deltaTime)
{
	static std::vector<std::pair<void*, Animator*>> todoList;

	todoList.clear();
	for (std::pair<void*, Animator*> i : _activeAnimators)
		todoList.push_back(i);

	for (std::pair<void*, Animator*> todoItem : todoList)
	{
		auto i = _activeAnimators.find(todoItem.first);
		if (i != _activeAnimators.end() && (*i).second == todoItem.second)
		{
			if (todoItem.second->Animate(deltaTime) > 0)
				StopAnimation(todoItem.first);
		}
	}

	for (Animator* animator : _stoppedAnimators)
	{
		delete animator->_animation;
		delete animator;
	}

	_stoppedAnimators.clear();
}





/*
std::vector<animation_x**> animation_x::_animations;


void animation_x::update_all(float secondsSinceLastUpdate)
{
	for (auto i = _animations.begin(); i != _animations.end(); ++i)
	{
		animation_x** animation = *i;
		if (*animation != 0)
		{
			(*animation)->update(secondsSinceLastUpdate);
			if ((*animation)->is_done())
			{
				delete *animation;
				*animation = 0;
			}
		}
	}
}
*/


/***/


/*void animation_float::start()
{
}


void animation_float::update(float secondsSinceLastUpdate)
{
	_property->_value += _delta * secondsSinceLastUpdate;
}


bool animation_float::is_done()
{
	return false;
}*/


/***/


/*void animation_vector2_interpolate_linear::start()
{
	_bounds.min = _property->_value;
	_t = 0;
}


void animation_vector2_interpolate_linear::update(float secondsSinceLastUpdate)
{
	_t += secondsSinceLastUpdate / _duration;
	if (_t > 1)
		_t = 1;
	_property->_value = glm::mix(_bounds.min, _bounds.max, _t);
}


bool animation_vector2_interpolate_linear::is_done()
{
	return _t == 1;
}*/

/***/


/*void animation_vector2_interpolate_easeinout::start()
{
	_bounds.min = _property->_value;
	_t = 0;
}


void animation_vector2_interpolate_easeinout::update(float secondsSinceLastUpdate)
{
	_t += secondsSinceLastUpdate / _duration;
	if (_t > 1)
		_t = 1;
	_property->_value = glm::mix(_bounds.min, _bounds.max, glm::smoothstep(0.0f, 1.0f, _t));
}


bool animation_vector2_interpolate_easeinout::is_done()
{
	return _t == 1;
}*/
