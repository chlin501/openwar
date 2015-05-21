//
//  SurfaceAdapter.m
//  Lookahead-iOS
//
//  Created by Nicke on 2015-05-18.
//  Copyright (c) 2015 Nicke. All rights reserved.
//

#import "SurfaceAdapter_iOS.h"

#include "Graphics/GraphicsContext.h"
#include "Graphics/RenderLoopObserver.h"
#include "Animator.h"
#include "Gesture.h"
#include "Surface.h"
#include "Touch.h"
#include "Viewport.h"



@implementation SurfaceAdapter
{
	Surface* _surface;
	NSTimer* _timer;
	NSTimeInterval _timestamp;
	std::map<UITouch*, Touch*> _touches;
}


- (instancetype)initWithFrame:(CGRect)frame context:(EAGLContext *)context
{
    self = [super initWithFrame:frame context:context];
	if (self)
	{
		self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		self.enableSetNeedsDisplay = YES;
		[self setNeedsDisplay];
	}
	return self;
}



- (instancetype)initWithCoder:(NSCoder *)decoder
{
    self = [super initWithCoder:decoder];
    if (self)
    {
		self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		self.enableSetNeedsDisplay = YES;
		[self setNeedsDisplay];
    }
    return self;
}


- (void)removeFromSuperview
{
	[super removeFromSuperview];
	[self removeTimer];
	[self removeSurface];
}


- (void)drawRect:(CGRect)rect
{
	[EAGLContext setCurrentContext:self.context];

	if (!_surface)
		[self createSurface];

	AnimatorHost::UpdateAll();

	if (_surface)
		_surface->RenderViews();
}


- (void)createSurface
{
	if (_surfaceFactory)
	{
		GraphicsContext* gc = [self createGraphicsContext];
		_surface = [_surfaceFactory createSurfaceWithGraphicsContext:gc];
		_surface->SetUIView(self);

		CGSize size = self.bounds.size;
		_surface->SetNativeSize(glm::vec2(size.width, size.height));

		[self startTimer];
		[self setNeedsDisplay];
	}
}


- (GraphicsContext*)createGraphicsContext
{
	UIScreen* screen = [UIScreen mainScreen];

	/*CGRect bounds = [screen bounds]; // points
	 CGRect  nativeBounds = CGRectMake(0, 0, 0, 0); // pixels
	 if ([screen respondsToSelector:@selector(nativeBounds)])
	 nativeBounds = [screen nativeBounds];*/

	CGFloat pixelDensity = [screen scale];

	CGFloat nativeScale = 0;
	if ([screen respondsToSelector:@selector(nativeScale)])
		nativeScale = [screen nativeScale];

	if (nativeScale != 0)
		pixelDensity = nativeScale;

	return new GraphicsContext(pixelDensity, 1);
}


- (void)removeSurface
{
	delete _surface;
	_surface = nullptr;
}


- (void)startTimer
{
	_timestamp = [NSDate timeIntervalSinceReferenceDate];
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
		//[self mouseEnterHoverLeave];

		[EAGLContext setCurrentContext:self.context];

		NSTimeInterval timestamp = t.fireDate.timeIntervalSinceReferenceDate;
		double secondsSinceLastUpdate = timestamp - _timestamp;
		RenderLoopObserver::NotifyRenderLoop(secondsSinceLastUpdate);

		_timestamp = timestamp;

		[self setNeedsDisplay];
	}
}



- (void)createTouch:(UITouch*)original
{
	glm::vec2 position = [self toVector:[original locationInView:self]];
	_touches[original] = new Touch(original.tapCount, position, original.timestamp, MouseButtons());
}


- (void)updateTouch:(UITouch*)original
{
	Touch* touch = _touches[original];
	if (touch != nullptr)
	{
		glm::vec2 position = [self toVector:[original locationInView:self]];
		glm::vec2 previous = [self toVector:[original previousLocationInView:self]];
		touch->Update(position, previous, original.timestamp);
	}
}


- (void)deleteTouch:(UITouch*)original
{
	Touch* touch = _touches[original];
	if (touch != nullptr)
	{
		delete touch;
		_touches.erase(original);
	}
}


- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface)
	{
		for (UITouch* original in touches)
			if (original.view == self)
				[self createTouch:original];

		for (UITouch* original in touches)
			if (original.view == self)
				_surface->NotifyViewsOfTouchBegin(_touches[original]);

		for (UITouch* original in touches)
			if (original.view == self)
				_touches[original]->TouchBegin();

		for (UITouch* original in touches)
			if (original.view == self)
				_touches[original]->TouchBegan();

		[self setNeedsDisplay];
	}
}


- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface)
	{
		for (UITouch* original in touches)
			if (original.view == self)
				[self updateTouch:original];

		for (UITouch* original in touches)
			if (original.view == self)
				_touches[original]->TouchMoved();

		[self setNeedsDisplay];
	}
}


- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface)
	{
		for (UITouch* original in touches)
			if (original.view == self)
				[self updateTouch:original];

		for (UITouch* original in touches)
			if (original.view == self)
				_touches[original]->TouchEnded();

		for (UITouch* original in touches)
			if (original.view == self)
				[self deleteTouch:original];

		[self setNeedsDisplay];
	}
}


- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (_surface)
	{
		for (UITouch* original in touches)
			if (original.view == self)
				[self deleteTouch:original];

		[self setNeedsDisplay];
	}
}


- (glm::vec2)toVector:(CGPoint)position
{
	return glm::vec2(position.x, self.bounds.size.height - position.y);
}



@end
