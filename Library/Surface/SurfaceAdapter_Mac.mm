// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#import "SurfaceAdapter_Mac.h"

#include "Dependency.h"
#include "Gesture.h"
#include "Surface.h"
#include "Touch.h"
#include "Graphics/GraphicsContext.h"
#include "Graphics/RenderLoopObserver.h"
#include "Graphics/Viewport.h"


@implementation SurfaceAdapter
{
	Surface* _surface;
	NSTimer* _timer;
	NSTimeInterval _timestamp;
	BOOL _transparent;
	Touch* _touch;
	bool _mouse;
}


- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format
{
	self = [super initWithFrame:frameRect pixelFormat:format];
	if (self != nil)
	{
		[self setWantsBestResolutionOpenGLSurface:YES];
	}
	return self;
}


- (id)initWithCoder:(NSCoder *)decoder
{
	self = [super initWithCoder:decoder];
	if (self)
	{
	}
	return self;
}


- (BOOL)isOpaque
{
	return !_transparent;
}


- (BOOL)isTransparent
{
	return _transparent;
}


- (void)setTransparent:(BOOL)value
{
	_transparent = value;

	GLint opacity = value ? 0 : 1;
	[self.openGLContext setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];
}


- (void)removeFromSuperview
{
	[super removeFromSuperview];
	[self removeTimer];
	[self removeSurface];
}


- (void)drawRect:(NSRect)dirtyRect
{
	[self.openGLContext makeCurrentContext];

	if (!_surface)
		[self createSurface];

    [self mouseEnterHoverLeave];
    
    NSTimeInterval timestamp = [NSProcessInfo processInfo].systemUptime;
    if (_timestamp == 0)
		_timestamp = timestamp;

    double secondsSinceLastUpdate = timestamp - _timestamp;
    RenderLoopObserver::NotifyRenderLoop(secondsSinceLastUpdate);
	_timestamp = timestamp;

    if (_touch)
    {
        double oldTimestamp = _touch->GetTimestamp();
        _touch->Update(timestamp);
        if (_touch->GetTimestamp() != oldTimestamp)
            _touch->TouchMoved();
    }

    DependencyBase::UpdateAll();

	if (_surface)
		_surface->RenderViews();

	glFinish();
}


- (void)createSurface
{
	if (_surfaceFactory)
	{
		GraphicsContext* gc = [self createGraphicsContext];
		_surface = [_surfaceFactory createSurfaceWithGraphicsContext:gc];
		_surface->SetNSView(self);

		CGSize size = self.bounds.size;
		_surface->SetNativeSize(glm::vec2(size.width, size.height));

		[self startTimer];
		[self setNeedsDisplay:YES];
	}
}


- (GraphicsContext*)createGraphicsContext
{
	float pixelDensity = (float)[self convertRectToBacking:CGRectMake(0, 0, 1, 1)].size.width;
	return new GraphicsContext(pixelDensity, 1);
}


- (void)removeSurface
{
	delete _surface;
	_surface = nullptr;
}


- (void)startTimer
{
	_timestamp = [NSProcessInfo processInfo].systemUptime;
	_timer = [NSTimer timerWithTimeInterval:1.0 / 30.0
					  target:self
					  selector:@selector(timerEvent:)
					  userInfo:nil
					  repeats:YES];

	[[NSRunLoop mainRunLoop] addTimer:_timer forMode:NSDefaultRunLoopMode];
}


- (void)removeTimer
{
	[_timer invalidate];
	_timer = nil;
}


- (void)timerEvent:(NSTimer*)t
{
	if (_surface)
	{
		[self setNeedsDisplay:YES];
	}
}


- (void)mouseEnterHoverLeave
{
	if (_surface)
	{
        //NSPoint p = [self convertPoint:[[self window] convertScreenToBase:[NSEvent mouseLocation]] fromView:nil];
        //bool inside = CGRectContainsPoint([self bounds], p);
        
		bool inside = CGRectContainsPoint([self.window convertRectToScreen:self.bounds], [NSEvent mouseLocation]);

		if (inside)
		{
			if (!_mouse)
			{
				//_surface->MouseEnter([self toVector:p]);
				_mouse = true;
			}
			else
			{
				//_surface->MouseHover([self toVector:p]);
			}
		}
		else if (_mouse)
		{
			//_surface->MouseLeave([self toVector:p]);
			_mouse = false;
		}
	}
}


- (void)update
{
	static bool _update = false;
	if (_update)
		return;

	_update = true;

	[super update];

	if (_surface)
	{
		[self.openGLContext makeCurrentContext];
		CGSize size = self.bounds.size;
		_surface->SetNativeSize(glm::vec2(size.width, size.height));
	}

	_update = false;
}


- (void)reshape
{
	[super reshape];

	if (_surface)
	{
		[self.openGLContext makeCurrentContext];
		CGSize size = self.bounds.size;
		_surface->SetNativeSize(glm::vec2(size.width, size.height));
	}
}


#pragma mark -


- (MouseButtons)currentMouseButtons
{
	return _touch ? _touch->GetCurrentButtons() : MouseButtons();
}


- (void)dispatchMouseEvent:(NSEvent*)event mouseButtons:(MouseButtons)mouseButtons
{
	[self.openGLContext makeCurrentContext];

	if (_touch == nullptr)
	{
		glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];
		_touch = new Touch((int)event.clickCount, position, event.timestamp, mouseButtons);
		_surface->NotifyViewsOfTouchBegin(_touch);
		_touch->TouchBegin();
		_touch->TouchBegan();

		[self acceptsFirstResponder];
	}
	else
	{
		glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];
		_touch->Update(position, event.timestamp, mouseButtons);

		if (_touch->GetCurrentButtons().Any())
			_touch->TouchMoved();
	}

	if (!_touch->GetCurrentButtons().Any())
	{
		_touch->TouchEnded();
		delete _touch;
		_touch = nullptr;
	}

	if (_surface)
		[self setNeedsDisplay:YES];
}


- (BOOL)mouseDownCanMoveWindow
{
	//return _transparent;
	return NO;
}


- (BOOL)acceptsFirstMouse:(NSEvent*)event
{
	//return !_transparent;
	return YES;
}


- (void)mouseDown:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	mouseButtons.left = true;
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)rightMouseDown:(NSEvent*)event
{
	if ([self menuForEvent:event] != nil)
	{
		[super rightMouseDown:event];
	}
	else
	{
		MouseButtons mouseButtons = [self currentMouseButtons];
		mouseButtons.right = true;
		[self dispatchMouseEvent:event mouseButtons:mouseButtons];
	}
}


- (void)otherMouseDown:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	mouseButtons.other = true;
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)mouseDragged:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)rightMouseDragged:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)otherMouseDragged:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)mouseUp:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	mouseButtons.left = false;
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)rightMouseUp:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	mouseButtons.right = false;
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)otherMouseUp:(NSEvent*)event
{
	MouseButtons mouseButtons = [self currentMouseButtons];
	mouseButtons.other = false;
	[self dispatchMouseEvent:event mouseButtons:mouseButtons];
}


- (void)scrollWheel:(NSEvent*)event
{
	glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];

	float k = [event hasPreciseScrollingDeltas] ? 1 : 3;

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->ScrollWheel(position, k * glm::vec2(event.deltaX, event.deltaY));
}


- (void)magnifyWithEvent:(NSEvent*)event
{
	glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->Magnify(position, (float)event.magnification);
}


- (void)smartMagnifyWithEvent:(NSEvent*)event
{
	glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->Magnify(position);
}


/*- (NSMenu*)menuForEvent:(NSEvent*)event
{
	if (event.type == NSRightMouseDown && _surface)
	{
		glm::vec2 position = [self toVector:[self convertPoint:event.locationInWindow fromView:nil]];
		if (!_surface->ShowContextualMenu(position))
			return nil;
	}

	return [super menuForEvent:event];
}*/


- (BOOL)acceptsFirstResponder
{
	return YES;
}


static char TranslateKeyCode(unsigned short keyCode)
{
	switch (keyCode)
	{
		case 12: return 'Q';
		case 13: return 'W';
		case 14: return 'E';
		case 0: return 'A';
		case 1: return 'S';
		case 2: return 'D';
		case 18: return '1';
		case 19: return '2';
		case 20: return '3';
		case 21: return '4';
		case 23: return '5';
		case 22: return '6';
		case 26: return '7';
		case 28: return '8';
		case 25: return '9';
		default: return '\0';
	}
}


- (void)keyDown:(NSEvent*)event
{
	char key = TranslateKeyCode(event.keyCode);
	if (key == '\0')
		return;

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->KeyDown(key);
}


- (void)keyUp:(NSEvent*)event
{
	char key = TranslateKeyCode(event.keyCode);
	if (key == '\0')
		return;

	if (Gesture::_gestures)
		for (Gesture* gesture : *Gesture::_gestures)
			gesture->KeyUp(key);
}


#pragma mark -


- (glm::vec2)toVector:(CGPoint)position
{
	if (!_surface)
		return glm::vec2();

	return glm::vec2(position.x, position.y) / _surface->GetGraphicsContext()->GetVirtualScaling();
}


@end
