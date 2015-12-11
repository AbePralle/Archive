//=============================================================================
// gl_core.cpp
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

#include "gl_core.h"

#define GL_ERROR_TEST { GLuint err = glGetError(); if (err != GL_NO_ERROR) printf("GL ERROR $%x\n",err);}

#if defined(MOBILE_PLATFORM)
#  define GL_LOWP " lowp "
#else
#  define GL_LOWP " "
#endif

void Shader__draw_arrays__Int32_Int32();
void Shader__native_compile__String_String();
void Shader__get_var_handle__Int32_String();
void Shader__set_var__Int32_Int32_ArrayList_of_Real32_Int32();
void Shader__set_var__Int32_Int32_Int32_Texture();
void Shader__set_var_colors__Int32_Int32_ArrayList_of_Int32();
void Shader__configure__Int32_Int32_Int32();

struct GLShaderInfo : BardResource
{
  const char* vertex_shader_src;
  const char* pixel_shader_src;
  int vertex_shader;
  int pixel_shader;
  int program;

  GLShaderInfo( const char* vertex_shader_src, const char* pixel_shader_src ) 
    : vertex_shader_src(vertex_shader_src), pixel_shader_src(pixel_shader_src),
      vertex_shader(0), pixel_shader(0), program(0)
  {
    // *_shader_src is assumed to be dynamically allocated.  We adopt them
    // and free them later.
  }

  ~GLShaderInfo()
  {
    if (vertex_shader_src)
    {
      delete vertex_shader_src;
      vertex_shader_src = NULL;
    }

    if (pixel_shader_src)
    {
      delete pixel_shader_src;
      pixel_shader_src = NULL;
    }
  }

  void reset()
  {
    if (vertex_shader)
    {
      glDeleteShader( vertex_shader );
      glDeleteShader( pixel_shader );
      glDeleteProgram( program );
      vertex_shader = 0;
      pixel_shader = 0;
      program = 0;
    }
  }

  void compile()
  {
    reset();

    vertex_shader = glCreateShader( GL_VERTEX_SHADER );
    pixel_shader = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( vertex_shader, 1, (const char**) &vertex_shader_src, NULL );
    glCompileShader( vertex_shader );
    check_for_shader_compile_errors( vertex_shader );

    glShaderSource( pixel_shader, 1, (const char**) &pixel_shader_src, NULL );
    glCompileShader( pixel_shader );
    check_for_shader_compile_errors( pixel_shader );

    if ( !program )
    {
      program = glCreateProgram();
      glAttachShader( program, vertex_shader );
      glAttachShader( program, pixel_shader );
    }

    glLinkProgram( program );
  }

  void check_for_shader_compile_errors( int shader )
  {
    char buffer[256];
    GLsizei len;
    glGetShaderInfoLog( shader, 256, &len, (char*) buffer );
    if (len)
    {
      TRACE( "Error compiling shader:" );
      TRACE( buffer );
    }
  }
};

bool use_scissor = false;

DrawBuffer draw_buffer;
GLTexture* all_textures = NULL;

ShaderList shader_list;
GLSolidColorShader* solid_color_shader = NULL;
GLAlphaTextureShader* alpha_texture_shader = NULL;
GLFixedColorAlphaTextureShader* fixed_color_alpha_texture_shader = NULL;

//-----------------------------------------------------------------------------
//  Plasmacore
//-----------------------------------------------------------------------------
void plasmacore_configure_graphics()
{
  if ( !solid_color_shader )
  {
    solid_color_shader = new GLSolidColorShader();
    alpha_texture_shader = new GLAlphaTextureShader();
    fixed_color_alpha_texture_shader = new GLFixedColorAlphaTextureShader();
  }
}

void NativeLayer_configure_graphics()
{
  bard_register_native_method( "Shader","draw_arrays(Int32,Int32)", Shader__draw_arrays__Int32_Int32 );
  bard_register_native_method( "Shader","native_compile(String,String)", 
      Shader__native_compile__String_String );
  bard_register_native_method( "Shader","get_var_handle(Int32,String)", Shader__get_var_handle__Int32_String );
  bard_register_native_method( "Shader","set_var(Int32,Int32,ArrayList<<Real32>>,Int32)", 
      Shader__set_var__Int32_Int32_ArrayList_of_Real32_Int32 );
  bard_register_native_method( "Shader","set_var(Int32,Int32,Int32,Texture)", Shader__set_var__Int32_Int32_Int32_Texture );
  bard_register_native_method( "Shader","set_var_colors(Int32,Int32,ArrayList<<Int32>>)", 
      Shader__set_var_colors__Int32_Int32_ArrayList_of_Int32 );
  bard_register_native_method( "Shader","configure(Int32,Int32,Int32)", Shader__configure__Int32_Int32_Int32 );

}

//=============================================================================
//  BardVertex
//=============================================================================
struct BardVertex : BardObject
{
  float x, y, z, u, v;
};

//=============================================================================
//  GLVertex2
//=============================================================================
void GLVertex2::transform()
{
  // 2D transform
  float new_x = (float)(plasmacore.view_transform.r1c1 * x + plasmacore.view_transform.r1c2 * y + plasmacore.view_transform.r1c4);
  float new_y = (float)(plasmacore.view_transform.r2c1 * x + plasmacore.view_transform.r2c2 * y + plasmacore.view_transform.r2c4);
  x = new_x;
  y = new_y;
}

void GLTexture::resize( int w, int h )
{
  image_width  = w;
  image_height = h;
  texture_width = 1;
  while (texture_width < w) texture_width <<= 1;
  texture_height = 1;
  while (texture_height < h) texture_height <<= 1;
}

//=============================================================================
//  GLModel
//=============================================================================
GLModel::GLModel( int num_triangles, int num_vertices, int num_frames )
{
  frame_count = num_frames;
  vertex_count = num_vertices;
  triangle_count = num_triangles;
  frame = 0;
  frames = new GLfloat*[ frame_count ];
  for (int i=0; i<frame_count; ++i)
  {
    frames[i] = new GLfloat[ vertex_count*3 ];
  }
  uv = new GLfloat[ vertex_count*2 ];
  triangles = new GLushort[ triangle_count*3 ];
}

GLModel::~GLModel()
{
  if (triangles)
  {
    delete triangles;
    triangles = 0;

    delete uv;
    uv = 0;

    for (int i=0; i<frame_count; ++i)
    {
      delete frames[i];
    }
    delete frames;
    frames = 0;
  }
}


//=============================================================================
//  GLShader
//=============================================================================
void plasmacore_check_for_shader_compile_errors( int shader )
{
  char buffer[256];
  GLsizei len;
  glGetShaderInfoLog( shader, 256, &len, (char*) buffer );
  if (len)
  {
    LOG( buffer );
  }
}

GLShader::GLShader( const char* name ) : vertex_shader(0), pixel_shader(0), program(0)
{
  this->name = new char[ strlen(name) + 1 ];
  strcpy( this->name, name );
}

GLShader::~GLShader()
{
  reset();
  delete name;
  name = NULL;
};

void GLShader::reset()
{
  if (vertex_shader)
  {
    glDeleteShader( vertex_shader );
    glDeleteShader( pixel_shader );
    vertex_shader = 0;
    pixel_shader = 0;
    glDeleteProgram( program );
    program = 0;
  }
}


GLShader* ShaderList::find( const char* name )
{
  int this_count = count + 1;
  int ch0 = name[0];
  GLShader** cur_ptr = data - 1;
  while (--this_count)
  {
    GLShader* cur = *(++cur_ptr);
    if ( cur->name[0] == ch0 )
    {
      if (0 == strcmp(cur->name,name)) return cur;
    }
  }
  return NULL;
}

void ShaderList::reset_shaders()
{
  for (int i=0; i<count; ++i)
  {
    (*this)[i]->reset();
  }
}


//=============================================================================
// GLSolidColorShader
//=============================================================================
GLSolidColorShader::GLSolidColorShader() : GLShader("solid_color")
{
}

void GLSolidColorShader::prepare()
{
  if ( !vertex_shader )
  {
    if ( !shader_list.contains(this) ) shader_list.add( this );

    const char* vertex_shader_src = 
#if defined(ANDROID) || TARGET_OS_IPHONE
      "precision mediump float;        \n"
#endif
      "uniform mat4   transform;             \n"
      "attribute vec4 position;              \n"
      "attribute" GL_LOWP "vec4 color;            \n"
      "varying " GL_LOWP "vec4 vertex_color;     \n"
      "void main()                           \n"
      "{                                     \n"
      "  gl_Position = transform * position; \n"
      "  vertex_color = color / 255.0;       \n"
      "}                                     \n";

    const char* pixel_shader_src = 
#if defined(ANDROID) || TARGET_OS_IPHONE
      "precision mediump float;        \n"
#endif
      "varying" GL_LOWP "vec4 vertex_color; \n"
      "void main()                      \n"
      "{                                \n"
      "  gl_FragColor = vertex_color;   \n"
      "}                                \n";

    vertex_shader = glCreateShader( GL_VERTEX_SHADER );
    pixel_shader = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( vertex_shader, 1, (const char**) &vertex_shader_src, NULL );
    glCompileShader( vertex_shader );
    plasmacore_check_for_shader_compile_errors( vertex_shader );

    glShaderSource( pixel_shader, 1, (const char**) &pixel_shader_src, NULL );
    glCompileShader( pixel_shader );
    plasmacore_check_for_shader_compile_errors( pixel_shader );

    if ( !program )
    {
      program = glCreateProgram();
      glAttachShader( program, vertex_shader );
      glAttachShader( program, pixel_shader );
    }

    glLinkProgram( program );

    transform_setting = glGetUniformLocation( program, "transform" );
    position_attribute = glGetAttribLocation( program, "position" );
    color_attribute = glGetAttribLocation( program, "color" );
  }

  glUseProgram( program );

  GLfloat m32[16];
  plasmacore.projection_transform.to_floats( m32 );
  glUniformMatrix4fv( transform_setting, 1, GL_FALSE, m32 );

  glVertexAttribPointer( position_attribute, 2, GL_FLOAT, GL_FALSE, 0, draw_buffer.vertices );
  glVertexAttribPointer( color_attribute, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, draw_buffer.colors );
  glEnableVertexAttribArray( position_attribute );
  glEnableVertexAttribArray( color_attribute );
}


//=============================================================================
// GLAlphaTextureShader
//=============================================================================
GLAlphaTextureShader::GLAlphaTextureShader() : GLShader("alpha_texture")
{
}

void GLAlphaTextureShader::prepare()
{
  if ( !vertex_shader )
  {
    if ( !shader_list.contains(this) ) shader_list.add( this );

    const char* vertex_shader_src = 
#if defined(ANDROID) || TARGET_OS_IPHONE
      "precision mediump float;        \n"
#endif
      "uniform             mat4 transform;    \n"
      "attribute           vec4 position;     \n"
      "attribute" GL_LOWP "vec4 color;        \n"
      "attribute           vec2 uv;           \n"
      "varying  " GL_LOWP "vec4 vertex_color; \n"
      "varying             vec2 vertex_uv;    \n"
      "void main()                            \n"
      "{                                      \n"
      "  gl_Position = transform * position;  \n"
      "  vertex_color = color / 255.0;        \n"
      "  vertex_uv = uv;                      \n"
      "}                                      \n";

    const char* pixel_shader_src = 
#if defined(ANDROID) || TARGET_OS_IPHONE
      "precision mediump float;        \n"
#endif
      "uniform   sampler2D texture;                       \n"
      "varying" GL_LOWP "vec4 vertex_color;               \n"
      "varying           vec2 vertex_uv;                  \n"
      "void main()                                        \n"
      "{                                                  \n"
      "  gl_FragColor = texture2D(texture,vertex_uv) * vertex_color;  \n"
      "}                                                  \n";

    vertex_shader = glCreateShader( GL_VERTEX_SHADER );
    pixel_shader = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( vertex_shader, 1, (const char**) &vertex_shader_src, NULL );
    glCompileShader( vertex_shader );
    plasmacore_check_for_shader_compile_errors( vertex_shader );

    glShaderSource( pixel_shader, 1, (const char**) &pixel_shader_src, NULL );
    glCompileShader( pixel_shader );
    plasmacore_check_for_shader_compile_errors( pixel_shader );

    if ( !program )
    {
      program = glCreateProgram();
      glAttachShader( program, vertex_shader );
      glAttachShader( program, pixel_shader );
    }

    glLinkProgram( program );

    transform_setting = glGetUniformLocation( program, "transform" );
    texture_setting   = glGetUniformLocation( program, "texture" );
    position_attribute = glGetAttribLocation( program, "position" );
    color_attribute = glGetAttribLocation( program, "color" );
    uv_attribute = glGetAttribLocation( program, "uv" );
  }

  glUseProgram( program );

  //glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE );
  //glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE );
  //glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );

  GLfloat m32[16];
  plasmacore.projection_transform.to_floats( m32 );
  glUniformMatrix4fv( transform_setting, 1, GL_FALSE, m32 );
  glUniform1i( texture_setting, 0 );

  glVertexAttribPointer( position_attribute, 2, GL_FLOAT, GL_FALSE, 0, draw_buffer.vertices );
  glVertexAttribPointer( color_attribute, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, draw_buffer.colors );
  glVertexAttribPointer( uv_attribute, 2, GL_FLOAT, GL_FALSE, 0, draw_buffer.uv );
  glEnableVertexAttribArray( position_attribute );
  glEnableVertexAttribArray( color_attribute );
  glEnableVertexAttribArray( uv_attribute );
}


//=============================================================================
// GLFixedColorAlphaTextureShader
//=============================================================================
GLFixedColorAlphaTextureShader::GLFixedColorAlphaTextureShader() : GLShader("fixed_color_alpha_texture")
{
}

void GLFixedColorAlphaTextureShader::prepare()
{
  if ( !vertex_shader )
  {
    if ( !shader_list.contains(this) ) shader_list.add( this );

    const char* vertex_shader_src = 
#if defined(ANDROID) || TARGET_OS_IPHONE
      "precision mediump   float;                           \n"
#endif
      "uniform mat4   transform;             \n"
      "attribute vec4 position;              \n"
      "attribute" GL_LOWP "vec4 color;       \n"
      "attribute vec2 uv;                    \n"
      "varying" GL_LOWP "vec4 vertex_color;  \n"
      "varying   vec2 vertex_uv;             \n"
      "void main()                           \n"
      "{                                     \n"
      "  gl_Position = transform * position; \n"
      "  vertex_color = color / 255.0;       \n"
      "  vertex_uv = uv;                     \n"
      "}                                     \n";

    const char* pixel_shader_src = 
#if defined(ANDROID) || TARGET_OS_IPHONE
      "precision mediump   float;                           \n"
#endif
      "uniform   sampler2D texture;                         \n"
      "varying" GL_LOWP "vec4 vertex_color;                 \n"
      "varying           vec2 vertex_uv;                    \n"
      "void main()                                          \n"
      "{                                                    \n"
      "  float a = vertex_color.a * texture2D(texture,vertex_uv).a; \n"
      "  gl_FragColor = vec4( vertex_color.rgb * a, a );    \n"
      "}                                                    \n";

    vertex_shader = glCreateShader( GL_VERTEX_SHADER );
    pixel_shader = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( vertex_shader, 1, (const char**) &vertex_shader_src, NULL );
    glCompileShader( vertex_shader );
    plasmacore_check_for_shader_compile_errors( vertex_shader );

    glShaderSource( pixel_shader, 1, (const char**) &pixel_shader_src, NULL );
    glCompileShader( pixel_shader );
    plasmacore_check_for_shader_compile_errors( pixel_shader );

    if ( !program )
    {
      program = glCreateProgram();
      glAttachShader( program, vertex_shader );
      glAttachShader( program, pixel_shader );
    }

    glLinkProgram( program );

    transform_setting = glGetUniformLocation( program, "transform" );
    texture_setting   = glGetUniformLocation( program, "texture" );
    position_attribute = glGetAttribLocation( program, "position" );
    color_attribute = glGetAttribLocation( program, "color" );
    uv_attribute = glGetAttribLocation( program, "uv" );
  }

  glUseProgram( program );

  //glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE );
  //glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE );
  //glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );

  GLfloat m32[16];
  plasmacore.projection_transform.to_floats( m32 );
  glUniformMatrix4fv( transform_setting, 1, GL_FALSE, m32 );
  glUniform1i( texture_setting, 0 );

  glVertexAttribPointer( position_attribute, 2, GL_FLOAT, GL_FALSE, 0, draw_buffer.vertices );
  glVertexAttribPointer( color_attribute, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, draw_buffer.colors );
  glVertexAttribPointer( uv_attribute, 2, GL_FLOAT, GL_FALSE, 0, draw_buffer.uv );
  glEnableVertexAttribArray( position_attribute );
  glEnableVertexAttribArray( color_attribute );
  glEnableVertexAttribArray( uv_attribute );
}


//=============================================================================
//  DrawBuffer
//=============================================================================
DrawBuffer::DrawBuffer()
{
  draw_mode = DRAW_TEXTURED_TRIANGLES;
  texture = NULL;
  draw_target = NULL;
  alpha_src = NULL;
  render_flags = 0;
  src_blend = BLEND_SRC_ALPHA;
  dest_blend = BLEND_INVERSE_SRC_ALPHA;
  constant_color = 0;

  reset();
}

void DrawBuffer::reset()
{
  count = 0;
  vertex_pos = vertices;
  uv_pos     = uv;
  alpha_uv_pos = alpha_uv;
  color_pos  = colors;
}

void DrawBuffer::set_render_flags( int flags, int src_blend, int dest_blend )
{
  if (render_flags != flags || this->src_blend != src_blend || this->dest_blend != dest_blend) render();
  render_flags = flags;
  this->src_blend = src_blend;
  this->dest_blend = dest_blend;
}

void DrawBuffer::set_textured_triangle_mode( GLTexture* texture, GLTexture* alpha_src )
{
  if (draw_mode != DRAW_TEXTURED_TRIANGLES || this->texture != texture || this->alpha_src != alpha_src) render();
  draw_mode = DRAW_TEXTURED_TRIANGLES;
  this->texture = texture;
  this->alpha_src = alpha_src;
}

void DrawBuffer::set_solid_triangle_mode()
{
  if (draw_mode != DRAW_SOLID_TRIANGLES) render();
  draw_mode = DRAW_SOLID_TRIANGLES;
}

void DrawBuffer::set_line_mode()
{
  if (draw_mode != DRAW_LINES) render();
  draw_mode = DRAW_LINES;
}


void DrawBuffer::set_point_mode()
{
  if (draw_mode != DRAW_POINTS) render();
  draw_mode = DRAW_POINTS;
}

void DrawBuffer::set_draw_target( GLTexture* target )
{
  if (draw_target == target) return;

  render();
  draw_target = target;

  if (draw_target) 
  {
    glBindFramebuffer( GL_FRAMEBUFFER, draw_target->frame_buffer );
    glBindTexture( GL_TEXTURE_2D, 0 );
    glViewport(0, 0, draw_target->texture_width, draw_target->texture_height );
  }
  else 
  {
    glBindFramebuffer( GL_FRAMEBUFFER, MAIN_BUFFER );

    glViewport(0, 0, plasmacore.display_width, plasmacore.display_height );
    glEnable( GL_BLEND );
  }
}

void DrawBuffer::add( GLVertex2 v1, GLVertex2 v2, GLVertex2 v3,
    BardInt32 color1, BardInt32 color2, BardInt32 color3,
    GLVertex2 uv1, GLVertex2 uv2, GLVertex2 uv3 )
{
  if (count == MAX_BUFFERED_VERTICES) render();

  if ((render_flags & RENDER_FLAG_FIXED_COLOR) && constant_color != color1)
  {
    render();
    constant_color = color1;
  }

  vertex_pos[0] = v1;
  vertex_pos[1] = v2;
  vertex_pos[2] = v3;
  vertex_pos += 3;
  color_pos[0] = color1;
  color_pos[1] = color2;
  color_pos[2] = color3;
  color_pos += 3;
  uv_pos[0] = uv1;
  uv_pos[1] = uv2;
  uv_pos[2] = uv3;
  uv_pos += 3;
  count += 3;
}

void DrawBuffer::add( GLVertex2 v1, GLVertex2 v2, GLVertex2 v3,
    BardInt32 color1, BardInt32 color2, BardInt32 color3,
    GLVertex2 uv1, GLVertex2 uv2, GLVertex2 uv3,
    GLVertex2 alpha_uv1, GLVertex2 alpha_uv2, GLVertex2 alpha_uv3 )
{
  if (count == MAX_BUFFERED_VERTICES) render();

  if ((render_flags & RENDER_FLAG_FIXED_COLOR) && constant_color != color1)
  {
    render();
    constant_color = color1;
  }

  vertex_pos[0] = v1;
  vertex_pos[1] = v2;
  vertex_pos[2] = v3;
  vertex_pos += 3;
  color_pos[0] = color1;
  color_pos[1] = color2;
  color_pos[2] = color3;
  color_pos += 3;
  uv_pos[0] = uv1;
  uv_pos[1] = uv2;
  uv_pos[2] = uv3;
  uv_pos += 3;
  alpha_uv_pos[0] = alpha_uv1;
  alpha_uv_pos[1] = alpha_uv2;
  alpha_uv_pos[2] = alpha_uv3;
  alpha_uv_pos += 3;
  count += 3;
}


void DrawBuffer::add( GLVertex2 v1, GLVertex2 v2, GLVertex2 v3,
    BardInt32 color1, BardInt32 color2, BardInt32 color3 )
{
  if (count == MAX_BUFFERED_VERTICES) render();
  vertex_pos[0] = v1;
  vertex_pos[1] = v2;
  vertex_pos[2] = v3;
  vertex_pos += 3;
  color_pos[0] = color1;
  color_pos[1] = color2;
  color_pos[2] = color3;
  color_pos += 3;
  count += 3;
}

void DrawBuffer::add( GLVertex2 v1, GLVertex2 v2, BardInt32 color )
{
  if (count == MAX_BUFFERED_VERTICES) render();
  vertex_pos[0] = v1;
  vertex_pos[1] = v2;
  vertex_pos += 2;
  color_pos[0] = color;
  color_pos[1] = color;
  color_pos += 2;
  count += 2;
}

void DrawBuffer::add( GLVertex2 v, BardInt32 color )
{
  if (count == MAX_BUFFERED_VERTICES) render();
  *(vertex_pos++) = v;
  *(color_pos++) = color;
  ++count;
}

void DrawBuffer::add_box( double x1, double y1, double width, double height, BardInt32 color )
{
  set_draw_target( NULL );
  set_render_flags( 0, BLEND_ONE, BLEND_ZERO );
  set_solid_triangle_mode();
  double x2 = x1 + width;
  double y2 = y1 + height;
  GLVertex2 v1( x1, y1 );
  GLVertex2 v2( x2, y1 );
  GLVertex2 v3( x2, y2 );
  GLVertex2 v4( x1, y2 );
  add( v1, v2, v4, color, color, color );
  add( v4, v2, v3, color, color, color );
}

void DrawBuffer::render()
{
  if ( !count ) return;

  //glEnableClientState(GL_COLOR_ARRAY);
  //glEnableClientState(GL_VERTEX_ARRAY);

  GLenum src_factor, dest_factor;
  switch (src_blend)
  {
    case BLEND_ZERO:
      src_factor = GL_ZERO;
      break;
    case BLEND_ONE:
      src_factor = GL_ONE;
      break;
    case BLEND_SRC_ALPHA:
      src_factor = GL_SRC_ALPHA;
      break;
    case BLEND_INVERSE_SRC_ALPHA:
      src_factor = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case BLEND_DEST_ALPHA:
      src_factor = GL_DST_ALPHA;
      break;
    case BLEND_INVERSE_DEST_ALPHA:
      src_factor = GL_ONE_MINUS_DST_ALPHA;
      break;
    case BLEND_DEST_COLOR:
      src_factor = GL_DST_COLOR;
      break;
    case BLEND_INVERSE_DEST_COLOR:
      src_factor = GL_ONE_MINUS_DST_COLOR;
      break;
    case BLEND_OPAQUE:
      src_factor = GL_SRC_ALPHA_SATURATE;
      break;
    default:
      src_factor = GL_SRC_ALPHA;
  }
  switch (dest_blend)
  {
    case BLEND_ZERO:
      dest_factor = GL_ZERO;
      break;
    case BLEND_ONE:
      dest_factor = GL_ONE;
      break;
    case BLEND_SRC_ALPHA:
      dest_factor = GL_SRC_ALPHA;
      break;
    case BLEND_INVERSE_SRC_ALPHA:
      dest_factor = GL_ONE_MINUS_SRC_ALPHA;
      break;
    case BLEND_DEST_ALPHA:
      dest_factor = GL_DST_ALPHA;
      break;
    case BLEND_INVERSE_DEST_ALPHA:
      dest_factor = GL_ONE_MINUS_DST_ALPHA;
      break;
    case BLEND_SRC_COLOR:
      dest_factor = GL_SRC_COLOR;
      break;
    case BLEND_INVERSE_SRC_COLOR:
      dest_factor = GL_ONE_MINUS_SRC_COLOR;
      break;
    default:
      dest_factor = GL_ONE_MINUS_SRC_ALPHA;
  }

  glBlendFunc( src_factor, dest_factor );

  switch (draw_mode)
  {
    case DRAW_TEXTURED_TRIANGLES:
      {
        // set colorization flags
        //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glActiveTexture( GL_TEXTURE0 );
        //glEnable(GL_TEXTURE_2D);
        glBindTexture( GL_TEXTURE_2D, texture->id );


        /*
           if (render_flags & RENDER_FLAG_FIXED_COLOR)
           {
           GLfloat env_color[4] = {
           ((constant_color) & 255)/255.0f, 
           ((constant_color >> 8) & 255)/255.0f, 
           ((constant_color >> 16) & 255)/255.0f,
           ((constant_color >> 24) & 255)/255.0f };
           glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env_color );
           glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_RGB, GL_CONSTANT); 
           glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
           }
           else
           {
           glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
           }
           */

        GLShader* cur_shader;
        if (render_flags & RENDER_FLAG_FIXED_COLOR)
        {
          cur_shader = fixed_color_alpha_texture_shader;
        }
        else
        {
          cur_shader = alpha_texture_shader;
        }

        // set texture addressing to WRAP or CLAMP
        if (render_flags & RENDER_FLAG_TEXTURE_WRAP)
        {
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        }
        else
        {
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
        }

        if (render_flags & RENDER_FLAG_POINT_FILTER)
        {
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else
        {
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        /*
           if (alpha_src)
           {
           glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
           glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PRIMARY_COLOR);

           glActiveTexture( GL_TEXTURE1 );
           glEnable( GL_TEXTURE_2D );
           glBindTexture( GL_TEXTURE_2D, alpha_src->id );
           glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
           glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // grab color from first stage
        glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE );
        glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS );

        // use the alpha from second stage
        glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE );
        glTexEnvf( GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE );

        glActiveTexture( GL_TEXTURE0 );

        }
        else
        {
          glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE );
          glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE );
          glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
        }
        */

        cur_shader->prepare();
        glDrawArrays(GL_TRIANGLES, 0, count);
        /*
           glDisable(GL_TEXTURE_2D);
           glDisableClientState(GL_TEXTURE_COORD_ARRAY);

           if (alpha_src)
           {
           glActiveTexture( GL_TEXTURE1 );
           glDisable( GL_TEXTURE_2D );
           glActiveTexture( GL_TEXTURE0 );
           }
           */
      }
      break;

    case DRAW_SOLID_TRIANGLES:
      solid_color_shader->prepare();
      glDrawArrays(GL_TRIANGLES, 0, count);
      break;

    case DRAW_LINES:
      solid_color_shader->prepare();
      glDrawArrays( GL_LINES, 0, count );
      break;

    case DRAW_POINTS:
      solid_color_shader->prepare();
      glDrawArrays( GL_POINTS, 0, count );
      break;
  }
#ifdef OLD_OPENGL_V1
  glDisableClientState(GL_COLOR_ARRAY);
#endif // OLD_OPENGL_V1

  if (draw_target) 
  {
    // flushes offscreen buffer drawing queue
    glBindFramebuffer( GL_FRAMEBUFFER, MAIN_BUFFER ); 
    glBindFramebuffer( GL_FRAMEBUFFER, draw_target->frame_buffer );
  }

  reset();
}


void NativeLayer_set_projection_transform()
{
  //GLfloat m32[16];
  //plasmacore.projection_transform.to_floats( m32 );
  //glUniformMatrix4fv( plasmacore_vertex_shader__transform, 1, GL_FALSE, m32 );
//char buffer[512];
//sprintf( buffer, "%.4lf -> %.4f\n", plasmacore.projection_transform.r1c1, m32[0] );
//LOG( buffer );
  /*
  m32[0] = 0.0020f;
  m32[1] = 0;
  m32[2] = 0;
  m32[3] = 0;
  m32[4] = 0;
  m32[5] = -0.0026f;
  m32[6] = 0;
  m32[7] = 0;
  m32[8] = 0;
  m32[9] = 0;
  m32[10] = 0.0010f;
  m32[11] = 0;
  m32[12] = -1;
  m32[13] = 1;
  m32[14] = 0;
  m32[15] = 1;
  */


#ifdef OLD_OPENGL_V1
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

#if TARGET_OS_IPHONE || (ANDROID)
  //glOrthox( 0, plasmacore.display_width<<16, plasmacore.display_height<<16, 0, -1<<16, 1<<16 );
  BardInt32 m32[16];
  plasmacore.projection_transform.to_fixed( m32 );
  glMultMatrixx( m32 );
#else
  //glOrtho( 0, plasmacore.display_width, plasmacore.display_height, 0, -1, 1 );
  glMultMatrixd( (double*) &plasmacore.projection_transform );
#endif

  /*
  bool modified = false;

  int  camera_count = plasmacore.camera_transform_stack_count;
  if (plasmacore.camera_transform_stack_modified)
  {
    modified = true;
    plasmacore.camera_transform_stack_modified = false;

    if (camera_count > 0)
    {
      Matrix2x3* mptr = plasmacore.camera_transform_stack - 1;

      plasmacore.camera_transform = *(++mptr);

      while (--camera_count)
      {
        plasmacore.camera_transform = Matrix2x3_multiply( plasmacore.camera_transform, *(++mptr) );
      }
    }
    else
    {
      plasmacore.camera_transform = Matrix2x3(1,0,0,0,1,0);
    }
  }

  int  object_count = plasmacore.object_transform_stack_count;
  if (plasmacore.object_transform_stack_modified)
  {
    modified = true;
    plasmacore.object_transform_stack_modified = false;

    if (object_count > 0)
    {
      Matrix2x3* mptr = plasmacore.object_transform_stack + object_count;

      plasmacore.object_transform = *(--mptr);

      while (--object_count)
      {
        plasmacore.object_transform = Matrix2x3_multiply( plasmacore.object_transform, *(--mptr) );
      }
    }
    else
    {
      plasmacore.object_transform = Matrix2x3(1,0,0,0,1,0);
    }
  }

  if (modified)
  {
    plasmacore.transform = Matrix2x3_multiply( 
        plasmacore.camera_transform, plasmacore.object_transform );
  }
  */
#endif // OLD_OPENGL_V1
}

void NativeLayer_set_view_transform()
{
  // FIXME
#ifdef OLD_OPENGL_V1
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if ( !plasmacore.mode_3d ) return;

#if TARGET_OS_IPHONE || (ANDROID)
  //glOrthox( 0, plasmacore.display_width<<16, plasmacore.display_height<<16, 0, -1<<16, 1<<16 );
  BardInt32 m32[16];
  plasmacore.view_transform.to_fixed( m32 );
  glMultMatrixx( m32 );
#else
  //glOrtho( 0, plasmacore.display_width, plasmacore.display_height, 0, -1, 1 );
  glMultMatrixd( (double*) &plasmacore.view_transform );
#endif
#endif // OLD_OPENGL_V1
}


void Display__screen_shot__Bitmap()
{
  // Application::screen_shot(Bitmap=null).Bitmap
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  BARD_DISCARD_REF();

  draw_buffer.render();

  BARD_FIND_TYPE( type_bitmap, "Bitmap" );
  if ( !bitmap_obj )
  {
    bitmap_obj = (BardBitmap*) type_bitmap->create();
  }

  int w = plasmacore.display_width;
  int h = plasmacore.display_height;

  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // method result
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // for init(Int32,Int32)
  BARD_PUSH_INT32( w );
  BARD_PUSH_INT32( h );
  BARD_CALL( type_bitmap, "init(Int32,Int32)" );  // only reallocates if wrong size

  bitmap_obj = (BardBitmap*) BARD_PEEK_REF();  // reset ref in case a gc happened

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  glReadPixels( 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, bitmap_obj->pixels->data );

  // reverse row order and maybe Red/Blue values, premultiply alpha
  BardInt32* src_start = ((BardInt32*) bitmap_obj->pixels->data);
  BardInt32* dest_start = src_start + (w*(h-1));
  while (src_start <= dest_start)
  {
    BardInt32* src  = src_start - 1;
    BardInt32* dest = dest_start - 1;
    int c = w + 1;
    while (--c)
    {
      BardInt32 color1 = *(++src);
      int r1 = (color1 >> 16) & 255;
      int g1 = (color1 >> 8) & 255;
      int b1 = color1 & 255;

      BardInt32 color2 = *(++dest);
      int r2 = (color2 >> 16) & 255;
      int g2 = (color2 >> 8) & 255;
      int b2 = color2 & 255;

#if defined(ANDROID) || TARGET_OS_IPHONE
      *src  = (0xff000000) | (b2 << 16) | (g2 << 8) | r2;
      *dest = (0xff000000) | (b1 << 16) | (g1 << 8) | r1;
#else
      *src  = (0xff000000) | (r2 << 16) | (g2 << 8) | b2;
      *dest = (0xff000000) | (r1 << 16) | (g1 << 8) | b1;
#endif
    }
    src_start  += w;
    dest_start -= w;
  }
}

void Display__native_set_clipping_region__Box()
{
  // Application::native_set_clipping_region(Box)
  Vector2 top_left = BARD_POP(Vector2);
  Vector2 size = BARD_POP(Vector2);
  BARD_DISCARD_REF();  // discard singleton

  Vector2 bottom_right( top_left.x+size.x, top_left.y+size.y );

  top_left = plasmacore.view_transform.transform( top_left );
  bottom_right = plasmacore.view_transform.transform( bottom_right );

  double x1 = top_left.x;
  double y1 = top_left.y;
  double x2 = bottom_right.x;
  double y2 = bottom_right.y;

  draw_buffer.render();

  GLint ix1 = (GLint)(x1+0.5);
  GLint iy1 = (GLint)(y1+0.5);
  GLint ix2 = (GLint)(x2+0.5);
  GLint iy2 = (GLint)(y2+0.5);

  if (ix1 > ix2)
  {
    int temp = ix1;
    ix1 = ix2;
    ix2 = temp;
  }

  if (iy1 > iy2)
  {
    int temp = iy1;
    iy1 = iy2;
    iy2 = temp;
  }

  int max_w, max_h;
  if (plasmacore.orientation == 1)
  {
    max_w = plasmacore.display_height;
    max_h = plasmacore.display_width;
  }
  else
  {
    max_w = plasmacore.display_width;
    max_h = plasmacore.display_height;
  }

  if (ix1 < 0) ix1 = 0;
  if (iy1 < 0) iy1 = 0;
  if (ix2 > max_w) ix2 = max_w;
  if (iy2 > max_h) iy2 = max_h;

  glScissor( ix1, max_h-iy2, ix2-ix1, iy2-iy1 );
  use_scissor = true;
  glEnable( GL_SCISSOR_TEST );
}

void Display__flush()
{
  BARD_DISCARD_REF();  // singleton
  draw_buffer.render();
}

GLTexture* NativeLayer_get_native_texture_data( BardObject* texture_obj )
{
  BARD_GET_REF( native_data, texture_obj, "native_data" );
  if ( !native_data ) return NULL;

  return (GLTexture*) (((BardNativeData*)native_data)->data);
}

void Display__native_set_draw_target__OffscreenBuffer_Logical()
{
  // Application::native_set_draw_target(OffscreenBuffer,Logical)
  BardInt32 blend = BARD_POP_INT32();
  BardObject* image_obj = BARD_POP_REF();
  BARD_DISCARD_REF();  // discard singleton

  draw_buffer.render();

  if ( !image_obj )
  {
    draw_buffer.set_draw_target( NULL );
    draw_buffer.render();
    return;
  }

  BARD_GET_REF( texture_obj, image_obj, "texture" );
  BVM_NULL_CHECK( texture_obj, return );

  GLTexture* target = NativeLayer_get_native_texture_data(texture_obj);
  draw_buffer.set_draw_target( target );

  if (blend) glEnable( GL_BLEND );
  else       glDisable(GL_BLEND);

  use_scissor = false;
  glDisable( GL_SCISSOR_TEST );
}

void LineManager__draw__Line_Color_Render()
{
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32 color  = BARD_POP_INT32();
  Vector2 pt1 = BARD_POP(Vector2); 
  Vector2 pt2 = BARD_POP(Vector2); 

  BARD_DISCARD_REF(); // discard singleton ref 

  //NativeLayer_update_transform();

  color = plasmacore_argb_to_rgba( color );
  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_line_mode();

  GLVertex2 v1(pt1.x, pt1.y);
  GLVertex2 v2(pt2.x, pt2.y);
  v1.transform();
  v2.transform();
  draw_buffer.add( v1, v2, color );
}

void Model__native_init__Int32_Int32_Int32()
{
  int frame_count    = BARD_POP_INT32();
  int vertex_count   = BARD_POP_INT32();
  int triangle_count = BARD_POP_INT32();
  BardObject* model_obj = BARD_POP_REF();

  GLModel* model = new GLModel( triangle_count, vertex_count, frame_count );
  BardObject* native_data = BardNativeData::create( model, BardNativeDataDeleteResource );
  BARD_SET_REF( model_obj, "native_data", native_data );
}

void Model__define_triangles__Array_of_Char()
{
  BardArray* array_obj = (BardArray*) BARD_POP_REF();
  BardObject* model_obj = BARD_POP_REF();
  BARD_GET_NATIVE_DATA( GLModel*, model, model_obj, "native_data" );
  memcpy( model->triangles, array_obj->data, model->triangle_count * 3 * 2 );
}

void Model__define_frame__Int32_Array_of_Vertex()
{
  BardArray* array_obj = (BardArray*) BARD_POP_REF();
  int frame = BARD_POP_INT32();
  BardObject* model_obj = BARD_POP_REF();
  BARD_GET_NATIVE_DATA( GLModel*, model, model_obj, "native_data" );
  if (model && frame < model->frame_count)
  {
    BardVertex** src = ((BardVertex**)(array_obj->data)) - 1;
    GLfloat* vertex_dest = model->frames[frame] - 1;
    GLfloat* uv_dest = model->uv - 1;
    int count = model->vertex_count + 1;
    while (--count)
    {
      BardVertex* v = *(++src);
      *(++vertex_dest) = v->x;
      *(++vertex_dest) = v->y;
      *(++vertex_dest) = v->z;
      *(++uv_dest) = v->u;
      *(++uv_dest) = v->v;
    }
  }
}

void Model__native_draw__Texture()
{
  // FIXME
#ifdef OLD_OPENGL_V1
  BardObject* texture_obj = BARD_POP_REF();
  BardObject* model_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data( texture_obj );
  if ( !texture ) return;

  BARD_GET_NATIVE_DATA( GLModel*, model, model_obj, "native_data" );
  if ( !model ) return;

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, texture->id );

  glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE );
  glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE );
  glTexEnvf( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );

  /*
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_STENCIL_TEST);
  */
  glDisable(GL_LIGHTING);

  NativeLayer_set_view_transform();
  /*
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  GLfloat m[] =
  {
    1,0,0,0,
    0,1,0,0,
    0,0,-1,0,
    0,0,0,1
  };
  glMultMatrixf(m);
  */
  //glFrustum(-1,1,-1,1,1,10);

  glColor4f(1.0f,1.0f,1.0f,1.0f);
  glDisableClientState(GL_COLOR_ARRAY);
  glVertexPointer( 3, GL_FLOAT, 0, model->frames[0] );
  //glEnableClientState( GL_VERTEX_ARRAY );
  //glVertexPointer( 3, GL_FLOAT, 0, vertices );

  glTexCoordPointer( 2, GL_FLOAT, 0, model->uv );
  //glColorPointer( 4, GL_UNSIGNED_BYTE, 0, draw_buffer.colors);

  glDrawElements( GL_TRIANGLES, model->triangle_count*3, GL_UNSIGNED_SHORT, model->triangles );
  //glDrawArrays( GL_TRIANGLES, 0, 6 );

  glEnableClientState( GL_COLOR_ARRAY );
#endif //OLD_OPENGL_V1
}

void QuadManager__fill__Quad_ColorGradient_Render()
{
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32 color1 = BARD_POP_INT32();
  BardInt32 color2 = BARD_POP_INT32();
  BardInt32 color3 = BARD_POP_INT32();
  BardInt32 color4 = BARD_POP_INT32();
  Vector2 pt1 = BARD_POP(Vector2); 
  Vector2 pt2 = BARD_POP(Vector2); 
  Vector2 pt3 = BARD_POP(Vector2); 
  Vector2 pt4 = BARD_POP(Vector2); 

  BARD_DISCARD_REF(); // discard singleton ref 

  //NativeLayer_update_transform();

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );
  color4 = plasmacore_argb_to_rgba( color4 );

  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_solid_triangle_mode();

  GLVertex2 v1( pt1.x, pt1.y );
  GLVertex2 v2( pt2.x, pt2.y );
  GLVertex2 v3( pt3.x, pt3.y );
  GLVertex2 v4( pt4.x, pt4.y );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  draw_buffer.add( v1, v2, v4, color1, color2, color4 );
  draw_buffer.add( v4, v2, v3, color4, color2, color3 );
}


//=============================================================================
//  Scene3D
//=============================================================================
void Scene3D__native_begin_draw()
{
  // FIXME
#ifdef OLD_OPENGL_V1
  BARD_DISCARD_REF();
  glEnable( GL_TEXTURE_2D );
  glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
    // Since the camera is adjusted so that up is +Y it reverses the necessary
    // culling order of triangles defined with clockwise vertices.
  plasmacore.mode_3d = true;
#endif //OLD_OPENGL_V1
}

void Scene3D__native_end_draw()
{
  // FIXME
#ifdef OLD_OPENGL_V1
  BARD_DISCARD_REF();
  glDisable( GL_TEXTURE_2D );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glDisable(GL_CULL_FACE);
  glVertexPointer( 2, GL_FLOAT, 0, draw_buffer.vertices );
  glTexCoordPointer( 2, GL_FLOAT, 0, draw_buffer.uv);

  plasmacore.mode_3d = false;
  NativeLayer_set_view_transform();
#endif //OLD_OPENGL_V1
}

//=============================================================================
//  Shader
//=============================================================================
void Shader__draw_arrays__Int32_Int32()
{
  int count = BARD_POP_INT32();
  int shape_type = BARD_POP_INT32();
  BARD_DISCARD_REF();  // context

  int draw_type = 0;
  switch (shape_type)
  {
    case BARD_TRIANGLES: draw_type = GL_TRIANGLES; break;
    case BARD_LINES:     draw_type = GL_LINES;  break;
    case BARD_POINTS:    draw_type = GL_POINTS; break;
  }
  glDrawArrays( draw_type, 0, count );
}

void Shader__native_compile__String_String()
{
  BardString* pixel_shader_src = (BardString*) BARD_POP_REF();
  BardString* vertex_shader_src = (BardString*) BARD_POP_REF();
  BardObject* shader_obj = BARD_POP_REF();

  if ( !pixel_shader_src || !vertex_shader_src ) return;

  BARD_GET_NATIVE_DATA( GLShaderInfo*, shader_info, shader_obj, "native_data" );
  if ( !shader_info )
  {
    shader_info = new GLShaderInfo( vertex_shader_src->to_new_ascii(), pixel_shader_src->to_new_ascii() );
    BardObject* native_data = BardNativeData::create( shader_info, 
        BardNativeDataDeleteResource );
    BARD_SET_REF( shader_obj, "native_data", native_data );
  }

  shader_info->compile();
}

void Shader__get_var_handle__Int32_String()
{
  int var_type = BARD_POP_INT32();
  BardString* id_obj = (BardString*) BARD_POP_REF();
  BardObject* shader_obj = BARD_POP_REF();
  BARD_GET_NATIVE_DATA( GLShaderInfo*, shader_info, shader_obj, "native_data" );
  if ( !shader_info || !id_obj)
  {
    BARD_PUSH_INT32( -1 );
    return;
  }

  char name[80];
  id_obj->to_ascii( name, 80 );

  if (var_type == 1)
  {
    BARD_PUSH_INT32( glGetUniformLocation(shader_info->program, name) );
  }
  else
  {
    BARD_PUSH_INT32( glGetAttribLocation(shader_info->program, name) );
  }
}

void Shader__set_var__Int32_Int32_ArrayList_of_Real32_Int32()
{
  int count_per_vertex = BARD_POP_INT32();
  BardArrayList* list = (BardArrayList*) BARD_POP_REF();
  int handle = BARD_POP_INT32();
  int var_type = BARD_POP_INT32();

  BARD_DISCARD_REF(); // context

  if (var_type == 1)
  {
    glUniform1fv( handle, list->count, (GLfloat*) list->array->data );
  }
  else
  {
    glVertexAttribPointer( handle, count_per_vertex, GL_FLOAT, GL_FALSE, 0, list->array->data );
    glEnableVertexAttribArray( handle );
  }
}

void Shader__set_var__Int32_Int32_Int32_Texture()
{
  BardObject* texture_obj = BARD_POP_REF();
  int stage = BARD_POP_INT32();
  int handle = BARD_POP_INT32();
  int var_type = BARD_POP_INT32();

  BARD_DISCARD_REF(); // context
  if (texture_obj == NULL) return;
  GLTexture* texture = NativeLayer_get_native_texture_data( texture_obj );
  if ( !texture ) return;

  if (var_type == 1)
  {
    glBindTexture( GL_TEXTURE_2D, texture->id );
    glActiveTexture( GL_TEXTURE0+stage );
    glUniform1i( handle, stage );

  }
  else
  {
    LOG( "Shader::set(Int32,Int32,Texture) not supported for vertex variables." );
  }

}

void Shader__set_var_colors__Int32_Int32_ArrayList_of_Int32()
{
  BardArrayList* list = (BardArrayList*) BARD_POP_REF();
  int handle = BARD_POP_INT32();
  int var_type = BARD_POP_INT32();

  BARD_DISCARD_REF(); // context

  BardInt32* color_data = (BardInt32*) list->array->data;
  plasmacore_swap_red_and_blue( color_data, list->count  );

  if (var_type == 1)
  {
    glUniform1iv( handle, list->count, (GLint*) list->array->data );
  }
  else
  {
    glVertexAttribPointer( handle, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, color_data );
    glEnableVertexAttribArray( handle );
  }
}

void Shader__configure__Int32_Int32_Int32()
{
  int setting2 = BARD_POP_INT32();
  int setting1 = BARD_POP_INT32();
  int option  = BARD_POP_INT32();

  BardObject* shader_obj = BARD_POP_REF();
  BARD_GET_NATIVE_DATA( GLShaderInfo*, shader_info, shader_obj, "native_data" );
  if ( !shader_info ) return;

  switch (option)
  {
    case 1:  // use program
      glUseProgram( shader_info->program );
      break;

    case 2:  // blending_enabled
      if (setting1 != -1)
      {
        glEnable( GL_BLEND );
        GLenum src_factor, dest_factor;
        switch (setting1)  // src_blend
        {
          case BLEND_ZERO:
            src_factor = GL_ZERO;
            break;
          case BLEND_ONE:
            src_factor = GL_ONE;
            break;
          case BLEND_SRC_ALPHA:
            src_factor = GL_SRC_ALPHA;
            break;
          case BLEND_INVERSE_SRC_ALPHA:
            src_factor = GL_ONE_MINUS_SRC_ALPHA;
            break;
          case BLEND_DEST_ALPHA:
            src_factor = GL_DST_ALPHA;
            break;
          case BLEND_INVERSE_DEST_ALPHA:
            src_factor = GL_ONE_MINUS_DST_ALPHA;
            break;
          case BLEND_DEST_COLOR:
            src_factor = GL_DST_COLOR;
            break;
          case BLEND_INVERSE_DEST_COLOR:
            src_factor = GL_ONE_MINUS_DST_COLOR;
            break;
          case BLEND_OPAQUE:
            src_factor = GL_SRC_ALPHA_SATURATE;
            break;
          default:
            src_factor = GL_SRC_ALPHA;
        }
        switch (setting2)  // dest_blend
        {
          case BLEND_ZERO:
            dest_factor = GL_ZERO;
            break;
          case BLEND_ONE:
            dest_factor = GL_ONE;
            break;
          case BLEND_SRC_ALPHA:
            dest_factor = GL_SRC_ALPHA;
            break;
          case BLEND_INVERSE_SRC_ALPHA:
            dest_factor = GL_ONE_MINUS_SRC_ALPHA;
            break;
          case BLEND_DEST_ALPHA:
            dest_factor = GL_DST_ALPHA;
            break;
          case BLEND_INVERSE_DEST_ALPHA:
            dest_factor = GL_ONE_MINUS_DST_ALPHA;
            break;
          case BLEND_SRC_COLOR:
            dest_factor = GL_SRC_COLOR;
            break;
          case BLEND_INVERSE_SRC_COLOR:
            dest_factor = GL_ONE_MINUS_SRC_COLOR;
            break;
          default:
            dest_factor = GL_ONE_MINUS_SRC_ALPHA;
        }
        glBlendFunc( src_factor, dest_factor );
      }
      else
      {
        glDisable( GL_BLEND );
      }
      break;

    case 3:  // texture
      if (setting1) glEnable( GL_TEXTURE_2D );
      else          glDisable( GL_TEXTURE_2D );
      break;

    case 4:  // texture_wrap
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setting1 ? GL_REPEAT : GL_CLAMP_TO_EDGE );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setting2 ? GL_REPEAT : GL_CLAMP_TO_EDGE );
      break;

    case 5:  // texture filter
      switch (setting1)
      {
        case 1:  // point
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
          break;

        case 2:  // linear
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
          glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
          break;
      }
      break;

    default:
      {
        char buffer[80];
        sprintf( buffer, "Unhandled shader configuration option: %d", option );
        LOG( buffer );
      }
  }
}

//=============================================================================
//  System
//=============================================================================
void System__max_texture_size()
{
  // System.max_texture_size().Vector2
  BARD_DISCARD_REF();  // System singleton

  GLint tex_size; 
  glGetIntegerv( GL_MAX_TEXTURE_SIZE, &tex_size );
  BARD_PUSH_REAL64( tex_size );
  BARD_PUSH_REAL64( tex_size );

  // android is 1024x1024
}


void TriangleManager__fill__Triangle_Color_Color_Color_Render()
{
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32 color3 = BARD_POP_INT32();
  BardInt32 color2 = BARD_POP_INT32();
  BardInt32 color1 = BARD_POP_INT32();
  Vector2 pt1 = BARD_POP(Vector2); 
  Vector2 pt2 = BARD_POP(Vector2); 
  Vector2 pt3 = BARD_POP(Vector2); 

  BARD_DISCARD_REF(); // discard singleton ref 

  //NativeLayer_update_transform();

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );

  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_solid_triangle_mode();

  GLVertex2 v1( pt1.x, pt1.y );
  GLVertex2 v2( pt2.x, pt2.y );
  GLVertex2 v3( pt3.x, pt3.y );

  v1.transform();
  v2.transform();
  v3.transform();

  draw_buffer.add( v1, v2, v3, color1, color2, color3 );
}

void Texture_init( BardInt32* data, int w, int h, int format )
{
  // Helper fn
  BardObject* texture_obj = BARD_POP_REF();

  if (w==0 || h==0) return;

  static int prior_format = 0;

  if (format != prior_format)
  {
    prior_format = format;
    if (format == 1)
    {
      LOG( "Loading 32-bit textures." );
    }
    else
    {
      LOG( "Loading 16-bit textures." );
    }
  }

  if (format == 1)
  {
    // 32-bit

    int wp2 = 1;
    int hp2 = 1;
    int made_copy = 0;
    while (wp2 < w) wp2 <<= 1;
    while (hp2 < h) hp2 <<= 1;

    BardInt32* processed_data;
    if ((wp2 > w || hp2 > h) && data)
    {
      made_copy = 1;
      processed_data = (BardInt32*) new char[ wp2 * hp2 * 4 ];
      int i, j;
      int src_index = 0;
      int dest_index = 0;
      for (j=0; j<h; ++j)
      {
        int color = 0;
        for (i=0; i<w; ++i) 
        {
          processed_data[dest_index++] = color = data[src_index++];
        }
        for (i=w; i<wp2; ++i) processed_data[dest_index++] = color;
      }
      src_index = (h-1)*wp2;
      for (j=h; j<hp2; ++j)
      {
        for (i=0; i<wp2; ++i) processed_data[dest_index++] = processed_data[src_index++];
        src_index -= wp2;
      }

      data = processed_data;
    }

    // this method is also used to assign new data to an existing texture
    GLTexture* texture = NativeLayer_get_native_texture_data( texture_obj );
    if ( !texture )
    {
      texture = new GLTexture(w,h,false);

      //BardLocalRef gc_guard(texture_obj);
      BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );

      BARD_SET_REF( texture_obj, "native_data", data_obj );

      BARD_SET( Vector2, texture_obj, "texture_size", Vector2(wp2,hp2) );
      BARD_SET( Vector2, texture_obj, "image_size", Vector2(w,h) );
    }

    glBindTexture( GL_TEXTURE_2D, texture->id );
    glActiveTexture( GL_TEXTURE0 );

#ifdef SWAP_RED_AND_BLUE
    if (data) plasmacore_swap_red_and_blue( data, wp2*hp2 );
#endif

    // Define the OpenGL texture
    texture->resize( w, h );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, wp2, hp2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

    if (made_copy)
    {
      delete processed_data;
    }
#ifdef SWAP_RED_AND_BLUE
    else
    {
      if (data) plasmacore_swap_red_and_blue( data, wp2*hp2 );
    }
#endif
  }
  else
  {
    // 16-bit
    int wp2 = 1;
    int hp2 = 1;
    while (wp2 < w) wp2 <<= 1;
    while (hp2 < h) hp2 <<= 1;

    BardInt16* processed_data = NULL;
    if (data)
    {
      processed_data = (BardInt16*) new char[ wp2 * hp2 * 2 ];
      int i, j;
      int src_index = 0;
      int dest_index = 0;
      for (j=0; j<h; ++j)
      {
        int color = 0;
        for (i=0; i<w; ++i) 
        {
          color = data[src_index++];
          int a = (color >> 24) & 255;
          int r = (color >> 16) & 255;
          int g = (color >> 8) & 255;
          int b = (color) & 255;
          r >>= 4;
          g >>= 4;
          b >>= 4;
          a >>= 4;
          color = (r<<12) | (g<<8) | (b<<4) | (a);
          processed_data[dest_index++] = color;
        }
        for (i=w; i<wp2; ++i) processed_data[dest_index++] = color;
      }
      src_index = (h-1)*wp2;
      for (j=h; j<hp2; ++j)
      {
        for (i=0; i<wp2; ++i) processed_data[dest_index++] = processed_data[src_index++];
        src_index -= wp2;
      }
    }

    // this method is also used to assign new data to an existing texture
    GLTexture* texture = NativeLayer_get_native_texture_data( texture_obj );
    if ( !texture )
    {
      texture = new GLTexture(w,h,false);

      //BardLocalRef gc_guard(texture_obj);
      BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );

      BARD_SET_REF( texture_obj, "native_data", data_obj );

      BARD_SET( Vector2, texture_obj, "texture_size", Vector2(wp2,hp2) );
      BARD_SET( Vector2, texture_obj, "image_size", Vector2(w,h) );
    }

    glBindTexture( GL_TEXTURE_2D, texture->id );
    glActiveTexture( GL_TEXTURE0 );

    // Define the OpenGL texture
    texture->resize( w, h );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, wp2, hp2, 0, 
        GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4,  processed_data );

    delete processed_data;
  }
}

void Texture__init__Bitmap_Int32()
{
  int format = BARD_POP_INT32();
  BardObject* bitmap_obj = BARD_POP_REF();
  // Leave Texture object on stack.

  BARD_GET_REF( array, bitmap_obj, "data" );
  BardInt32* data = (BardInt32*) (((BardArray*)array)->data);

  BARD_GET_INT32( w, bitmap_obj, "width" );
  BARD_GET_INT32( h, bitmap_obj, "height" );

  Texture_init( data, w, h, format );
}

void Texture__init__Vector2_Int32()
{
  // Texture::init(Vector2,Int32)
  int pixel_format = BARD_POP_INT32();
  int w = (int) BARD_POP_REAL64();
  int h = (int) BARD_POP_REAL64();
  BardObject* texture_obj = BARD_POP_REF();

  if (w==0 || h==0) return;
  int wp2 = 1;
  int hp2 = 1;
  while (wp2 < w) wp2 <<= 1;
  while (hp2 < h) hp2 <<= 1;

  if (pixel_format == 1)
  {
    // 32-bit
    GLTexture* texture = new GLTexture(w,h,false);

    //BardLocalRef gc_guard(texture_obj);
    BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );

    BARD_SET_REF( texture_obj, "native_data", data_obj );

    BARD_SET( Vector2, texture_obj, "texture_size", Vector2(wp2,hp2) );
    BARD_SET( Vector2, texture_obj, "image_size", Vector2(w,h) );

    glBindTexture( GL_TEXTURE_2D, texture->id );
    glActiveTexture( GL_TEXTURE0 );

    // Define the OpenGL texture
    texture->resize( w, h );

    char* buffer = new char[ wp2 * hp2 * 4 ];
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, wp2, hp2, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
    delete buffer;
  }
  else
  {
    // 16-bit
    GLTexture* texture = new GLTexture(w,h,false);

    //BardLocalRef gc_guard(texture_obj);
    BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );

    BARD_SET_REF( texture_obj, "native_data", data_obj );

    BARD_SET( Vector2, texture_obj, "texture_size", Vector2(wp2,hp2) );
    BARD_SET( Vector2, texture_obj, "image_size", Vector2(w,h) );

    glBindTexture( GL_TEXTURE_2D, texture->id );
    glActiveTexture( GL_TEXTURE0 );

    // Define the OpenGL texture
    texture->resize( w, h );

    char* buffer = new char[ wp2 * hp2 * 2 ];
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, wp2, hp2, 0, 
        GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4,  buffer );
    delete buffer;
  }
}

void Texture__init__Vector2()
{
  // Texture::init(Vector2)
  Vector2 size = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  BVM_NULL_CHECK( texture_obj, return );

  int w = (int) size.x;
  int h = (int) size.y;
	if (w==0 || h==0) return;

  GLTexture* texture = new GLTexture(w,h,true);

  //BardLocalRef gc_guard(texture_obj);
  BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );

  BARD_SET_REF( texture_obj, "native_data", data_obj );

  int wp2 = texture->texture_width;
  int hp2 = texture->texture_height;

  BARD_SET( Vector2, texture_obj, "texture_size", Vector2(wp2,hp2) );
  BARD_SET( Vector2, texture_obj, "image_size", Vector2(w,h) );

  glBindTexture( GL_TEXTURE_2D, texture->id );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, wp2, hp2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->id, 0 );
  glEnable( GL_BLEND );

  glDisable( GL_SCISSOR_TEST );
  glClearColor( 0, 0, 0, 0 );
  glClear(GL_COLOR_BUFFER_BIT);
  if (use_scissor) glEnable( GL_SCISSOR_TEST );

  if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
  {
    printf( "Failed to create offscreen buffer.\n" );
    glDeleteFramebuffers( 1, &texture->frame_buffer );
    texture->frame_buffer = 0;
  }
}


void Texture__draw__Corners_Vector2_Color_Render_Blend()
{
  // Texture::draw(Corners,Vector2,Color,Render,Blend)
  BardInt32 src_blend  = BARD_POP_INT32();
  BardInt32 dest_blend = BARD_POP_INT32();
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32    color = BARD_POP_INT32();
  Vector2 size  = BARD_POP(Vector2);
  Vector2 uv_a   = BARD_POP(Vector2);
  Vector2 uv_b   = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, NULL );

  Vector2 pt1( 0, 0 );
  Vector2 pt2( size.x, 0 );
  Vector2 pt3( size.x, size.y );
  Vector2 pt4( 0, size.y );

  //NativeLayer_update_transform();

  color = plasmacore_argb_to_rgba( color );

  GLVertex2 v1( pt1.x, pt1.y );
  GLVertex2 v2( pt2.x, pt2.y );
  GLVertex2 v3( pt3.x, pt3.y );
  GLVertex2 v4( pt4.x, pt4.y );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  GLVertex2 uv1( uv_a.x, uv_a.y );
  GLVertex2 uv2( uv_b.x, uv_a.y );
  GLVertex2 uv3( uv_b.x, uv_b.y );
  GLVertex2 uv4( uv_a.x, uv_b.y );

  draw_buffer.add( v1, v2, v4, color, color, color, uv1, uv2, uv4 );
  draw_buffer.add( v4, v2, v3, color, color, color, uv4, uv2, uv3 );
}

void Texture__draw__Corners_Vector2_Color_Render_Blend_Texture_Corners()
{
  // Texture::draw(Corners,Vector2,Color,Render,Blend,Texture,Corners)
  Vector2 alpha_uv_a = BARD_POP(Vector2);
  Vector2 alpha_uv_b = BARD_POP(Vector2);
  BardObject*  alpha_texture_obj = BARD_POP_REF();
  BardInt32 src_blend  = BARD_POP_INT32();
  BardInt32 dest_blend = BARD_POP_INT32();
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32    color = BARD_POP_INT32();
  Vector2 size  = BARD_POP(Vector2);
  Vector2 uv_a   = BARD_POP(Vector2);
  Vector2 uv_b   = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  GLTexture* alpha_texture = NativeLayer_get_native_texture_data( alpha_texture_obj );
  if ( !alpha_texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, alpha_texture );

  Vector2 pt1( 0, 0 );
  Vector2 pt2( size.x, 0 );
  Vector2 pt3( size.x, size.y );
  Vector2 pt4( 0, size.y );

  //NativeLayer_update_transform();

  color = plasmacore_argb_to_rgba( color );

  GLVertex2 v1( pt1.x, pt1.y );
  GLVertex2 v2( pt2.x, pt2.y );
  GLVertex2 v3( pt3.x, pt3.y );
  GLVertex2 v4( pt4.x, pt4.y );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  GLVertex2 uv1( uv_a.x, uv_a.y );
  GLVertex2 uv2( uv_b.x, uv_a.y );
  GLVertex2 uv3( uv_b.x, uv_b.y );
  GLVertex2 uv4( uv_a.x, uv_b.y );

  GLVertex2 alpha_uv1( alpha_uv_a.x, alpha_uv_a.y );
  GLVertex2 alpha_uv2( alpha_uv_b.x, alpha_uv_a.y );
  GLVertex2 alpha_uv3( alpha_uv_b.x, alpha_uv_b.y );
  GLVertex2 alpha_uv4( alpha_uv_a.x, alpha_uv_b.y );

  draw_buffer.add( v1, v2, v4, color, color, color, uv1, uv2, uv4, alpha_uv1, alpha_uv2, alpha_uv4 );
  draw_buffer.add( v4, v2, v3, color, color, color, uv4, uv2, uv3, alpha_uv4, alpha_uv2, alpha_uv3 );
}

void Texture__draw__Corners_Quad_ColorGradient_Render_Blend()
{
  // Texture::draw(Corners,Quad,ColorGradient,Render,Blend)
  BardInt32 src_blend  = BARD_POP_INT32();
  BardInt32 dest_blend = BARD_POP_INT32();
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32    color1 = BARD_POP_INT32();
  BardInt32    color2 = BARD_POP_INT32();
  BardInt32    color3 = BARD_POP_INT32();
  BardInt32    color4 = BARD_POP_INT32();
  Vector2 pt1    = BARD_POP(Vector2);
  Vector2 pt2    = BARD_POP(Vector2);
  Vector2 pt3    = BARD_POP(Vector2);
  Vector2 pt4    = BARD_POP(Vector2);
  Vector2 uv_a   = BARD_POP(Vector2);
  Vector2 uv_b   = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data( texture_obj );
  if ( !texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, NULL );

  //NativeLayer_update_transform();

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );
  color4 = plasmacore_argb_to_rgba( color4 );

  GLVertex2 v1( pt1.x, pt1.y );
  GLVertex2 v2( pt2.x, pt2.y );
  GLVertex2 v3( pt3.x, pt3.y );
  GLVertex2 v4( pt4.x, pt4.y );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  GLVertex2 uv1( uv_a.x, uv_a.y );
  GLVertex2 uv2( uv_b.x, uv_a.y );
  GLVertex2 uv3( uv_b.x, uv_b.y );
  GLVertex2 uv4( uv_a.x, uv_b.y );

  draw_buffer.add( v1, v2, v4, color1, color2, color4, uv1, uv2, uv4 );
  draw_buffer.add( v4, v2, v3, color4, color2, color3, uv4, uv2, uv3 );
}

void Texture__draw__Vector2_Vector2_Vector2_Triangle_Color_Color_Color_Render_Blend()
{
  // Texture::draw(Vector2,Vector2,Vector2,Triangle,Color,Color,Color,Render,Blend)
  BardInt32 src_blend  = BARD_POP_INT32();
  BardInt32 dest_blend = BARD_POP_INT32();
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32    color3 = BARD_POP_INT32();
  BardInt32    color2 = BARD_POP_INT32();
  BardInt32    color1 = BARD_POP_INT32();
  Vector2 pt1    = BARD_POP(Vector2);
  Vector2 pt2    = BARD_POP(Vector2);
  Vector2 pt3    = BARD_POP(Vector2);
  Vector2 uv_c   = BARD_POP(Vector2);
  Vector2 uv_b   = BARD_POP(Vector2);
  Vector2 uv_a   = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, NULL );

  //NativeLayer_update_transform();

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );

  GLVertex2 v1( pt1.x, pt1.y );
  GLVertex2 v2( pt2.x, pt2.y );
  GLVertex2 v3( pt3.x, pt3.y );

  v1.transform();
  v2.transform();
  v3.transform();

  GLVertex2 uv1( uv_a.x, uv_a.y );
  GLVertex2 uv2( uv_b.x, uv_b.y );
  GLVertex2 uv3( uv_c.x, uv_c.y );

  draw_buffer.add( v1, v2, v3, color1, color2, color3, uv1, uv2, uv3 );
}

void Texture__set__Bitmap_Vector2()
{
  Vector2 pos  = BARD_POP(Vector2);
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  BVM_NULL_CHECK( bitmap_obj, return );
  BardObject* texture_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  glBindTexture( GL_TEXTURE_2D, texture->id );

  int x = (int) pos.x;
  int y = (int) pos.y;
  int w  = bitmap_obj->width;
  int h  = bitmap_obj->height;

  if (x<0 || y<0 || x+w > texture->texture_width || y+h > texture->texture_height)
  {
    return;
  }

#ifdef SWAP_RED_AND_BLUE
  plasmacore_swap_red_and_blue( (BardInt32*) bitmap_obj->pixels->data, w*h );
#endif
  glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, bitmap_obj->pixels->data );
#ifdef SWAP_RED_AND_BLUE
  plasmacore_swap_red_and_blue( (BardInt32*) bitmap_obj->pixels->data, w*h );
#endif
}

void Vector2Manager__draw__Vector2_Color_Render()
{
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32 color  = BARD_POP_INT32();
  Vector2 pt  = BARD_POP(Vector2); 

  BARD_DISCARD_REF(); // discard singleton ref 

  //NativeLayer_update_transform();
  color = plasmacore_argb_to_rgba(color);

  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_point_mode();

  GLVertex2 v(pt.x+1.0/plasmacore.display_width,pt.y-1.0/plasmacore.display_height);
  v.transform();
  draw_buffer.add( v, color );
}

