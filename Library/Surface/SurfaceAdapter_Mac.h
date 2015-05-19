// Copyright (C) 2015 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#import <GLKit/GLKit.h>
#include <glm/glm.hpp>

class GraphicsContext;
class Surface;
@protocol SurfaceFactory;



@interface SurfaceAdapter : NSOpenGLView

@property(nonatomic, retain) IBOutlet id<SurfaceFactory> surfaceFactory;

@end



@protocol SurfaceFactory <NSObject>

@required

- (Surface*)createSurfaceWithGraphicsContext:(GraphicsContext*)gc;

@end
