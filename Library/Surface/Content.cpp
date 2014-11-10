// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include <glm/gtc/matrix_transform.hpp>
#include "Content.h"
#include "Surface.h"
#include "CommonShaders.h"
#include "Shapes/VertexShape.h"
#import "WidgetShape.h"
#import "WidgetViewport.h"



Content::Content(GraphicsContext* gc) :
	_gc(gc),
	_container(nullptr),
	_viewport(new WidgetViewport(gc)),
	_visible(true),
	_flip(false),
	_dismissed(false)
{
}


Content::~Content()
{
	SetContainer(nullptr, nullptr);
}


void Content::Dismiss()
{
	_dismissed = true;
}


bool Content::IsDismissed() const
{
	return _dismissed;
}


Container* Content::GetContainer() const
{
	return _container;
}


void Content::SetContainer(Container* value, Content* behindContent)
{
	SetContentContainer(this, value, behindContent);
}


WidgetViewport* Content::GetViewport() const
{
	return _viewport;
}


bool Content::IsVisible() const
{
	return _visible;
}


void Content::SetVisible(bool value)
{
	_visible = value;
}
