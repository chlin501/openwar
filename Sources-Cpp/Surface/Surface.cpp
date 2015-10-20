// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"

#include "View.h"
#include "Touch.h"
#include "Graphics/GraphicsContext.h"

#ifdef PHALANX_TARGET_UI_APPKIT
#import <AppKit/AppKit.h>
#endif
#ifdef PHALANX_TARGET_UI_UIKIT
#import <UIKit/UIKit.h>
#endif


Surface::Surface(GraphicsContext* gc) :
#ifdef PHALANX_TARGET_UI_APPKIT
	_nsview(nil),
#endif
#ifdef PHALANX_TARGET_UI_UIKIT
	_uiview(nil),
#endif
	_gc(gc)
{
}


Surface::~Surface()
{
}


GraphicsContext* Surface::GetGraphicsContext() const
{
	return _gc;
}


void Surface::SetNativeSize(glm::vec2 value)
{
	_nativeSize = value;
}


glm::vec2 Surface::GetNativeSize() const
{
	return _nativeSize;
}


glm::vec2 Surface::GetVirtualSize() const
{
	return _nativeSize / _gc->GetVirtualScaling();
}


bool Surface::IsUsingDepth() const
{
	return _usingDepth;
}


void Surface::SetUsingDepth(bool value)
{
	_usingDepth = value;
}


#ifdef PHALANX_TARGET_UI_APPKIT
void Surface::SetNSView(NSView* value)
{
    _nsview = value;
}
#endif


#ifdef PHALANX_TARGET_UI_APPKIT
NSView* Surface::GetNSView() const
{
	return _nsview;
}
#endif


#ifdef PHALANX_TARGET_UI_UIKIT
void Surface::SetUIView(UIView* value)
{
	_uiview = value;
}
#endif


#ifdef PHALANX_TARGET_UI_UIKIT
UIView* Surface::GetUIView() const
{
	return _uiview;
}
#endif


Surface* Surface::FindSurface()
{
	return this;
}
