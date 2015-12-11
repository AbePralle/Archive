//
//  ES2Renderer.m
//  plasmacore
//
//  Created by Abe Pralle on 2/20/10.
//  Copyright Apple Inc 2010. All rights reserved.
//

#import "ES2Renderer.h"
#import "ios_core.h"
#include "plasmacore.h"
#include "mac_audio.h"

#include "gl_core.h"

ES2Renderer* renderer = 0;
int plasmacore_input_scale = 1;

extern bool plasmacore_allow_hires_iphone4;

@implementation ES2Renderer

// Create an OpenGL ES 2.0 context
- (id)init
{
    if (self = [super init])
    {
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

        if (!context || ![EAGLContext setCurrentContext:context])
        {
            [self release];
            return nil;
        }

        // Create default framebuffer object. The backing will be allocated for the current layer in -resizeFromLayer
        glGenFramebuffers(1, &frame_buffer);
        glGenRenderbuffers(1, &color_render_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        glBindRenderbuffer(GL_RENDERBUFFER, color_render_buffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_render_buffer);
    }

    plasmacore_configure_graphics();

    return self;
}

- (void)render
{
  if ( !plasmacore_running ) return;

  update_sounds();

  renderer = self;
  if ( plasmacore_update() )
  {
    plasmacore_draw();
  }

  int updates_per_draw = int(plasmacore.updates_per_second / plasmacore.target_fps);
  if (updates_per_draw != plasmacore_view.animation_frame_interval)
  {
    [plasmacore_view setAnimationFrameInterval:updates_per_draw];
  }
}

- (BOOL)resizeFromLayer:(CAEAGLLayer *)layer
{	
  if (plasmacore_allow_hires_iphone4 
      && [[[UIDevice currentDevice] systemVersion] floatValue] >= 3.2f)
  {
    int w = (int) [UIScreen mainScreen].currentMode.size.width;
    int h = (int) [UIScreen mainScreen].currentMode.size.height;
    if (w == 640 && h == 960)
    {
      plasmacore_view.contentScaleFactor = 2.0;
      plasmacore_view.layer.contentsScale = 2;
      plasmacore_input_scale = 2;
    }
  }

  // Allocate color buffer backing based on the current layer size
  glBindRenderbuffer(GL_RENDERBUFFER, color_render_buffer);
  [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer];

  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backing_width);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, 
      &backing_height);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
      NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
      return NO;
  }

  plasmacore_configure_graphics();

  return YES;
}

- (void)dealloc
{
    // Tear down GL
    if (frame_buffer)
    {
        glDeleteFramebuffers(1, &frame_buffer);
        frame_buffer = 0;
    }

    if (color_render_buffer)
    {
        glDeleteRenderbuffers(1, &color_render_buffer);
        color_render_buffer = 0;
    }

    // Tear down context
    if ([EAGLContext currentContext] == context)
        [EAGLContext setCurrentContext:nil];

    [context release];
    context = nil;

    [super dealloc];
}

@end


void NativeLayer_begin_draw()
{
  [EAGLContext setCurrentContext:renderer->context];

  draw_buffer.reset();

  glBindFramebuffer(GL_FRAMEBUFFER, renderer->frame_buffer);
  if (plasmacore.orientation == 1) glViewport(0, 0, plasmacore.display_height, plasmacore.display_width);
  else                             glViewport(0, 0, plasmacore.display_width, plasmacore.display_height);

  glDisable( GL_SCISSOR_TEST );

  // Clear the screen to Display.background.color unless that color's alpha=0.
  BARD_FIND_TYPE( type_display, "Display" );
  BARD_GET_INT32( argb, type_display->singleton(), "background_color" );
  int alpha = (argb >> 24) & 255;
  if (alpha)
  {
    glClearColor( ((argb>>16)&255)/255.0f,
        ((argb>>8)&255)/255.0f,
        ((argb)&255)/255.0f,
        alpha/255.0f );

    glClear(GL_COLOR_BUFFER_BIT);
  }

  // Prepare for drawing.
  glEnable( GL_BLEND );

  draw_buffer.set_draw_target( NULL );
}

void NativeLayer_end_draw()
{
  draw_buffer.render();

  glBindRenderbuffer(GL_RENDERBUFFER, renderer->color_render_buffer);
  [renderer->context presentRenderbuffer:GL_RENDERBUFFER];
}

