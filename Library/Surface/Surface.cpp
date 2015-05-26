// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"

#include "View.h"
#include "Touch.h"
#include "Graphics/GraphicsContext.h"

#ifdef OPENWAR_USE_SURFACE_ADAPTER_MAC
#import <AppKit/AppKit.h>
#endif
#ifdef OPENWAR_USE_SURFACE_ADAPTER_IOS
#import <UIKit/UIKit.h>
#endif


Surface::Surface(GraphicsContext* gc) :
#ifdef OPENWAR_USE_SURFACE_ADAPTER_MAC
	_nsview(nil),
#endif
#ifdef OPENWAR_USE_SURFACE_ADAPTER_IOS
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


#ifdef OPENWAR_USE_SURFACE_ADAPTER_MAC
void Surface::SetNSView(NSView* value)
{
    _nsview = value;
}
#endif


#ifdef OPENWAR_USE_SURFACE_ADAPTER_MAC
NSView* Surface::GetNSView() const
{
	return _nsview;
}
#endif


#ifdef OPENWAR_USE_SURFACE_ADAPTER_IOS
void Surface::SetUIView(UIView* value)
{
	_uiview = value;
}
#endif


#ifdef OPENWAR_USE_SURFACE_ADAPTER_IOS
UIView* Surface::GetUIView() const
{
	return _uiview;
}
#endif


Surface* Surface::FindSurface()
{
	return this;
}
