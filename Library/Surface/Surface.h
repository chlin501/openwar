// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Surface_H
#define Surface_H

#include "View.h"
#include <vector>
#include <glm/glm.hpp>

class GraphicsContext;
class Touch;
class Viewport;

#ifdef PHALANX_TARGET_UI_APPKIT
@class NSView;
#endif
#ifdef PHALANX_TARGET_UI_UIKIT
@class UIView;
#endif


class Surface : public ViewOwner
{
#ifdef PHALANX_TARGET_UI_APPKIT
	NSView* _nsview;
#endif
#ifdef PHALANX_TARGET_UI_UIKIT
	UIView* _uiview;
#endif

	GraphicsContext* _gc;
	glm::vec2 _nativeSize;
	bool _usingDepth;

public:
	Surface(GraphicsContext* gc);
	virtual ~Surface();

	GraphicsContext* GetGraphicsContext() const;

	void SetNativeSize(glm::vec2 value);
	glm::vec2 GetNativeSize() const;
	glm::vec2 GetVirtualSize() const;

	bool IsUsingDepth() const;
	void SetUsingDepth(bool value);

#ifdef PHALANX_TARGET_UI_APPKIT
	void SetNSView(NSView* value);
	NSView* GetNSView() const;
#endif
#ifdef PHALANX_TARGET_UI_UIKIT
	void SetUIView(UIView* value);
	UIView* GetUIView() const;
#endif

private: // ViewOwner
	virtual Surface* FindSurface();
};


#endif
