//
//  MacDevTestView.m
//  MacDevTest
//
//  Created by Abraham Pralle on 1/3/16.
//  Copyright © 2016 Abe Pralle. All rights reserved.
//

#import "MacDevTestView.h"
#include "SuperCPP.h"
#include "GGBitmap.h"
using namespace SuperCPP;
using namespace GG;
using namespace Starbright;

@implementation MacDevTestView

- (int)loadImage:(NSString*)filename withExtension:(NSString*)extension
{
  int        texture_id;
  NSString*  filepath  = [[NSBundle mainBundle] pathForResource:filename ofType:extension];

  if ( !renderer ) return 0;

  NSImage*   ns_image = [[NSImage alloc] initWithContentsOfFile:filepath];
  CGImageRef cg_image = [ns_image CGImageForProposedRect:nil
                         context:[NSGraphicsContext currentContext] hints:nil];

  if ( !cg_image ) return 0;

  size_t width = CGImageGetWidth( cg_image );
  size_t height = CGImageGetHeight( cg_image );

  GG::Bitmap bitmap( 0, (int)width, (int)height, 32 );

  CGContextRef drawing_context = CGBitmapContextCreate( bitmap.data, width, height, 8, width*4,
      CGImageGetColorSpace(cg_image), kCGImageAlphaPremultipliedLast );
  CGContextDrawImage( drawing_context, CGRectMake(0, 0, width, height), cg_image );
  CGContextRelease( drawing_context );

  bitmap.expand_to_power_of_two();

  texture_id = renderer->define_texture( bitmap.data, bitmap.width, bitmap.height, 32 );

  return texture_id;
}

int img_new_years = 0;

-(void) onCreate
{
  img_new_years = [self loadImage:@"NewYearsEve" withExtension:@"jpg"];
  printf( "new year's image id:%d\n", img_new_years );
}

- (void)onDraw
{
  renderer->begin_draw( display_width, display_height );
  renderer->set_transform_2d( 0, 0, display_width-1, display_height-1 );

  if (((int)(Int64)current_time_seconds()) & 1) renderer->set_clear_color( 0xffff0000 );
  else                                          renderer->set_clear_color( 0xffffff00 );

  renderer->clear( Renderer::CLEAR_COLOR );

  renderer->set_render_mode( 
      RenderMode(
          RenderMode::BLEND_ONE, RenderMode::BLEND_ZERO,
          RenderMode::RENDER_TEXTURES
      )
  );

  renderer->set_texture_count( 1 );
  renderer->set_texture( 0, img_new_years );

  Vertex* v = renderer->add_vertices( 6 );
  v[0].x = 0;
  v[0].y = 0;
  v[1].x = display_width - 1;
  v[1].y = 0;
  v[2].x = display_width - 1;
  v[2].y = display_height - 1;
  v[3].x = 0;
  v[3].y = 0;
  v[4].x = display_width - 1;
  v[4].y = display_height - 1;
  v[5].x = 0;
  v[5].y = display_height - 1;

  v[0].u = 0;
  v[0].v = 0;
  v[1].u = 990.0/1024.0;
  v[1].v = 0;
  v[2].u = 990.0/1024.0;
  v[2].v = 660.0/1024.0;
  v[3].u = 0;
  v[3].v = 0;
  v[4].u = 990.0/1024.0;
  v[4].v = 660.0/1024.0;
  v[5].u = 0;
  v[5].v = 660.0/1024.0;

  renderer->end_draw();
}

//- (void)onUpdate
//{
//}


@end
