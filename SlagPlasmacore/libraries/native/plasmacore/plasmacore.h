#ifndef PLASMACORE_H
#define PLASMACORE_H
//=============================================================================
// plasmacore.h
//
// $(PLASMACORE_VERSION) $(DATE)
//
// http://plasmaworks.com/plasmacore
// 
// Code common to all versions of Plasmacore.
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

#include "bard.h"

void LOG( const char* mesg );
#define TRACE(mesg) plasmacore_queue_signal( "trace", mesg )

extern int embedded_font_system_17_size;
extern unsigned char embedded_font_system_17[];

#ifdef ANDROID
#define DATA_ARCHIVE   1
#define IMAGES_ARCHIVE 2
#define SOUNDS_ARCHIVE 3
struct Archive
{
  int archive_type;

  Archive( int type ); 

  char* load( BardString* filename, int* size );
  char* load( const char* filename, int* size );
};
#else
struct Archive
{
  char* archive_filename;

  Archive( const char* archive_filename );
  ~Archive();

  void* open();
  char* load( BardString* filename, int* size );
  char* load( const char* filename, int* size );
};
#endif

#define RENDER_FLAG_POINT_FILTER (1 << 0)
#define RENDER_FLAG_FIXED_COLOR  (1 << 2)
#define RENDER_FLAG_TEXTURE_WRAP (1 << 3)

//--------
#define BLEND_ZERO                0
#define BLEND_ONE                 1
#define BLEND_SRC_ALPHA           2
#define BLEND_INVERSE_SRC_ALPHA   3
#define BLEND_DEST_ALPHA          4
#define BLEND_INVERSE_DEST_ALPHA  5
#define BLEND_DEST_COLOR          6
#define BLEND_INVERSE_DEST_COLOR  7
#define BLEND_OPAQUE              8
#define BLEND_SRC_COLOR           9
#define BLEND_INVERSE_SRC_COLOR  10
//--------

#define PIXEL_FORMAT_RGB32     1
#define PIXEL_FORMAT_RGB16     2
#define PIXEL_FORMAT_INDEXED   4

//--------

// new
#define BARD_TRIANGLES 1
#define BARD_LINES     2
#define BARD_POINTS    3

// old
#define MAX_BUFFERED_VERTICES 512*3
#define DRAW_TEXTURED_TRIANGLES 1
#define DRAW_SOLID_TRIANGLES    2
#define DRAW_LINES              3
#define DRAW_POINTS             4


//--------

#define SEPARATE_ARGB(c,a,r,g,b) a=(c>>24)&255; r=(c>>16)&255; g=(c>>8)&255; b=c&255
#define COMBINE_ARGB(a,r,g,b) ((a<<24)|(r<<16)|(g<<8)|b)


struct BardBitmap : BardObject
{
  BardArray*    pixels;
  BardInt32     width;
  BardInt32     height;
};

struct Vector2
{
  BardReal64 x, y;

  Vector2() { }
  Vector2( BardReal64 x, BardReal64 y ) : x(x), y(y) {}

  Vector2& operator+=( Vector2 other )
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vector2& operator-=( Vector2 other )
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  void transform();
};

struct Vector3
{
  BardReal64 x, y, z;

  Vector3() { }
  Vector3( BardReal64 x, BardReal64 y, BardReal64 z ) : x(x), y(y), z(z) {}
};

struct Matrix4
{
  BardReal64 r1c1, r1c2, r1c3, r1c4;
  BardReal64 r2c1, r2c2, r2c3, r2c4;
  BardReal64 r3c1, r3c2, r3c3, r3c4;
  BardReal64 r4c1, r4c2, r4c3, r4c4;

  Matrix4()
  {
    // Set to Identity.
    memset( this, 0, sizeof(Matrix4) );
    r1c1 = r2c2 = r3c3 = r4c4 = 1;
  }

  Matrix4(
      BardReal64 r1c1, BardReal64 r1c2, BardReal64 r1c3, BardReal64 r1c4,
      BardReal64 r2c1, BardReal64 r2c2, BardReal64 r2c3, BardReal64 r2c4,
      BardReal64 r3c1, BardReal64 r3c2, BardReal64 r3c3, BardReal64 r3c4,
      BardReal64 r4c1, BardReal64 r4c2, BardReal64 r4c3, BardReal64 r4c4
      )
  {
    // Arbitrary 4x4 matrix.
    this->r1c1 = r1c1;
    this->r1c2 = r1c2;
    this->r1c3 = r1c3;
    this->r1c4 = r1c4;
    this->r2c1 = r2c1;
    this->r2c2 = r2c2;
    this->r2c3 = r2c3;
    this->r2c4 = r2c4;
    this->r3c1 = r3c1;
    this->r3c2 = r3c2;
    this->r3c3 = r3c3;
    this->r3c4 = r3c4;
    this->r4c1 = r4c1;
    this->r4c2 = r4c2;
    this->r4c3 = r4c3;
    this->r4c4 = r4c4;
  }

  void      print();
  Matrix4   operator *( Matrix4 m2 );
  Matrix4   transposed();
  double    determinant();
  Matrix4   inverse();
  Vector2   transform( Vector2 v );
  Vector3   transform( Vector3 v );
  void      to_transposed_doubles( double* data );
  void      to_transposed_floats( float* data );
  void      to_floats( float* data );
  void      to_fixed( BardInt32* data );
};

struct PCoreBox
{
  Vector2 top_left, size;

  PCoreBox() {}
  PCoreBox( Vector2 tl, Vector2 br ) : top_left(tl), size(br) {}
  PCoreBox( double x, double y, double w, double h ) : top_left(x,y), size(w,h) {}
};

extern bool mouse_visible;
extern bool log_drawing;
extern bool use_scissor;


struct Plasmacore
{
  int display_width;
  int display_height;

  bool    mode_3d;
  Matrix4 projection_transform;
  Matrix4 view_transform;

  int     border_x;
  int     border_y;

  int fast_hardware;
  int orientation;  // 0..3 = up right down left
  int original_orientation;  // what's read in from plasmacore

  bool is_fullscreen;
  bool borderless_window;

  char* window_title;
  ArrayList<char*> command_line_args;

  double updates_per_second;  // default: 60.0
  int    target_fps;          // default: 60   - should always be <= updates_per_second
  double time_debt;           // in seconds

  BardInt64  last_update_time_ms;  // timestamp
  BardInt32  last_draw_time_ms;    // interval

  // Permanent references to string objects for efficiency.
  BardObject* event_launch;
  BardObject* event_update;
  BardObject* event_draw;
  BardObject* event_key;
  BardObject* event_mouse_move;
  BardObject* event_mouse_button;
  BardObject* event_mouse_wheel;
  BardObject* event_textures_lost;
  BardObject* event_suspend;
  BardObject* event_resume;
  BardObject* event_shut_down;

  Plasmacore() : mode_3d(false), window_title(NULL), event_suspend(NULL)
  {
    updates_per_second = 60;
    target_fps = 60;
    orientation = 0;
    set_defaults(); 
  }

  ~Plasmacore()
  {
    if (window_title)
    {
      delete window_title;
      window_title = NULL;
    }
  }

  void set_defaults()
  {
    border_x = 0;
    border_y = 0;
    is_fullscreen  = false;
    borderless_window = false;
    fast_hardware = true;

    time_debt = 0.0;
    last_update_time_ms = 0;
    last_draw_time_ms   = 0;

    event_launch = NULL;
    event_update = NULL;
    event_draw   = NULL;

    /*
    camera_transform_stack_count = 0;
    camera_transform_stack_modified = true;
    object_transform_stack_count = 0;
    object_transform_stack_modified = true;
    */
  }

  /*
  Vector2 point_to_screen( Vector2 v )
  {
    return Vector2( v.x*scale_factor+border_x, v.y*scale_factor+border_y );
  }

  Vector2 size_to_screen( Vector2 v )
  {
    return Vector2( v.x*scale_factor, v.y*scale_factor );
  }
  */
};

extern Plasmacore plasmacore;

//=============================================================================
//  UpdateCycleRegulator
//=============================================================================
struct UpdateCycleRegulator
{
  BardInt64 next_frame_time;
  int       ms_error;

  UpdateCycleRegulator() : next_frame_time(0), ms_error(0)
  {
    reset();
  }

  void reset()
  {
    next_frame_time = bard_get_time_ms() + (1000/plasmacore.target_fps);
  }

  int update()
  {
    BardInt64 cur_time = bard_get_time_ms();
    int kill_ms = int(next_frame_time - cur_time);

    if (kill_ms <= 0)
    {
      next_frame_time = cur_time;
      kill_ms = 1;
    }
    next_frame_time += 1000 / plasmacore.target_fps;
    ms_error += 1000 % plasmacore.target_fps;
    if (ms_error >= plasmacore.target_fps)
    {
      ms_error -= plasmacore.target_fps;
      ++next_frame_time;
    }
    return kill_ms;
  }
};

//=============================================================================
//  TouchInfo
//=============================================================================
#define PLASMACORE_MAX_TOUCHES 10

struct TouchInfo
{
  int     active;
  Vector2 position;

  TouchInfo() { active = 0; }
};

struct TouchManager
{
  TouchInfo touches[PLASMACORE_MAX_TOUCHES];
  int   num_active_touches;

  TouchManager() { num_active_touches = 0; }

  // Each method returns the index (1+) that TouchManager
  // is using to track the event.
  int begin_touch( double x, double y );
  int update_touch( double x, double y );
  int end_touch( double x, double y );
};


#if !defined(_WIN32)
# define GL_PLASMACORE
# if defined(ANDROID)
#   include <GLES2/gl2.h>
#   include <GLES2/gl2ext.h>
#   define MAIN_BUFFER 0
# elif defined(MAC)
#   import <QuartzCore/QuartzCore.h>
#   define glBindFramebufferOES glBindFramebufferEXT
#   define GL_FRAMEBUFFER_OES   GL_FRAMEBUFFER_EXT
#   define GL_FRAMEBUFFER_COMPLETE_OES   GL_FRAMEBUFFER_COMPLETE_EXT
#   define GL_COLOR_ATTACHMENT0_OES   GL_COLOR_ATTACHMENT0_EXT
#   define glFramebufferTexture2DOES   glFramebufferTexture2DEXT
#   define glCheckFramebufferStatusOES   glCheckFramebufferStatusEXT
#   define glDeleteFramebuffersOES glDeleteFramebuffersEXT
#   define MAIN_BUFFER 0
# elif TARGET_OS_IPHONE
#   include <OpenGLES/ES2/gl.h>
#   include <OpenGLES/ES2/glext.h>
#   define MAIN_BUFFER NativeLayer_get_frame_buffer()
    GLuint NativeLayer_get_frame_buffer();
# elif defined(UNIX)
#   define GL_GLEXT_PROTOTYPES 1
#   define glBindFramebufferOES glBindFramebufferEXT
#   define GL_FRAMEBUFFER_OES   GL_FRAMEBUFFER_EXT
#   define GL_FRAMEBUFFER_COMPLETE_OES   GL_FRAMEBUFFER_COMPLETE_EXT
#   define GL_COLOR_ATTACHMENT0_OES   GL_COLOR_ATTACHMENT0_EXT
#   define glFramebufferTexture2DOES   glFramebufferTexture2DEXT
#   define glCheckFramebufferStatusOES   glCheckFramebufferStatusEXT
#   define glDeleteFramebuffersOES glDeleteFramebuffersEXT
#   define MAIN_BUFFER 0
# endif

# include "gl_core.h"
#endif


bool plasmacore_is_id( char ch );
int  plasmacore_argb_to_rgba( int argb );
void plasmacore_swap_red_and_blue( BardInt32* data, int count );


void plasmacore_init();
void plasmacore_shut_down();
void plasmacore_configure( int default_display_width, int default_display_height, bool force_default_size, bool allow_new_orientation );
void plasmacore_configure_graphics();
void plasmacore_scale_to_fit( int new_width, int new_height );

void plasmacore_queue_signal( BardObject* type );
void plasmacore_queue_signal( BardObject* type, BardObject* arg );
void plasmacore_queue_signal( const char* type, const char* value );
void plasmacore_queue_signal( BardObject* type, BardObject* object, double x, double y=0.0  );
void plasmacore_queue_signal( BardObject* type, int id, int index, bool flag=false,
    double x=0.0, double y=0.0 );
void plasmacore_raise_pending_signals();
void plasmacore_raise_signal( const char* type );
void plasmacore_raise_signal( const char* type, const char* value );

void plasmacore_launch();
bool plasmacore_update();
void plasmacore_draw();
void plasmacore_clear_transforms();
void plasmacore_on_exit_request();

BardObject* plasmacore_find_event_key( const char* event_type );

void NativeLayer_alert( const char* mesg );
void NativeLayer_configure_graphics();
void NativeLayer_set_projection_transform();
void NativeLayer_set_view_transform();
void NativeLayer_set_window_title( const char* title );
void NativeLayer_sleep( int ms );

void NativeLayer_begin_draw();
void NativeLayer_end_draw();
void NativeLayer_shut_down();

void Application__log__String();
void Application__title__String();

void Bitmap__copy_pixels_to__Int32_Int32_Int32_Int32_Bitmap_Int32_Int32_Logical();
void Bitmap__init__ArrayList_of_Byte();
void Bitmap__init__String();
void Bitmap__to_png_bytes();
void Bitmap__to_jpg_bytes__Real64();
void Bitmap__rotate_right();
void Bitmap__rotate_left();
void Bitmap__rotate_180();
void Bitmap__flip_horizontal();
void Bitmap__flip_vertical();
void Bitmap__resize_horizontal__Int32();
void Bitmap__resize_vertical__Int32();

void Display__flush();
void Display__fullscreen();
void Display__fullscreen__Logical();
void Display__last_draw_time_ms();
void Display__native_set_clipping_region__Box();
void Display__native_set_draw_target__OffscreenBuffer_Logical();
void Display__orientation();
void Display__screen_shot__Bitmap();
void Display__native_scale_to_fit__Int32_Int32();

void Image__uv();
void Image__uv__Corners();

void Input__mouse_position__Vector2();
void Input__mouse_visible__Logical();
void Input__keyboard_visible__Logical();
void Input__keyboard_visible();
void Input__input_capture__Logical();

void LineManager__draw__Line_Color_Render();

void Matrix4Manager__inverse__Matrix4();

void Model__native_init__Int32_Int32_Int32();
void Model__define_triangles__Array_of_Char();
void Model__define_frame__Int32_Array_of_Vertex();
void Model__native_draw__Texture();

void NativeSound__init__String();
void NativeSound__init__ArrayList_of_Byte();

void NativeSound__create_duplicate();
void NativeSound__play();
void NativeSound__pause();
void NativeSound__is_playing();
void NativeSound__volume__Real64();
void NativeSound__pan__Real64();
void NativeSound__pitch__Real64();
void NativeSound__repeats__Logical();
void NativeSound__current_time();
void NativeSound__current_time__Real64();
void NativeSound__duration();

void OffscreenBuffer__clear__Color();

void QuadManager__fill__Quad_ColorGradient_Render();

void ResourceManager__load_data_file__String();
void ResourceManager__load_gamestate__String();
void ResourceManager__save_gamestate__String_String();
void ResourceManager__delete_gamestate__String();

void Scene3D__native_begin_draw();
void Scene3D__native_end_draw();

void System__device_id();
void System__max_texture_size();
void System__open_url__String();
void System__country_name();
void System__target_fps__Int32();

void SystemMonitor__log_drawing__Logical();

void Texture__init__Bitmap_Int32();
void Texture__init__String_Int32();
void Texture__init__Vector2_Int32();
void Texture__init__Vector2();
void Texture__native_release();
void Texture__draw__Corners_Vector2_Color_Render_Blend();
void Texture__draw__Corners_Vector2_Color_Render_Blend_Texture_Corners();
void Texture__draw__Corners_Quad_ColorGradient_Render_Blend();
void Texture__draw__Vector2_Vector2_Vector2_Triangle_Color_Color_Color_Render_Blend();
void Texture__draw_tile__Corners_Vector2_Vector2_Int32();
void Texture__set__Bitmap();
void Texture__set__Bitmap_Vector2();

/*
void TransformManager__create_from__Vector2_Vector2_Radians_Vector2_Vector2_Logical_Logical();
void TransformManager__current();
void TransformManager__inverse__Transform();
void TransformManager__opMUL__Transform_Transform();
void TransformManager__push_object_transform__Transform();
void TransformManager__pop_object_transform();
void TransformManager__push_camera_transform__Transform();
void TransformManager__pop_camera_transform();
*/

void TriangleManager__fill__Triangle_Color_Color_Color_Render();

void Vector2Manager__draw__Vector2_Color_Render();

#endif // PLASMACORE_H

