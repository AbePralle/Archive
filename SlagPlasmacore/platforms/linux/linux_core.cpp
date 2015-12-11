//=============================================================================
// linux_core.cpp
//
// $(PLASMACORE_VERSION) $(DATE)
//
// http://plasmaworks.com/plasmacore
//
// Contains Unix-specific Plasmacore implementation.
//
// ----------------------------------------------------------------------------
//
// $(COPYRIGHT)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <string>
#include <sys/stat.h>

#include <SDL/SDL.h>

#include "plasmacore.h"
#include "unzip.h"

#include "gd.h"
#include "png.h"

// Patch for deprecated call in gd 2.0.35
extern "C" int png_check_sig( png_byte* sig, int num )
{
  return !png_sig_cmp(sig,0,num);
}

//kludge
void sound_manager_init();
void sound_manager_shut_down();

GLTexture* NativeLayer_get_native_texture_data( BardObject* texture_obj );

static SDL_Surface *sdl_surface;

bool plasmacore_running;

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

void LinuxCore_update();

Archive data_archive( "data.zip" );
Archive image_archive( "images.zip" );

void LOG( const char* mesg ) 
{
  printf( "%s\n", mesg );

  FILE* logfile=fopen("save/log.txt","ab");
  fprintf( logfile, "%s\n", mesg ); 
  fclose(logfile); 
}


void LinuxCore_init(int argc, char * argv[])
{
  // init filesystem
  std::string path = argv[0];
  for (int i = path.size() - 2; i >= 0; i--)
  {
    if (path[i] == '/')
    {
      path[i] = 0;
      break;
    }
  }
  if (strlen(path.c_str())) chdir(path.c_str());
  mkdir("save", 0700);

  // clear log
  FILE* log = fopen( "save/log.txt", "wb" );
  fclose(log);

  // init SDL
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
  {
    fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute ( SDL_GL_DOUBLEBUFFER, 1 );

  plasmacore_init();
}

void LinuxCore_configure()
{
  // configure Plasmacore
  plasmacore_configure( 1024, 768, false, false );

  // Init Display

  // nVidia drivers pay attention to this? Other attempts are below.
  putenv("__GL_SYNC_TO_VBLANK=1");

  // Create window
  sdl_surface = SDL_SetVideoMode ( plasmacore.display_width, plasmacore.display_height, 0, SDL_OPENGL );
  if (sdl_surface == NULL) 
  {
    throw "SDL_SetVideoMode failed";
  }

  SDL_WM_SetCaption( "Plasmacore", "Plasmacore" );

  // Sets up matrices and transforms for OpenGL
  glViewport(0, 0, plasmacore.display_width, plasmacore.display_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho( 0, plasmacore.display_width, plasmacore.display_height, 0, -1, 1 );
  glMatrixMode(GL_MODELVIEW);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

  // Sets up pointers and enables states needed for using vertex arrays and textures
  glClientActiveTexture(GL_TEXTURE0);
  glVertexPointer( 2, GL_FLOAT, 0, draw_buffer.vertices );
  glTexCoordPointer( 2, GL_FLOAT, 0, draw_buffer.uv);
  glColorPointer( 4, GL_UNSIGNED_BYTE, 0, draw_buffer.colors);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glClientActiveTexture(GL_TEXTURE1);
  glTexCoordPointer( 2, GL_FLOAT, 0, draw_buffer.alpha_uv);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL_TEXTURE0);

  // enable vertical blank synchronization
#if SDL_VERSION_ATLEAST(1,3,0)
  SDL_GL_SetSwapInterval(1);
#elif defined(SDL_GL_SWAP_CONTROL)
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
#else 
  // Some other method?
#endif 

  sound_manager_init();

  plasmacore_launch();
  draw_buffer.render();
}

void NativeLayer_alert( const char* mesg )
{
  LOG(mesg);
  /*
     [[NSAlert alertWithMessageText:@"Fatal Error" defaultButton:nil 
      alternateButton:nil otherButton:nil 
      informativeTextWithFormat:[NSString stringWithCString:mesg encoding:1]] runModal];
  */
  //TODO: popup box
}


void LinuxCore_main_loop()
{
  int ms_error = 0;
  BardInt64 next_time = bard_get_time_ms() + (1000/plasmacore.target_fps);
  plasmacore_running = true;
  int kill_ms = 0;
  while ( plasmacore_running ) 
  {
    // Check for events 
    SDL_Event event;
    while ( SDL_PollEvent (&event) ) 
    {
      switch (event.type)
      {
        case SDL_QUIT:
          plasmacore_on_exit_request();
          break;

        case SDL_KEYDOWN:
	  {
            int unicode = 0;
            int code = event.key.keysym.sym;
            plasmacore_queue_data_event( plasmacore.event_key, unicode, code, true );
          }
          break;

        case SDL_KEYUP:
          {
            int unicode = 0;
            int code = event.key.keysym.sym;
            plasmacore_queue_data_event( plasmacore.event_key, unicode, code, false );
          }
          break;

        case SDL_MOUSEMOTION:
          {
            int mouse_id = 1;

            // Relative
            int x = int( event.motion.xrel / plasmacore.scale_factor );
            int y = int( event.motion.yrel / plasmacore.scale_factor );
            plasmacore_queue_data_event( plasmacore.event_mouse_move, 
              mouse_id, 0, false, x, y  );

            // Absolute
            x = int((event.button.x-plasmacore.border_x) / plasmacore.scale_factor);
            y = int((event.button.y-plasmacore.border_y) / plasmacore.scale_factor);
            plasmacore_queue_data_event( plasmacore.event_mouse_move, 
              mouse_id, 0, true, x, y  );

          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          switch (event.button.button)
          {
            case SDL_BUTTON_WHEELUP:
              {
                int mouse_id = 1;
                int scroll_dir = -1;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_wheel, 
                    mouse_id, scroll_dir, false, x, y );
              }
              break;

            case SDL_BUTTON_WHEELDOWN:
              {
                int mouse_id = 1;
                int scroll_dir = 1;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_wheel, 
                    mouse_id, scroll_dir, false, x, y );
              }
              break;

            case SDL_BUTTON_LEFT:
              {
                int index = 1;
                int mouse_id = 1;
                bool press = true;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button,
                  mouse_id, index, press, x, y );
              }
              break;

            case SDL_BUTTON_RIGHT:
              {
                int index = 2;
                int mouse_id = 1;
                bool press = true;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button,
                  mouse_id, index, press, x, y );
              }
              break;

            case SDL_BUTTON_MIDDLE:
              {
                int index = 3;
                int mouse_id = 1;
                bool press = true;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button,
                  mouse_id, index, press, x, y );
              }
              break;
          }
          break;

        case SDL_MOUSEBUTTONUP:
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              {
                int index = 1;
                int mouse_id = 1;
                bool press = false;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button,
                  mouse_id, index, press, x, y );
              }
              break;

            case SDL_BUTTON_RIGHT:
              {
                int index = 2;
                int mouse_id = 1;
                bool press = false;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button,
                  mouse_id, index, press, x, y );
              }
              break;

            case SDL_BUTTON_MIDDLE:
              {
                int index = 3;
                int mouse_id = 1;
                bool press = false;
                double x = (event.button.x-plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y-plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button,
                  mouse_id, index, press, x, y );
              }
              break;
          }
          break;

        case SDL_VIDEORESIZE:
          //plasmacore.window_width = event.resize.w;
          //plasmacore.window_height = event.resize.h;
          //reset_display();
          break;

          // maybe use this code later
             //case SDL_USEREVENT:
             //if (e.user.code == 0)
             //{
          // Switch to fullscreen if possible.
          //if (plasmacore.fullscreen_allowed) 
          //{
          //toggle_fullscreen();
          //}          
          //}
          //break;

      }
    }

    LinuxCore_update();
    SDL_GL_SwapBuffers();

    BardInt64 cur_time = bard_get_time_ms();

    kill_ms = (int) (next_time - cur_time);
    if (kill_ms <= 0) 
    {
      next_time = cur_time;
      kill_ms = 1;
    }
    next_time += 1000 / plasmacore.target_fps;
    ms_error += 1000 % plasmacore.target_fps;
    if (ms_error >= plasmacore.target_fps)
    {
      ms_error -= plasmacore.target_fps;
      ++next_time;
    }

    //plasmacore_clear_transforms();
    draw_buffer.set_draw_target( NULL );
    glDisable( GL_SCISSOR_TEST );
    draw_buffer.render();

    NativeLayer_sleep( kill_ms );
  }

  NativeLayer_shut_down();
}

void LinuxCore_update()
{
  //update_sounds();

  if ( plasmacore_update() )
  {
    plasmacore_draw();
  }
}

void NativeLayer_begin_draw()
{
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
  glDisable( GL_SCISSOR_TEST );
  glEnable( GL_BLEND );

  draw_buffer.set_draw_target( NULL );
}

void NativeLayer_end_draw()
{
  draw_buffer.render();
}

void NativeLayer_shut_down()
{
  sound_manager_shut_down();
}

// local helper
void NativeLayer_init_bitmap( BardObject* bitmap_obj, char* raw_data, int data_size )
{
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

  if (img) 
  {
    int width = img->sx;
    int height = img->sy;

    BARD_PUSH_REF( bitmap_obj );
    BARD_PUSH_REF( bitmap_obj );
    BARD_PUSH_INT32( width );
    BARD_PUSH_INT32( height );
    BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
    BARD_GET( BardArray*, array, BARD_PEEK_REF(), "data" );
    BARD_POP_REF();
    
    // premultiply the alpha
    BardInt32* dest = ((BardInt32*) array->data) - 1;
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

    gdImageDestroy(img);
  }
}

void bard_adjust_filename_for_os( char* filename, int buffer_size )
{
}

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
  BARD_POP_REF();  // discard singleton

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

//=============================================================================

void Application__title__String()
{
  // Application::title(String) 
  BardString* mesg = (BardString*) BARD_POP_REF();  // title string 
  BARD_POP_REF();  // discard singleton

  if ( !mesg ) return;

  char buffer[100];
  ((BardString*)mesg)->to_ascii( buffer, 100 );
  SDL_WM_SetCaption( buffer, buffer );
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
  BARD_POP_REF();
  BARD_PUSH_INT32( 1 );
}

void Display__fullscreen__Logical()
{
  // Application::fullscreen(Logical) 
  BARD_POP_INT32();  // ignore fullscreen setting 
  BARD_POP_REF();
}

void Input__input_capture__Logical()
{
  BARD_POP_INT32(); // ignore setting
  BARD_POP_REF();      // discard singleton
}

void Input__mouse_visible__Logical()
{
  bool setting = BARD_POP_INT32();
  BARD_POP_REF();      // discard singleton

  if (setting == mouse_visible) return;
  mouse_visible = setting;

  if (setting) SDL_ShowCursor( SDL_ENABLE );
  else         SDL_ShowCursor( SDL_DISABLE );
}

void OffscreenBuffer__clear__Color()
{
  // OffscreenBuffer::clear(Color)
  draw_buffer.render();

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
  BARD_POP_REF();  // singleton

  char * result = getenv("LANG");
  if (!result || strlen(result) == 0) result = "Undefined";

  BARD_PUSH_REF( BardString::create(result) );
}

void on_audio_interruption( void* user_data, uint32_t interruption_state )
{
  //if (interruption_state == kAudioSessionEndInterruption) NSLog(@"end audio interruption");
  //else                                                    NSLog(@"start audio interruption");
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

  GLVertex v1( pos.x, pos.y );
  GLVertex v2( pos.x+size.x, pos.y );
  GLVertex v3( pos.x+size.x, pos.y+size.y );
  GLVertex v4( pos.x, pos.y+size.y );

  if (hflip)
  {
    GLVertex temp = v1;
    v1 = v2;
    v2 = temp;
    temp = v3;
    v3 = v4;
    v4 = temp;
  }

  if (vflip)
  {
    GLVertex temp = v1;
    v1 = v4;
    v4 = temp;
    temp = v2;
    v2 = v3;
    v3 = temp;
  }

  GLVertex uv1( uv_a.x, uv_a.y );
  GLVertex uv2( uv_b.x, uv_a.y );
  GLVertex uv3( uv_b.x, uv_b.y );
  GLVertex uv4( uv_a.x, uv_b.y );


  draw_buffer.add( v1, v2, v4, 0xffffffff, 0xffffffff, 0xffffffff, uv1, uv2, uv4 );
  draw_buffer.add( v4, v2, v3, 0xffffffff, 0xffffffff, 0xffffffff, uv4, uv2, uv3 );
}

