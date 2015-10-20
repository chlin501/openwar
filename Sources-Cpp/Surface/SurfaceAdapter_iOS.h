//
//  SurfaceAdapter.h
//  Lookahead-iOS
//
//  Created by Nicke on 2015-05-18.
//  Copyright (c) 2015 Nicke. All rights reserved.
//

#import <GLKit/GLKit.h>
#include <glm/glm.hpp>

class GraphicsContext;
class Surface;
@protocol SurfaceFactory;



@interface SurfaceAdapter : GLKView

@property(nonatomic, retain) IBOutlet id<SurfaceFactory> surfaceFactory;

@end



@protocol SurfaceFactory <NSObject>

@required

- (Surface*)createSurfaceWithGraphicsContext:(GraphicsContext*)gc;

@end
