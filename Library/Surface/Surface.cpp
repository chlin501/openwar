// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "Surface.h"
#include "View.h"
#include "Touch.h"
#include "Graphics/GraphicsContext.h"

#ifdef ENABLE_SURFACE_ADAPTER_MAC
#import <AppKit/AppKit.h>
#endif
#ifdef ENABLE_SURFACE_ADAPTER_IOS
#import <UIKit/UIKit.h>
#endif


Surface::Surface(GraphicsContext* gc) :
#ifdef ENABLE_SURFACE_ADAPTER_MAC
	_nsview(nil),
#endif
#ifdef ENABLE_SURFACE_ADAPTER_IOS
	_uiview(nil),
#endif
	_gc(gc)
{
}


Surface::~Surface()
{
#ifdef ENABLE_SURFACE_ADAPTER_MAC
	[_nsview release];
#endif
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


#ifdef ENABLE_SURFACE_ADAPTER_MAC
void Surface::SetNSView(NSView* value)
{
    _nsview = [value retain];
}
#endif


#ifdef ENABLE_SURFACE_ADAPTER_MAC
NSView* Surface::GetNSView() const
{
	return _nsview;
}
#endif


#ifdef ENABLE_SURFACE_ADAPTER_IOS
void Surface::SetUIView(UIView* value)
{
	_uiview = value;
}
#endif


#ifdef ENABLE_SURFACE_ADAPTER_IOS
UIView* Surface::GetUIView() const
{
	return _uiview;
}
#endif


Surface* Surface::FindSurface()
{
	return this;
}
