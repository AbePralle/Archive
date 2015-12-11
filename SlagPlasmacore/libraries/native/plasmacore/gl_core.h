#ifndef GL_CORE_H
#define GL_CORE_H
//=============================================================================
// gl_core.h
//
// $(PLASMACORE_VERSION) $(DATE)
//
// http://plasmaworks.com/plasmacore
// 
// Code common to the OpenGL/OpenGL ES versions of Plasmacore.
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

#include "plasmacore.h"

#if defined(__APPLE__) || defined(ANDROID)
#define SWAP_RED_AND_BLUE 1
#endif

struct GLVertex2
{
  GLfloat x,y;
  GLVertex2() { }
  GLVertex2( float _x, float _y ) : x(_x), y(_y) {}

  void transform();
};

struct GLModel : BardResource
{
  GLfloat** frames;
  GLfloat*  uv;
  GLushort* triangles;
  int       frame_count;
  int       vertex_count;
  int       triangle_count;
  int       frame;  // current frame

  GLModel( int num_triangles, int num_vertices, int num_frames );
  ~GLModel();
};

struct GLTexture : BardResource
{
  static void on_textures_lost();

  GLuint id, frame_buffer;
  int image_width;
  int image_height;
  int texture_width;
  int texture_height;
  GLTexture* next_texture;

  GLTexture( int w, int h, bool offscreen_buffer );

  void resize( int w, int h );

  ~GLTexture() { destroy(); }

  void destroy();
};

extern GLTexture* all_textures;

//=============================================================================
//  GLShader
//=============================================================================
void plasmacore_check_for_shader_compile_errors( int shader );

struct GLShader
{
  char* name; 
  int vertex_shader;
  int pixel_shader;
  int program;


  GLShader( const char* name );
  virtual ~GLShader();
  virtual void prepare() = 0;
  virtual void reset();
};

struct ShaderList : AllocList<GLShader*>
{
  void      reset_shaders();
  GLShader* find( const char* name );
};

struct GLSolidColorShader : GLShader
{
  int transform_setting;
  int position_attribute;
  int color_attribute;

  GLSolidColorShader();
  void prepare();
};

struct GLAlphaTextureShader : GLShader
{
  int transform_setting;
  int position_attribute;
  int color_attribute;
  int uv_attribute;
  int texture_setting;

  GLAlphaTextureShader();
  void prepare();
};

struct GLFixedColorAlphaTextureShader : GLShader
{
  int transform_setting;
  int position_attribute;
  int color_attribute;
  int uv_attribute;
  int texture_setting;

  GLFixedColorAlphaTextureShader();
  void prepare();
};

extern ShaderList shader_list;
extern GLSolidColorShader* solid_color_shader;
extern GLAlphaTextureShader* alpha_texture_shader;
extern GLFixedColorAlphaTextureShader* fixed_color_alpha_texture_shader;

//=============================================================================
//  DrawBuffer
//=============================================================================
struct DrawBuffer
{
  GLVertex2  vertices[MAX_BUFFERED_VERTICES];
  GLVertex2  uv[MAX_BUFFERED_VERTICES];
  GLVertex2  alpha_uv[MAX_BUFFERED_VERTICES];
  BardInt32 colors[MAX_BUFFERED_VERTICES];

  int draw_mode;
  int count;
  int render_flags;
  GLVertex2* vertex_pos;
  GLVertex2* uv_pos;
  GLVertex2* alpha_uv_pos;
  BardInt32*    color_pos;
  int constant_color;
  int src_blend, dest_blend;

  GLTexture* texture;
  GLTexture* alpha_src;
  GLTexture* draw_target;

  DrawBuffer();
  void reset();
  void set_render_flags( int flags, int src_blend, int dest_blend );
  void set_textured_triangle_mode( GLTexture* texture, GLTexture* alpha_src );
  void set_solid_triangle_mode();
  void set_line_mode();
  void set_point_mode();
  void set_draw_target( GLTexture* target );
  void add( GLVertex2 v1, GLVertex2 v2, GLVertex2 v3,
      BardInt32 color1, BardInt32 color2, BardInt32 color3,
      GLVertex2 uv1, GLVertex2 uv2, GLVertex2 uv3 );
  void add( GLVertex2 v1, GLVertex2 v2, GLVertex2 v3,
      BardInt32 color1, BardInt32 color2, BardInt32 color3,
      GLVertex2 uv1, GLVertex2 uv2, GLVertex2 uv3,
      GLVertex2 alpha_uv1, GLVertex2 alpha_uv2, GLVertex2 alpha_uv3 );
  void add( GLVertex2 v1, GLVertex2 v2, GLVertex2 v3,
      BardInt32 color1, BardInt32 color2, BardInt32 color3 );
  void add( GLVertex2 v1, GLVertex2 v2, BardInt32 color );
  void add( GLVertex2 v, BardInt32 color );
  void add_box( double x1, double y1, double width, double height, BardInt32 color );
  void render();
};

void Texture_init( BardInt32* data, int w, int h, int format );

extern DrawBuffer draw_buffer;

#endif // GL_CORE_H

