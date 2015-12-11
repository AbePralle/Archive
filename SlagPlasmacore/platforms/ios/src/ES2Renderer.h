//
//  ES1Renderer.h
//  plasmacore
//
//  Created by Abe Pralle on 2/20/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import "ESRenderer.h"

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#include "gl_core.h"

@interface ES2Renderer : NSObject <ESRenderer>
{
@public
    EAGLContext *context;

    // The pixel dimensions of the CAEAGLLayer
    GLint backing_width;
    GLint backing_height;

    // The OpenGL ES names for the framebuffer and renderbuffer used to render to this view
    GLuint frame_buffer, color_render_buffer;
}

- (void)render;
- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer;

@end
