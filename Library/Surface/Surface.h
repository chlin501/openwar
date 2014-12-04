// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Surface_H
#define Surface_H

#include <vector>
#include <glm/glm.hpp>
#include "View.h"

class GraphicsContext;
class Touch;
class Viewport;

#ifdef ENABLE_SURFACE_ADAPTER_MAC
@class NSView;
#endif
#ifdef ENABLE_SURFACE_ADAPTER_IOS
@class UIView;
#endif


class Surface : public ViewOwner
{
#ifdef ENABLE_SURFACE_ADAPTER_MAC
	NSView* _nsview;
#endif
#ifdef ENABLE_SURFACE_ADAPTER_IOS
	UIView* _uiview;
#endif

	GraphicsContext* _gc;
	glm::vec2 _nativeSize;

public:
	Surface(GraphicsContext* gc);
	virtual ~Surface();

	GraphicsContext* GetGraphicsContext() const;

	void SetNativeSize(glm::vec2 value);
	glm::vec2 GetNativeSize() const;
	glm::vec2 GetVirtualSize() const;

#ifdef ENABLE_SURFACE_ADAPTER_MAC
	void SetNSView(NSView* value);
	NSView* GetNSView() const;
#endif
#ifdef ENABLE_SURFACE_ADAPTER_IOS
	void SetUIView(UIView* value);
	UIView* GetUIView() const;
#endif

private: // ViewOwner
	virtual Surface* FindSurface();

private:
	Surface(const Surface&) { }
	Surface& operator=(const Surface&) { return *this; }
};


#endif
