//=============================================================================
// mac_core.mm
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Plasmacore Native Layer implementation for Mac.
// 
// ----------------------------------------------------------------------------
//
// $(COPYRIGHT)
//
//   http://plasmaworks.com/plasmacore
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
//   http://www.apache.org/licenses/LICENSE-2.0 
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an 
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
// either express or implied. See the License for the specific 
// language governing permissions and limitations under the License.
//
//=============================================================================

#import "mac_core.h"
#include <OpenGL/gl.h>
#import <QuartzCore/QuartzCore.h>
#import <AppKit/NSAlert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "unzip.h"
#include "gd.h"
#include "png.h"
#include "bard_stdlib.h"

#include "mac_audio.h"
#include "setjmp.h"

// PROTOTYPES
GLTexture* NativeLayer_get_native_texture_data( BardObject* texture_obj );

// GLOBALS
PlasmacoreWindow* plasmacore_window = nil;
PlasmacoreView*   plasmacore_view   = nil;

Archive data_archive( "data.zip" );
Archive image_archive( "images.zip" );
Archive sound_archive( "sounds.zip" );

int key_modifier_flags = 0;

int alpha_to_gd_alpha_map[256] =
{
  // Typical Alpha is 255=opaque, 0=Transparent.  GD uses 0=Opaque, 127=Transparent.
  127,127, 126,126, 125,125, 124,124, 123,123, 122,122, 121,121, 120,120, 119,119,
  118,118, 117,117, 116,116, 115,115, 114,114, 113,113, 112,112, 111,111, 110,110,
  109,109, 108,108, 107,107, 106,106, 105,105, 104,104, 103,103, 102,102, 101,101,
  100,100, 99,99, 98,98, 97,97, 96,96, 95,95, 94,94, 93,93, 92,92, 91,91, 90,90,
  89,89, 88,88, 87,87, 86,86, 85,85, 84,84, 83,83, 82,82, 81,81, 80,80, 79,79,
  78,78, 77,77, 76,76, 75,75, 74,74, 73,73, 72,72, 71,71, 70,70, 69,69, 68,68,
  67,67, 66,66, 65,65, 64,64, 63,63, 62,62, 61,61, 60,60, 59,59, 58,58, 57,57,
  56,56, 55,55, 54,54, 53,53, 52,52, 51,51, 50,50, 49,49, 48,48, 47,47, 46,46,
  45,45, 44,44, 43,43, 42,42, 41,41, 40,40, 39,39, 38,38, 37,37, 36,36, 35,35,
  34,34, 33,33, 32,32, 31,31, 30,30, 29,29, 28,28, 27,27, 26,26, 25,25, 24,24,
  23,23, 22,22, 21,21, 20,20, 19,19, 18,18, 17,17, 16,16, 15,15, 14,14, 13,13,
  12,12, 11,11, 10,10, 9,9, 8,8, 7,7, 6,6, 5,5, 4,4, 3,3, 2,2, 1,1, 0,0
};

int gd_alpha_to_alpha_map[128] =
{
  // Typical Alpha is 255=opaque, 0=Transparent.  GD uses 0=Opaque, 127=Transparent.
  255, 253, 251, 249, 247, 245, 243, 241, 239, 237, 235, 233, 231, 229, 227, 225,
  223, 221, 219, 217, 215, 213, 211, 209, 207, 205, 203, 201, 199, 197, 195, 193,
  191, 189, 187, 185, 183, 181, 179, 177, 175, 173, 171, 169, 167, 165, 163, 161,
  159, 157, 155, 153, 151, 149, 147, 145, 143, 141, 139, 137, 135, 133, 131, 129,
  127, 125, 123, 121, 119, 117, 115, 113, 111, 109, 107, 105, 103, 101, 99,   97,
  95, 93, 91, 89, 87, 85, 83, 81, 79, 77, 75, 73, 71, 69, 67, 65, 63, 61, 59, 57,
  55, 53, 51, 49, 47, 45, 43, 41, 39, 37, 35, 33, 31, 29, 27, 25, 23, 21, 19, 17,
  15, 13, 11, 9, 7, 5, 3, 0
};

int cocoa_to_plasmacore_keycode_map[128] =
{
   97,  //   0 -> a
  115,  //   1 -> s
  100,  //   2 -> d
  102,  //   3 -> f
  104,  //   4 -> h
  103,  //   5 -> g
  122,  //   6 -> z
  120,  //   7 -> x
   99,  //   8 -> c
  118,  //   9 -> v
    0,  //  10 -> (unknown)
   98,  //  11 -> b
  113,  //  12 -> q
  119,  //  13 -> w
  101,  //  14 -> e
  114,  //  15 -> r
  121,  //  16 -> y
  116,  //  17 -> t
   49,  //  18 -> 1
   50,  //  19 -> 2
   51,  //  20 -> 3
   52,  //  21 -> 4
   54,  //  22 -> 6
   53,  //  23 -> 5
   61,  //  24 -> EQUALS
   57,  //  25 -> 9
   55,  //  26 -> 7
   45,  //  27 -> MINUS
   56,  //  28 -> 8
   48,  //  29 -> 0
   93,  //  30 -> ]
  111,  //  31 -> o
  117,  //  32 -> u
   91,  //  33 -> [
  105,  //  34 -> i
  112,  //  35 -> p
   13,  //  36 -> ENTER
  108,  //  37 -> l
  106,  //  38 -> j
   34,  //  39 -> '
  107,  //  40 -> k
   59,  //  41 -> ;
   92,  //  42 -> '\'
   44,  //  43 -> ,
   47,  //  44 -> /
  110,  //  45 -> n
  109,  //  46 -> m
   46,  //  47 -> .
    9,  //  48 -> TAB
   32,  //  49 -> SPACE
   96,  //  50 -> grave (back quote)
    8,  //  51 -> BACKSPACE
    0,  //  52 -> (unknown)
   27,  //  53 -> ESC
    0,  //  54 -> (unknown)
    0,  //  55 -> (unknown)
    0,  //  56 -> (unknown)
    0,  //  57 -> (unknown)
    0,  //  58 -> (unknown)
    0,  //  59 -> (unknown)
    0,  //  60 -> (unknown)
    0,  //  61 -> (unknown)
    0,  //  62 -> (unknown)
    0,  //  63 -> (unknown)
  327,  //  64 -> F17
  266,  //  65 -> NUMPAD .
    0,  //  66 -> (unknown)
  268,  //  67 -> NUMPAD *
    0,  //  68 -> (unknown)
  270,  //  69 -> NUMPAD +
    0,  //  70 -> (unknown)
  297,  //  71 -> CLEAR (Mac)
    0,  //  72 -> (unknown)
    0,  //  73 -> (unknown)
    0,  //  74 -> (unknown)
  267,  //  75 -> NUMPAD /
    0,  //  76 -> (unknown)
    0,  //  77 -> (unknown)
  269,  //  78 -> NUMPAD -
  328,  //  79 -> F18
  329,  //  80 -> F19
  272,  //  81 -> NUMPAD =
  256,  //  82 -> NUMPAD 0
  257,  //  83 -> NUMPAD 1
  258,  //  84 -> NUMPAD 2
  259,  //  85 -> NUMPAD 3
  260,  //  86 -> NUMPAD 4
  261,  //  87 -> NUMPAD 5
  262,  //  88 -> NUMPAD 6
  263,  //  89 -> NUMPAD 7
    0,  //  90 -> (unknown)
  264,  //  91 -> NUMPAD 8
  265,  //  92 -> NUMPAD 9
    0,  //  93 -> (unknown)
    0,  //  94 -> (unknown)
    0,  //  95 -> (unknown)
  286,  //  96 -> F5
    0,  //  97 -> (unknown)
    0,  //  98 -> (unknown)
  284,  //  99 -> F3
    0,  // 100 -> (unknown)
    0,  // 101 -> (unknown)
    0,  // 102 -> (unknown)
    0,  // 103 -> (unknown)
    0,  // 104 -> (unknown)
  294,  // 105 -> F13
  326,  // 106 -> F16
  295,  // 107 -> F14
    0,  // 108 -> (unknown)
    0,  // 109 -> (unknown)
    0,  // 110 -> (unknown)
    0,  // 111 -> (unknown)
    0,  // 112 -> (unknown)
  296,  // 113 -> F15
    0,  // 114 -> (unknown)
  278,  // 115 -> HOME
  280,  // 116 -> PAGE UP
  127,  // 117 -> DELETE
  285,  // 118 -> F4
  279,  // 119 -> END
  120,  // 120 -> F2
  281,  // 121 -> PAGE DOWN
  282,  // 122 -> F1
  276,  // 123 -> LEFT ARROW
  275,  // 124 -> RIGHT ARROW
  274,  // 125 -> DOWN ARROW
  273,  // 126 -> UP ARROW
    0,  // 127 -> (unknown)
};

//=============================================================================
//  PlasmacoreWindow
//=============================================================================
@implementation PlasmacoreWindow
- (BOOL) acceptsMouseMovedEvents
{
  return YES;
}

//-----------------------------------------------------------------------------
//  Input Events
//-----------------------------------------------------------------------------
- (Vector2) transformedPos:(NSEvent*)event
{
  NSPoint point = [event locationInWindow];
  double max_y = (plasmacore.display_height - 1);
  Vector2 pos( point.x, max_y - point.y );
  return pos;
}

- (void) mouseMoved:(NSEvent*)event
{
  Vector2 pos = [self transformedPos:event];
  plasmacore_queue_signal( plasmacore.event_mouse_move, 1, 0, false, pos.x, pos.y );
}

- (void) mouseDragged:(NSEvent*)event
{
  Vector2 pos = [self transformedPos:event];
  plasmacore_queue_signal( plasmacore.event_mouse_move, 1, 0, false, pos.x, pos.y );
}

- (void) rightMouseDragged:(NSEvent*)event
{
  Vector2 pos = [self transformedPos:event];
  plasmacore_queue_signal( plasmacore.event_mouse_move, 1, 0, false, pos.x, pos.y );
}

- (void) mouseDown:(NSEvent*)event
{
  Vector2 pos = [self transformedPos:event];
  plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 1, true, pos.x, pos.y );
}

- (void) mouseUp:(NSEvent*)event
{
  Vector2 pos = [self transformedPos:event];
  plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 1, false, pos.x, pos.y );
}

- (void) scrollWheel:(NSEvent*)event
{
  Vector2 pos = [self transformedPos:event];
  double dy = [event deltaY];
  int scroll_dir = 0;
  if (dy < -0.2)     scroll_dir =  1;
  else if (dy > 0.2) scroll_dir = -1;
  if (scroll_dir)
  {
    plasmacore_queue_signal( plasmacore.event_mouse_wheel, 1, scroll_dir, false, pos.x, pos.y );
  }
}

- (void) keyDown:(NSEvent*)event
{
  if ( ![event isARepeat] )
  {
    int keycode = [event keyCode] & 0x7f;
    keycode = cocoa_to_plasmacore_keycode_map[ keycode ];
    plasmacore_queue_signal( plasmacore.event_key, 0, keycode, true );
  }
}

- (void) keyUp:(NSEvent*)event
{
  if ( ![event isARepeat] )
  {
    int keycode = [event keyCode] & 0x7f;
    keycode = cocoa_to_plasmacore_keycode_map[ keycode ];
    plasmacore_queue_signal( plasmacore.event_key, 0, keycode, false );
  }
}

- (void) handleModifiedKey:(int)modified withMask:(int)mask plasmacoreKeyCode:(int)key_code
{
  if ((modified & mask) == mask)
  {
    bool pressed = !(key_modifier_flags & mask);
    plasmacore_queue_signal( plasmacore.event_key, 0, key_code, pressed );
  }
}

- (void) flagsChanged:(NSEvent*)event
{
  int new_flags = [event modifierFlags];
  int modified = key_modifier_flags ^ new_flags;

  // Caps Lock
  [self handleModifiedKey:modified withMask:NSAlphaShiftKeyMask plasmacoreKeyCode:301];

  // Shift
  [self handleModifiedKey:modified withMask:(NSShiftKeyMask|0x2) plasmacoreKeyCode:304]; // Left SHIFT
  [self handleModifiedKey:modified withMask:(NSShiftKeyMask|0x4) plasmacoreKeyCode:303]; // Right SHIFT

  // Control
  [self handleModifiedKey:modified withMask:(NSControlKeyMask|0x1)    plasmacoreKeyCode:306]; // Left CTRL
  [self handleModifiedKey:modified withMask:(NSControlKeyMask|0x2000) plasmacoreKeyCode:305]; // Right CTRL

  // Alt
  [self handleModifiedKey:modified withMask:(NSAlternateKeyMask|0x20) plasmacoreKeyCode:308]; // Left ALT
  [self handleModifiedKey:modified withMask:(NSAlternateKeyMask|0x40) plasmacoreKeyCode:307]; // Right ALT

  // Command
  [self handleModifiedKey:modified withMask:(NSCommandKeyMask|0x08) plasmacoreKeyCode:310]; // Left COMMAND
  [self handleModifiedKey:modified withMask:(NSCommandKeyMask|0x10) plasmacoreKeyCode:309]; // Right COMMAND

  // Numpad ENTER
  [self handleModifiedKey:modified withMask:NSNumericPadKeyMask plasmacoreKeyCode:271];

  key_modifier_flags = new_flags;
}

@end


//=============================================================================
//  PlasmacoreView
//=============================================================================
@implementation PlasmacoreView
- (void) prepareOpenGL
{
  [super prepareOpenGL];
  draw_buffer.reset();
  plasmacore_configure_graphics();
  shader_list.reset_shaders();
}

- (void) setDefaults
{
  // Sets up matrices and transforms for OpenGL

  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

  // Sets up pointers and enables states needed for using vertex arrays and textures
  glClientActiveTexture(GL_TEXTURE0);
  //glVertexPointer( 2, GL_FLOAT, 0, draw_buffer.vertices );
  //glTexCoordPointer( 2, GL_FLOAT, 0, draw_buffer.uv);
  //glColorPointer( 4, GL_UNSIGNED_BYTE, 0, draw_buffer.colors);
  //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glEnableClientState(GL_VERTEX_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);

  //glClientActiveTexture(GL_TEXTURE1);
  //glTexCoordPointer( 2, GL_FLOAT, 0, draw_buffer.alpha_uv);
  //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  //glClientActiveTexture(GL_TEXTURE0);

  // enable vertical blank synchronization
  GLint swap_interval = 1;
  CGLSetParameter( CGLGetCurrentContext(), kCGLCPSwapInterval, &swap_interval );

  NativeLayer_set_projection_transform();
  //glMatrixMode(GL_MODELVIEW);
  //glLoadIdentity();
}

- (void) drawRect: (NSRect) bounds
{
  [self setDefaults];
  plasmacore_draw();
  [[self openGLContext] flushBuffer];
}

//-----------------------------------------------------------------------------
//  Input Events
//-----------------------------------------------------------------------------
- (void) rightMouseDown:(NSEvent*)event
{
  Vector2 pos = [plasmacore_window transformedPos:event];
  plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 2, true, pos.x, pos.y );
}

- (void) rightMouseUp:(NSEvent*)event
{
  Vector2 pos = [plasmacore_window transformedPos:event];
  plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 2, false, pos.x, pos.y );
}

@end


//=============================================================================
//  PlasmacoreAppDelegate
//=============================================================================
@implementation PlasmacoreAppDelegate

- (void) performCustomSetup
{
  // Override if desired.
}

- (PlasmacoreView*) createPlasmacoreView:(NSRect)bounds withPixelFormat:(NSOpenGLPixelFormat*)pixel_format
{
  return [[PlasmacoreView alloc] initWithFrame:bounds pixelFormat:pixel_format];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
  [self initPlasmacore];
  [self performCustomSetup];
  [self configurePlasmacore];

  NSTimeInterval timeInterval = 1.0 / 60.0;
  NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:timeInterval
                            target:self
                            selector:@selector(update)
                            userInfo:nil
                            repeats:YES];
  [[NSRunLoop currentRunLoop] addTimer:timer
                              forMode:NSEventTrackingRunLoopMode];
  [[NSRunLoop currentRunLoop] addTimer:timer
                              forMode:NSModalPanelRunLoopMode];
}

- (void) initPlasmacore
{
  // init filesystem
#if !defined(PLASMACORE_NO_CHDIR)
  [[NSFileManager defaultManager] 
      changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];
#endif
  [[NSFileManager defaultManager] createDirectoryAtPath:@"save"
      withIntermediateDirectories:NO 
      attributes:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:0755],
        NSFilePosixPermissions,nil]
      error:NULL];

  // clear log
  FILE* log = fopen( "save/log.txt", "wb" );
  fclose(log);

  plasmacore_init();
}

- (void) configurePlasmacore
{
  // configure Plasmacore
  plasmacore_configure( 1024, 768, false, false );

  // Create window
  if (plasmacore.is_fullscreen) [self createFullscreenWindow];
  else                          [self createWindow];

  if (plasmacore.window_title) NativeLayer_set_window_title( plasmacore.window_title );
  else                         NativeLayer_set_window_title( "Plasmacore" );

  plasmacore_launch();
  draw_buffer.render();
  plasmacore_update();

  [plasmacore_window makeKeyAndOrderFront:self];
  [plasmacore_window setAcceptsMouseMovedEvents:YES];
}

//-----------------------------------------------------------------------------
//  createWindow
//-----------------------------------------------------------------------------
- (void) createWindow
{
  if (plasmacore_window) [plasmacore_window release];

  NSRect bounds = [[NSScreen mainScreen] frame];
  bounds.origin.x += (bounds.size.width  - plasmacore.display_width) / 2;
  bounds.origin.y += (bounds.size.height - plasmacore.display_height) / 2;
  bounds.size.width  = plasmacore.display_width;
  bounds.size.height = plasmacore.display_height;

  plasmacore_window = [[PlasmacoreWindow alloc] initWithContentRect: bounds 
           styleMask:NSTitledWindowMask+NSClosableWindowMask+NSMiniaturizableWindowMask
           backing:NSBackingStoreBuffered defer:YES];
  [plasmacore_window retain];

  [plasmacore_window setOpaque:YES];
  [plasmacore_window setBackgroundColor:[NSColor blackColor]];

  NSOpenGLPixelFormatAttribute attrs[] =
  {
      NSOpenGLPFADoubleBuffer,
      0
  };
  NSOpenGLPixelFormat* pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
   
  NSRect view_box = NSMakeRect( 0.0, 0.0, plasmacore.display_width, plasmacore.display_height );
  plasmacore_view = [self createPlasmacoreView:view_box withPixelFormat:pixel_format];
  [plasmacore_window setContentView: plasmacore_view];
  [plasmacore_view openGLContext]; // force OpenGL to initialize
  [plasmacore_window setAcceptsMouseMovedEvents:YES];
}

//-----------------------------------------------------------------------------
//  createFullscreenWindow
//-----------------------------------------------------------------------------
- (void) createFullscreenWindow
{
  if (plasmacore_window) [plasmacore_window release];
  NSRect bounds = [[NSScreen mainScreen] frame];

  plasmacore_window = [[PlasmacoreWindow alloc] initWithContentRect: bounds 
           styleMask:NSBorderlessWindowMask backing:NSBackingStoreBuffered defer:YES];
  [plasmacore_window retain];

  [plasmacore_window setLevel:NSMainMenuWindowLevel+1];

  [plasmacore_window setOpaque:YES];
  [plasmacore_window setHidesOnDeactivate:YES];
  [plasmacore_window setBackgroundColor:[NSColor blackColor]];

  NSOpenGLPixelFormatAttribute attrs[] =
  {
      NSOpenGLPFADoubleBuffer,
      0
  };
  NSOpenGLPixelFormat* pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
   
  NSRect view_box = NSMakeRect(0.0, 0.0, bounds.size.width, bounds.size.height);
  plasmacore_view = [self createPlasmacoreView:view_box withPixelFormat:pixel_format];
  [plasmacore_window setContentView: plasmacore_view];
  [plasmacore_view openGLContext]; // force OpenGL to initialize
}

//-----------------------------------------------------------------------------
//  update
//-----------------------------------------------------------------------------
- (void) update
{
  update_sounds();
  if (plasmacore_update()) [plasmacore_view setNeedsDisplay:YES];
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

@end

//=============================================================================
//  GLTexure
//=============================================================================
GLTexture::GLTexture( int w, int h, bool offscreen_buffer )
{
  frame_buffer = 0;
  if (offscreen_buffer)
  {
    glGenFramebuffersEXT( 1, &frame_buffer );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, frame_buffer );
  }
  glGenTextures( 1, &id );
  glBindTexture( GL_TEXTURE_2D, id );
  texture_width = texture_height = 0;
  resize( w, h );
}

void GLTexture::destroy()
{
  glDeleteTextures( 1, &id );
  if (frame_buffer) glDeleteFramebuffersEXT( 1, &frame_buffer );
}


//-----------------------------------------------------------------------------
//  LOG()
//-----------------------------------------------------------------------------
void LOG( const char* mesg ) 
{
  printf( "%s\n", mesg );

  FILE* logfile=fopen("save/log.txt","ab");
  fprintf( logfile, "%s\n", mesg ); 
  fclose(logfile); 
}


//=============================================================================
//  NativeLayer
//=============================================================================
void NativeLayer_alert( const char* mesg )
{
  LOG(mesg);
  [[NSAlert alertWithMessageText:@"Fatal Error" defaultButton:nil 
      alternateButton:nil otherButton:nil 
      informativeTextWithFormat:[NSString stringWithCString:mesg encoding:1]] runModal];
}

void NativeLayer_begin_draw()
{
  // Clear the screen to Display.background.color unless that color's alpha=0.
  BARD_FIND_TYPE( type_display, "Display" );
  BARD_GET_INT32( argb, type_display->singleton(), "background_color" );

  // Prepare for drawing.
  glDisable( GL_SCISSOR_TEST );

  int alpha = (argb >> 24) & 255;
  if (alpha)
  {
    glClearColor( ((argb>>16)&255)/255.0f,
        ((argb>>8)&255)/255.0f,
        ((argb)&255)/255.0f,
        alpha/255.0f );

    glClear(GL_COLOR_BUFFER_BIT);
  }

  glEnable( GL_BLEND );

  draw_buffer.set_draw_target( NULL );
}

void NativeLayer_end_draw()
{
  draw_buffer.render();
}

gdImagePtr NativeLayer_decode_image( char* raw_data, int data_size )
{
  // Local helper
  gdImagePtr img = gdImageCreateFromPngPtr( data_size, raw_data );

  if (img == NULL)
  {
	  // try loading JPEG
	  img = gdImageCreateFromJpegPtr( data_size, raw_data );
  }

  if (img && img->pixels)
  {
    // convert palletized to true color
    int width = img->sx;
    int height = img->sy;
    gdImagePtr true_color_img = gdImageCreateTrueColor( width, height );
    for (int j=0; j<height; ++j)
    {
      BardByte* src = ((BardByte*) (img->pixels[j])) - 1;
      BardInt32* dest = ((BardInt32*) (true_color_img->tpixels[j])) - 1;
      int count = width + 1;
      while (--count)
      {
        int index = *(++src);
        *(++dest) = (img->alpha[index] << 24) | (img->red[index] << 16)
          | (img->green[index] << 8) | img->blue[index];
      }
    }
    gdImageDestroy(img);
    img = true_color_img;
  }

  return img;
}

// local helper
void NativeLayer_init_bitmap( BardObject* bitmap_obj, char* raw_data, int data_size )
{
  gdImagePtr img = NativeLayer_decode_image( raw_data, data_size );

  if (img) 
  {
    int width = img->sx;
    int height = img->sy;

    BARD_PUSH_REF( bitmap_obj );
    BARD_PUSH_REF( bitmap_obj );
    BARD_PUSH_INT32( width );
    BARD_PUSH_INT32( height );
    BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
    BARD_GET_REF( array, BARD_PEEK_REF(), "data" );
    BARD_DISCARD_REF();
    
    // premultiply the alpha
    BardInt32* dest = ((BardInt32*) (((BardArray*)array)->data)) - 1;
    for (int j=0; j<height; ++j)
    {
      BardInt32* cur = ((BardInt32*) img->tpixels[j]) - 1;
      int count = width + 1;
      while (--count)
      {
        BardInt32 color = *(++cur);
        int a = gd_alpha_to_alpha_map[(color >> 24) & 127];
        int r = (color >> 16) & 255;
        int g = (color >> 8) & 255;
        int b = color & 255;

        r = (r * a) / 255;
        g = (g * a) / 255;
        b = (b * a) / 255;

        *(++dest) = (a<<24) | (r<<16) | (g<<8) | b;
      }
    }

    gdImageDestroy(img);
  }
}


void NativeLayer_set_window_title( const char* title )
{
  NSString* ns_title = [NSString stringWithCString:title encoding:1];
  [plasmacore_window setTitle:ns_title];

  // E.g. "plasmacore-vm" refused to be changed directly to "Plasmacore" (it 
  // thinks it's the same?!), so this is a kludge to get whatever window title
  // we want working.
  [[[[NSApp mainMenu] itemAtIndex:0] submenu] setTitle:[NSString stringWithCString:"." encoding:1]]; 

  // Set the real title.
  [[[[NSApp mainMenu] itemAtIndex:0] submenu] setTitle:ns_title];
}

void NativeLayer_shut_down()
{
  // No action
}


//-----------------------------------------------------------------------------
//  bard_adjust_filename_for_os()
//-----------------------------------------------------------------------------
void bard_adjust_filename_for_os( char* filename, int buffer_size )
{
  int i = strlen(filename) - 1;
  while (i > 0 && filename[i] != '.') --i;
  if (i > 0)
  {
    filename[i] = 0;
    NSString* base = [NSString stringWithCString:filename encoding:1];
    NSString* ext  = [NSString stringWithCString:filename+i+1 encoding:1];
    NSString* path = [[NSBundle mainBundle] pathForResource:base ofType:ext];
    if (path == nil) filename[i] = '.';  // restore original
    else strcpy( filename, [path cStringUsingEncoding:1] );
  }
}

//-----------------------------------------------------------------------------
//  Native Methd Implementations
//-----------------------------------------------------------------------------
Archive::Archive( const char* archive_filename )
{
  this->archive_filename = (char*) archive_filename;
}

Archive::~Archive()
{
}

void* Archive::open()
{
  return unzOpen( archive_filename );
}



void Application__log__String()
{
  BardString* mesg = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();  // discard singleton

  int count = mesg->count;
  if (count >= 512)
  {
    char* buffer = new char[count+1];
    mesg->to_ascii( buffer, count+1 );
    LOG( buffer );
    delete buffer;
  }
  else
  {
    char buffer[512];
    mesg->to_ascii( buffer, 512 );
    LOG( buffer );
  }
}



struct MacSound : BardResource
{
  SoundPlayer* audio_player;
  NSData*      sound_data;

  MacSound( char* data, int data_size )
  {
    audio_player = nil;
    sound_data = [NSData dataWithBytes:data length:data_size];
    //mm.resource_bytes += data_size;
    [sound_data retain];

    create_from_stored_sound_data();
  }

  MacSound( MacSound* other )
  {
    this->sound_data = other->sound_data;
    [sound_data retain];

    create_from_stored_sound_data();
  }

  void update()
  {
    if (audio_player) audio_player->update();
  }

  void create_from_stored_sound_data()
  {
    audio_player = new SoundPlayer( (char*) [sound_data bytes], sound_data.length );

    if ( !audio_player ) return;

    if ( audio_player->error() )
    {
      delete audio_player;
      audio_player = 0;
      return;
    }

    //mm.resource_bytes += sound_data.length;
  }

  ~MacSound()
  {
    destroy();
  }

  void destroy()
  {
    if (audio_player)
    {
      //mm.resource_bytes -= sound_data.length;

      delete audio_player;

      audio_player = NULL;
    }

    if (sound_data)
    {
      //mm.resource_bytes -= sound_data.length;
      [sound_data release];
      sound_data = nil;
    }
  }

  bool error() { return audio_player == NULL; }

  void play()
  {
    if (audio_player) audio_player->play();
  }

  void pause()
  {
    if (audio_player) audio_player->pause();
  }

  bool is_playing()
  {
    if (audio_player) 
    {
      return audio_player->is_playing();
    }
    return false;
  }

  void set_volume( BardReal64 new_volume )
  {
    if (audio_player) audio_player->set_volume( new_volume );
}

  void set_repeats( BardInt32 setting )
  {
    if (audio_player) audio_player->repeats = setting;
  }

  double get_current_time()
  {
    if (audio_player) return audio_player->get_current_time();
    return 0.0;
  }

  void set_current_time( double new_time )
  {
    if (audio_player) audio_player->set_current_time( new_time );
  }

  double get_duration()
  {
    if (audio_player) return audio_player->duration;
    return 0.0;
  }
};


//=============================================================================

void Application__title__String()
{
  // Application::title(String) 
  BardString* mesg = (BardString*) BARD_POP_REF();  // title string 
  BARD_DISCARD_REF();  // discard singleton

  if ( !mesg ) return;

  char buffer[100];
  ((BardString*)mesg)->to_ascii( buffer, 100 );
  NativeLayer_set_window_title( buffer );
}

void Bitmap__to_png_bytes()
{
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  int w = bitmap_obj->width;
  int h = bitmap_obj->height;

  gdImagePtr img = gdImageCreateTrueColor( w, h );
  gdImageSaveAlpha( img, 1 );
  gdImageAlphaBlending( img, 0 );
  for (int j=0; j<h; ++j)
  {
    int* dest = img->tpixels[j];
    int* src = ((int*) bitmap_obj->pixels->data) + j*w;
    --dest;
    --src;
    for (int i=0; i<w; ++i)
    {
      int c = *(++src);
      int a = alpha_to_gd_alpha_map[(c >> 24) & 255] << 24;
      *(++dest) = a | (c & 0xffffff);
    }
  }

  int size;
  char* bytes = (char*) gdImagePngPtr( img, &size );
  gdImageDestroy(img);

  BARD_PUSH_REF( bard_create_byte_list(bytes,size) );

  gdFree( bytes );
}

void Bitmap__to_jpg_bytes__Real64()
{
  double compression = BARD_POP_REAL64();
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  int w = bitmap_obj->width;
  int h = bitmap_obj->height;

  gdImagePtr img = gdImageCreateTrueColor( w, h );
  for (int j=0; j<h; ++j)
  {
    int* dest = img->tpixels[j];
    int* src = ((int*) bitmap_obj->pixels->data) + j*w;
    --dest;
    --src;
    for (int i=0; i<w; ++i)
    {
      int c = *(++src);
      int a = alpha_to_gd_alpha_map[(c >> 24) & 255] << 24;
      *(++dest) = a | (c & 0xffffff);
    }
  }

  int size;
  char* bytes = (char*) gdImageJpegPtr( img, &size, int(compression*100) );
  gdImageDestroy(img);

  BARD_PUSH_REF( bard_create_byte_list(bytes,size) );

  gdFree( bytes );
}

void Display__fullscreen()
{
  // Application::fullscreen().Logical 
  BARD_DISCARD_REF();
  BARD_PUSH_LOGICAL( 1 );
}

void Display__fullscreen__Logical()
{
  // Application::fullscreen(Logical) 
  BARD_POP_INT32();  // ignore fullscreen setting 
  BARD_DISCARD_REF();
}

void Input__mouse_position__Vector2()
{
  Vector2 pos = BARD_POP( Vector2 );
  BARD_DISCARD_REF();
  
  pos.transform();
  NSRect client_rect = [plasmacore_window contentRectForFrameRect:[plasmacore_window frame]];
  pos.x += client_rect.origin.x;
  pos.y += client_rect.origin.y;
  CGWarpMouseCursorPosition( CGPointMake((float)pos.x,(float)pos.y) );
}

void Input__input_capture__Logical()
{
  BARD_POP_INT32(); // ignore setting
  BARD_DISCARD_REF();      // discard singleton
}

void Input__mouse_visible__Logical()
{
  bool setting = BARD_POP_INT32();
  BARD_DISCARD_REF();      // discard singleton

  if (setting == mouse_visible) return;
  mouse_visible = setting;

  if (setting) [NSCursor unhide];
  else         [NSCursor hide];
}

//-----------------------------------------------------------------------------
//  NativeSound
//-----------------------------------------------------------------------------

void NativeSound__init__String()
{
  // NativeSound::init(String)
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BardObject* sound_obj = BARD_POP_REF();

  char filename[PATH_MAX];
  filename_obj->to_ascii( filename, PATH_MAX );

  int data_size;
  char* data = sound_archive.load( filename_obj, &data_size );

  if (data) 
  {
    MacSound* sound = new MacSound( data, data_size );

    //BardLocalRef gc_guard(sound_obj);
    BardObject* data_obj = BardNativeData::create( sound, BardNativeDataDeleteResource );

    delete data;

    if ( !sound->error() )
    {
      BARD_SET_REF( sound_obj, "native_data", data_obj );
    }
  }
  else
  {
    bard_throw_file_error( filename );
  }
}

void NativeSound__init__ArrayList_of_Byte()
{
  // NativeSound::init(Byte[])
  BardArrayList* list = (BardArrayList*) BARD_POP_REF();
  BardObject* sound_obj = BARD_POP_REF();

  MacSound* sound = new MacSound( (char*)(list->array->data), list->count );

  //BardLocalRef gc_guard(sound_obj);
  BardObject* data_obj = BardNativeData::create( sound, BardNativeDataDeleteResource );

  if ( !sound->error() )
  {
    BARD_SET_REF( sound_obj, "native_data", data_obj );
  }
}

MacSound* get_sound_from_stack()
{
  BardObject* sound_obj = BARD_POP_REF(); // sound object 
  BARD_GET_REF( native_data, sound_obj, "native_data" );
  if ( !native_data ) return NULL;

  return (MacSound*) (((BardNativeData*)native_data)->data);
}

void NativeSound__create_duplicate()
{
  // NativeSound::create_duplicate().Sound 

  MacSound* sound = get_sound_from_stack();
  if (sound)
  {
    BARD_FIND_TYPE( type_sound, "NativeSound" );
    BardObject* sound_obj = type_sound->create();
    BARD_PUSH_REF( sound_obj );

    sound = new MacSound( sound );
    BardObject* data_obj = BardNativeData::create( sound, BardNativeDataDeleteResource );
    BARD_SET_REF( sound_obj, "native_data", data_obj );
  }
}

void NativeSound__play()
{
  // NativeSound::play() 

  MacSound* sound = get_sound_from_stack();
  if (sound) sound->play();
}

void NativeSound__pause()
{
  // NativeSound::pause() 

  MacSound* sound = get_sound_from_stack();
  if (sound) sound->pause();
}

void NativeSound__is_playing()
{
  // NativeSound::is_playing().Logical 

  MacSound* sound = get_sound_from_stack();
  if (sound) 
  {
    BARD_PUSH_INT32( sound->is_playing() );
  }
}

void NativeSound__volume__Real64()
{
  // NativeSound::volume(Real64) 

  double volume = BARD_POP_REAL64();
  MacSound* sound = get_sound_from_stack();
  if (sound) sound->set_volume(volume);
}

void NativeSound__pan__Real64()
{
  // NativeSound::pan(Real64) 

  BARD_POP_REAL64();
  BARD_DISCARD_REF(); // sound object 

  // no action
}

void NativeSound__pitch__Real64()
{
  // NativeSound::pitch(Real64) 

  BARD_POP_REAL64();
  BARD_DISCARD_REF(); // sound object 

  // no action
}

void NativeSound__repeats__Logical()
{
  // NativeSound::repeats(Logical) 

  int setting = BARD_POP_INT32();
  MacSound* sound = get_sound_from_stack();
  if (sound) sound->set_repeats(setting);
}

void NativeSound__current_time()
{
  // NativeSound::current_time().Real64 

  MacSound* sound = get_sound_from_stack();
  if (sound) 
  {
    BARD_PUSH_REAL64( (BardReal64)(sound->get_current_time()) );
  }
}

void NativeSound__current_time__Real64()
{
  // NativeSound::current_time(Real64) 

  BardReal64 new_time = BARD_POP_REAL64();
  MacSound* sound = get_sound_from_stack();
  if (sound) 
  {
    sound->set_current_time( new_time );
  }
}

void NativeSound__duration()
{
  // NativeSound::duration().Real64 

  MacSound* sound = get_sound_from_stack();
  if (sound) 
  {
    BARD_PUSH_REAL64( (BardReal64) sound->get_duration() );
  }
}


void OffscreenBuffer__clear__Color()
{
  // OffscreenBuffer::clear(Color)
  BardInt32 color = BARD_POP_INT32();
  BardObject* buffer_obj = BARD_POP_REF();

  BVM_NULL_CHECK( buffer_obj, return );

  BARD_GET_REF( texture_obj, buffer_obj, "texture" );
  BVM_NULL_CHECK( texture_obj, return );

  GLTexture* texture = NativeLayer_get_native_texture_data( texture_obj );
  if ( !texture || !texture->frame_buffer ) return;

  glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, texture->frame_buffer );
  glDisable( GL_SCISSOR_TEST );
  glClearColor( ((color>>16)&255)/255.0f,
      ((color>>8)&255)/255.0f,
      ((color)&255)/255.0f,
      ((color>>24)&255)/255.0f );
  glClear(GL_COLOR_BUFFER_BIT);
  if (use_scissor) glEnable( GL_SCISSOR_TEST );

  if (draw_buffer.draw_target)
  {
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, draw_buffer.draw_target->frame_buffer );
  }
  else
  {
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
  }
}

void System__country_name()
{
  BARD_DISCARD_REF();  // singleton

  NSLocale *locale = [NSLocale currentLocale];
  NSString *country_code = [locale objectForKey:NSLocaleCountryCode];
  NSString *country_name = [locale displayNameForKey:NSLocaleCountryCode value: country_code];

  BardString* result = (BardString*) BardString::create( [country_name length] );
  [country_name getCharacters:(unichar*)result->characters];
  BARD_PUSH_REF( result );  // leave result on stack
  result->set_hash_code();
}


void on_audio_interruption( void* user_data, UInt32 interruption_state )
{
  //if (interruption_state == kAudioSessionEndInterruption) NSLog(@"end audio interruption");
  //else                                                    NSLog(@"start audio interruption");
}

void Texture_init( BardInt32* data, int w, int h, int format );

void Texture__init__String_Int32()
{
  int format = BARD_POP_INT32();
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  // Leave Texture obj on stack.

  int data_size;
  char* data = image_archive.load( filename_obj, &data_size );

  if ( !data ) 
  {
    BARD_DISCARD_REF();  // Texture obj
    char buffer[256];
    filename_obj->to_ascii( buffer, 256 );
    bard_throw_file_not_found_error( buffer );
    return;
  }

  gdImagePtr img = NativeLayer_decode_image( data, data_size );
  if (img)
  {
    int width = img->sx;
    int height = img->sy;

    BardInt32* pixels = new BardInt32[ width*height ];

    // premultiply the alpha
    BardInt32* dest = pixels - 1;
    for (int j=0; j<height; ++j)
    {
      BardInt32* cur = ((BardInt32*) img->tpixels[j]) - 1;
      int count = width + 1;
      while (--count)
      {
        BardInt32 color = *(++cur);
        int a = gd_alpha_to_alpha_map[(color >> 24) & 255];
        int r = (color >> 16) & 255;
        int g = (color >> 8) & 255;
        int b = color & 255;

        r = (r * a) / 255;
        g = (g * a) / 255;
        b = (b * a) / 255;

        *(++dest) = (a<<24) | (r<<16) | (g<<8) | b;
      }
    }

    gdImageDestroy( img );
    Texture_init( pixels, width, height, format );
    delete pixels;
  }
  else
  {
    BARD_DISCARD_REF();
  }

  delete data;
}


void Texture__draw_tile__Corners_Vector2_Vector2_Int32()
{
  BardInt32 render_flags = BARD_POP_INT32();
  Vector2 size = BARD_POP(Vector2);
  Vector2 pos  = BARD_POP(Vector2);
  Vector2 uv_a = BARD_POP(Vector2);
  Vector2 uv_b = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data( texture_obj );
  if ( !texture ) return;

  draw_buffer.set_render_flags( render_flags, BLEND_ONE, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_textured_triangle_mode( texture, NULL );

  bool hflip;
  if (size.x < 0)
  {
    size.x = -size.x;
    hflip = true;
  }
  else
  {
    hflip = false;
  }

  bool vflip;
  if (size.y < 0)
  {
    size.y = -size.y;
    vflip = true;
  }
  else
  {
    vflip = false;
  }

  GLVertex2 v1( pos.x, pos.y );
  GLVertex2 v2( pos.x+size.x, pos.y );
  GLVertex2 v3( pos.x+size.x, pos.y+size.y );
  GLVertex2 v4( pos.x, pos.y+size.y );

  if (hflip)
  {
    GLVertex2 temp = v1;
    v1 = v2;
    v2 = temp;
    temp = v3;
    v3 = v4;
    v4 = temp;
  }

  if (vflip)
  {
    GLVertex2 temp = v1;
    v1 = v4;
    v4 = temp;
    temp = v2;
    v2 = v3;
    v3 = temp;
  }

  GLVertex2 uv1( uv_a.x, uv_a.y );
  GLVertex2 uv2( uv_b.x, uv_a.y );
  GLVertex2 uv3( uv_b.x, uv_b.y );
  GLVertex2 uv4( uv_a.x, uv_b.y );


  draw_buffer.add( v1, v2, v4, 0xffffffff, 0xffffffff, 0xffffffff, uv1, uv2, uv4 );
  draw_buffer.add( v4, v2, v3, 0xffffffff, 0xffffffff, 0xffffffff, uv4, uv2, uv3 );
}

