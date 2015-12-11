#include "win_core.h"
#include <winuser.h>

#include "gd.h"
#include "png.h"

#include "unzip.h"

//=============================================================================
//  WinCore
//=============================================================================

// Prototypes
void configure_filesystem();
void init_display();
void reset_display();
void create_window();
void set_up_directx_device_parameters();
void create_directx();
bool reset_directx();
void set_device_state();
void set_device_state();
bool begin_draw();
void end_draw();
void WinCore_update();


// Globals
const char plasmacore_window_class_name[] = "PlasmacoreWindow";
bool plasmacore_running = false;
bool plasmacore_launched = false;

bool keystate[256];

HINSTANCE h_instance = 0;
HWND      h_wnd = 0;
int       n_cmd_show = 0;

bool device_lost = false;
bool textures_saved = false;
D3DPRESENT_PARAMETERS device_params;
PCoreBox cur_clip;

Archive data_archive("data.zip");
Archive image_archive("images.zip");

int vk_list[] =
{
  // Note: don't use any indices outside 0..255
    8,  9, 12, 13, 19, 20, 27,                    

   32, 33, 34, 35, 36, 37, 38, 39,  40, 44, 45, 46, 
   48, 49, 50, 51, 52, 53, 54, 55,  56, 57, 
       65, 66, 67, 68, 69, 70, 71,  72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87,  88, 89, 90, 91, 92,
   96, 97, 98, 99,100,101,102,103, 104,105,106,107,    109,110,111,
  112,113,114,115,116,117,118,119, 120,121,122,123,124,125,126,127,
  128,129,130,
  144,145,
  160,161,162,163,164,165,
  186, // ;
  187, // =
  188, // ,
  189, // -
  190, // .
  191, // /
  192, // `
  219, // [
  220, // '\'
  221, // ]
  222, // '
  0  // terminating zero
};

int windows_to_plasmacore_keycode_map[256] =
{
    0,  0,  0,  0,  0,  0,  0,  0,   8,  9,  0,  0,297, 13,  0,  0, //0x
    0,  0,  0, 19,301,  0,  0,  0,   0,  0,  0, 27,  0,  0,  0,  0, //1x
   32,280,281,279,278,276,273,275, 274,  0,  0,  0,317,277,127,  0, //2x
   48, 49, 50, 51, 52, 53, 54, 55,  56, 57,  0,  0,  0,  0,  0,  0, //3x
    0, 97, 98, 99,100,101,102,103, 104,105,106,107,108,109,110,111, //4x
  112,113,114,115,116,117,118,119, 120,121,122,310,309,  0,  0,  0, //5x
  256,257,258,259,260,261,262,263, 264,265,268,270,  0,269,266,267, //6x
  282,283,284,285,286,287,288,289, 290,291,292,293,294,295,296,326, //7x
  327,238,329,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, //8x
  300,302,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, //9x
  304,303,306,305,308,307,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, //Ax
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0, 59, 61, 44, 45, 46, 47, //Bx
   96,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, //Cx
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0, 91, 92, 93, 34,  0, //Dx
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0, //Ex
    0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0,  0,  0  //Fx
};

Vector2 TransformedPos( HWND hwnd, WPARAM wparam, LPARAM lparam )
{
  Vector2 pos( lparam & 0xffff, lparam>>16 );
  return plasmacore.update_input_transform().transform( pos );
}


LRESULT CALLBACK plasmacore_window_listener( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
  if ( !plasmacore_launched )
  {
    return DefWindowProc(hwnd, msg, wparam, lparam);
  }

  switch(msg)
  {
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSCHAR:
      // Disable special handling of the ALT key etc.
      return 0;

    case WM_CLOSE:
      DestroyWindow(hwnd);
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_MOUSEMOVE:
      {
        Vector2 pos = TransformedPos( hwnd, wparam, lparam );
        plasmacore_queue_signal( plasmacore.event_mouse_move, 1, 0, false, pos.x, pos.y );
        break;
      }

    case WM_LBUTTONDOWN:
      {
        Vector2 pos = TransformedPos( hwnd, wparam, lparam );
        plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 1, true, pos.x, pos.y );
        break;
      }

    case WM_RBUTTONDOWN:
      {
        Vector2 pos = TransformedPos( hwnd, wparam, lparam );
        plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 2, true, pos.x, pos.y );
        break;
      }

    case WM_LBUTTONUP:
      {
        Vector2 pos = TransformedPos( hwnd, wparam, lparam );
        plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 1, false, pos.x, pos.y );
        break;
      }

    case WM_RBUTTONUP:
      {
        Vector2 pos = TransformedPos( hwnd, wparam, lparam );
        plasmacore_queue_signal( plasmacore.event_mouse_button, 1, 2, false, pos.x, pos.y );
        break;
      }

    case WM_MOUSEWHEEL:
      {
        POINT pt;
        pt.x = (lparam & 0xffff);
        pt.y = (lparam >> 16);
        ScreenToClient( hwnd, &pt );
        Vector2 pos( pt.x, pt.y );
        pos = plasmacore.update_input_transform().transform( pos );

        int scroll_dir = GET_WHEEL_DELTA_WPARAM( wparam );
        if (scroll_dir < 0) scroll_dir =  1;
        else                scroll_dir = -1;
        plasmacore_queue_signal( plasmacore.event_mouse_wheel, 1, scroll_dir, false, pos.x, pos.y );
        break;
      }

    default:
      return DefWindowProc(hwnd, msg, wparam, lparam);
  }
  return 0;
}

void WinCore_init( HINSTANCE _h_instance, int _n_cmd_show )
{
  // Check for memory leaks on exit
  _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

  timeBeginPeriod(1);  // Request 1ms timer resolution

  h_instance = _h_instance;
  n_cmd_show = _n_cmd_show;

  memset( keystate, 0, 256 );

  configure_filesystem();

  // clear log
  FILE* log = fopen( "save/log.txt", "wb" );
  fclose(log);

  // store the command line arguments & path info
  for (int i=1; i<__argc; i++)
  {
    plasmacore.command_line_args.add( __argv[i] );
  }

  init_display();

  plasmacore_init();
}

// patch for deprecated call in gd 2.0.35
/*
extern "C" int png_check_sig( png_byte* sig, int num )
{
  return !png_sig_cmp(sig,0,num);
}
*/

#define plasmacore_argb_to_rgba(color) color

// SDL effs up the floating point mode - this macro restores it to the default (precise) mode
#define RESTORE_FPU { unsigned int ctrl; _controlfp_s( &ctrl, _CW_DEFAULT, MCW_PC ); }

void NativeLayer_alert( const char* mesg );

//kludge
void sound_manager_init();
void sound_manager_shut_down();

//static SDL_Surface *gScreen;
LPDIRECT3D9       direct3d    = NULL;  // the DirectX graphics library 
LPDIRECT3DDEVICE9 d3d_device  = NULL;  // the rendering target
IDirect3DSurface9 *backbuffer = NULL;

void NativeLayer_alert( const char* mesg )
{
  MessageBoxA( 0, mesg, "Fatal Error", MB_ICONEXCLAMATION );
}

void NativeLayer_sleep( int ms )
{
  // Kill time between update cycles for the given number of milliseconds.
  // Add custom tasks as desired.
  Sleep( ms );
}

void LOG( const char* mesg ) 
{
  printf( "%s\n", mesg ); 

  FILE* logfile = fopen( "save/log.txt", "ab" );
  if (logfile)
  {
    fprintf( logfile, "%s\n", mesg ); 
    fclose(logfile); 
  }
}

void LOG( char* mesg, int value ) 
{ 
  printf( mesg, value ); 
  printf( "\n" ); 

  FILE* logfile = fopen("save/log.txt","ab");
  if (logfile)
  {
    fprintf( logfile, mesg, value ); 
    fprintf( logfile, "\n" ); 
    fclose(logfile); 
  }
}

#define PCORE_VERTEX_FVF2D (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX2)
#define PCORE_VERTEX_FVF3D (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

struct DXVertex2D
{
  FLOAT x, y, z;        // The transformed position for the vertex
  DWORD color;          // The vertex color 0xAARRGGBB
  FLOAT tu, tv;         // Texture coordinates 
  FLOAT alpha_tu, alpha_tv; 
    // 2nd pair of tex coords 
    // (used only when rendering with a separate alpha texture)

  DXVertex2D() { }
  DXVertex2D( FLOAT _x, FLOAT _y ) : x(_x), y(_y), z(0.0f) { }
  DXVertex2D( FLOAT _x, FLOAT _y, DWORD _color ) : x(_x), y(_y), z(0.0f), color(_color)  { }
  DXVertex2D( FLOAT _x, FLOAT _y, DWORD _color, FLOAT _tu, FLOAT _tv )
      : x(_x), y(_y), z(0.0f), color(_color), tu(_tu), tv(_tv) { }
  DXVertex2D( FLOAT _x, FLOAT _y, DWORD _color, FLOAT _tu, FLOAT _tv, FLOAT atu, FLOAT atv )
      : x(_x), y(_y), z(0.0f), color(_color), tu(_tu), tv(_tv), alpha_tu(atu), alpha_tv(atv) { }

  void transform()
  {
    // 2D transform
    Matrix4 xform = plasmacore.view_transform;
    float new_x = (float)(xform.r1c1 * x + xform.r1c2 * y + xform.r1c4);
    float new_y = (float)(xform.r2c1 * x + xform.r2c2 * y + xform.r2c4);
    x = new_x;
    y = new_y;
  }
};

struct DXVertex3D
{
  FLOAT x, y, z;      // The transformed position for the vertex
  FLOAT nx, ny, nz;   // Vertex normal
  FLOAT u, v;         // Texture coordinates 

  DXVertex3D() { }
};

struct DXModel : BardResource
{
  LPDIRECT3DVERTEXBUFFER9 vertices;
  LPDIRECT3DINDEXBUFFER9  indices;
  DXVertex3D* vertex_ptr;
  WORD* index_ptr;

  int triangle_count, vertex_count, frame_count;

  DXModel( int _triangle_count, int _vertex_count, int _frame_count )
      : triangle_count(_triangle_count), vertex_count(_vertex_count),
        frame_count(_frame_count)
  {
    // Create vertex buffer
    vertices = NULL;
    d3d_device->CreateVertexBuffer(
        sizeof(DXVertex3D) * vertex_count * frame_count,
        0,
        PCORE_VERTEX_FVF3D,
        D3DPOOL_MANAGED,
        &vertices,
        NULL
      );
    if (vertices == NULL) 
    {
      LOG( "Failed to create vertex buffer." );
      return;
    }

    // Create index buffer
    indices = NULL;
    if (FAILED( d3d_device->CreateIndexBuffer( 
        sizeof(WORD) * triangle_count * 3,
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, 
	       &indices, NULL ) ))
    {
      LOG( "Failed to create index buffer." );
      return;
    }

    lock();
  }

  ~DXModel()
  {
    if (vertices || indices)
    {
      unlock();
      if (vertices)
      {
        vertices->Release();
        vertices = NULL;
      }
      if (indices)
      {
        indices->Release();
        indices = NULL;
      }
    }
  }

  void lock()
  {
    vertices->Lock( 0, 
        sizeof(DXVertex3D) * vertex_count * frame_count, 
        (void**) &vertex_ptr, 0 );

    indices->Lock( 0, 0, (void**) &index_ptr, 0 );
  }

  void unlock()
  {
    indices->Unlock();
    vertices->Unlock();
  }

  int frame( int f ) { return f * triangle_count * 3; }
};

struct DXTexture;
ArrayList<DXTexture*> all_textures;

struct DXTexture : BardResource
{
  LPDIRECT3DTEXTURE9 data;
  LPDIRECT3DSURFACE9 draw_target_surface;
  LPDIRECT3DSURFACE9 draw_target_backup;
  int image_width;
  int image_height;
  int texture_width;
  int texture_height;
  D3DFORMAT  pixel_format;
  bool is_draw_target;
  char* saved_data;

  DXTexture( int w, int h, bool draw_target )
  {
    all_textures.add(this);
    data = NULL;
    draw_target_surface = NULL;
    draw_target_backup = NULL;
    this->is_draw_target = draw_target;
    resize( w, h, draw_target );
  }

  void resize( int w, int h, bool draw_target )
  {
    if (data)
    {
      if (image_width == w  && image_height == h && (draw_target_surface!=0) == draw_target) return;
      release();
    }

    image_width  = w;
    image_height = h;

    if (FAILED(d3d_device->CreateTexture( 
        image_width, 
        image_height,
        1,  // mipmap levels
        draw_target ? D3DUSAGE_RENDERTARGET : 0,  // usage
        D3DFMT_A8R8G8B8,  // pixel format
        draw_target ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED,
        &data,
        NULL
    )))
    {
      return;
    }

    if (draw_target)
    {
      data->GetSurfaceLevel( 0, &draw_target_surface );
    }

    D3DSURFACE_DESC desc;
    data->GetLevelDesc( 0, &desc );
    texture_width = desc.Width;
    texture_height = desc.Height;
    //mm.resource_bytes += texture_width * texture_height * 4;
  }

  ~DXTexture()
  {
    destroy();
  }

  void destroy()
  {
    all_textures.remove_value(this);

    if (draw_target_backup)
    {
      draw_target_backup->Release();
      draw_target_backup = NULL;
    }

    release();
  }

  void release()
  {
    if (draw_target_surface)
    {
      draw_target_surface->Release();
      draw_target_surface = NULL;
    }

    if (data)
    {
      //mm.resource_bytes -= texture_width * texture_height * 4;
      data->Release();
      data = NULL;
    }
  }

  void save_render_targets()
  {
    if ( !is_draw_target || draw_target_backup) return;

    if (SUCCEEDED(d3d_device->CreateOffscreenPlainSurface(
        texture_width, texture_height,
        D3DFMT_A8R8G8B8,  // pixel format
        D3DPOOL_SYSTEMMEM,
        &draw_target_backup,
        NULL )))
    {
      d3d_device->GetRenderTargetData( draw_target_surface, draw_target_backup );
    }
  }

  void release_render_targets()
  {
    if (is_draw_target) release();
  }

  void restore_render_targets()
  {
    if (is_draw_target && !data)
    {
      resize( image_width, image_height, is_draw_target );
      if (draw_target_backup)
      {
        d3d_device->UpdateSurface( draw_target_backup, NULL, draw_target_surface, NULL );
        draw_target_backup->Release();
        draw_target_backup = NULL;
      }
    }
  }
};

DXTexture* NativeLayer_get_native_texture_data( BardObject* texture_obj );

#define MAX_BUFFERED_VERTICES 512*3
#define DRAW_TEXTURED_TRIANGLES 1
#define DRAW_SOLID_TRIANGLES    2
#define DRAW_LINES              3
#define DRAW_POINTS             4

struct DrawBuffer
{
  LPDIRECT3DVERTEXBUFFER9 vertices;

  int draw_mode;
  int count;
  int render_flags;
  int src_blend, dest_blend;
  DXVertex2D* vertex_pos;
  DWORD constant_color;

  DXTexture* texture;
  DXTexture* alpha_src;
  DXTexture* draw_target;

  DrawBuffer()
  {
    vertices = NULL;
    draw_mode = DRAW_TEXTURED_TRIANGLES;
    count = 0;
    vertex_pos = 0;
    texture = NULL;
    draw_target = NULL; 
    render_flags = 0;
    alpha_src = 0;
    src_blend = BLEND_ONE;
    dest_blend = BLEND_INVERSE_SRC_ALPHA;
  }

  ~DrawBuffer()
  {
    release();
  }

  void init()
  {
    release();
    d3d_device->CreateVertexBuffer( 
        sizeof(DXVertex2D) * MAX_BUFFERED_VERTICES,
        0,
        PCORE_VERTEX_FVF2D,
        D3DPOOL_MANAGED,
        &vertices,
        NULL );
    lock();
  }

  void release()
  {
    if (vertices)
    {
      unlock();
      vertices->Release();
      vertices = NULL;
    }
  }

  void lock()
  {
    vertices->Lock( 0, MAX_BUFFERED_VERTICES * sizeof(DXVertex2D), (void**) &vertex_pos, 0 );
  }

  void unlock()
  {
    vertices->Unlock();
  }

  void set_render_flags( int flags, int src_blend, int dest_blend )
  {
    if (render_flags != flags || this->src_blend != src_blend || this->dest_blend != dest_blend) 
    {
      render();
      render_flags = flags;
      this->src_blend = src_blend;
      this->dest_blend = dest_blend;
      if (log_drawing) LOG( "  [changing render flags]" );
    }
  }

  void set_textured_triangle_mode( DXTexture* texture, DXTexture* alpha_src )
  {
    if (draw_mode != DRAW_TEXTURED_TRIANGLES || this->texture != texture || this->alpha_src != alpha_src)
    {
      render();
      if (log_drawing) 
      {
        if (draw_mode != DRAW_TEXTURED_TRIANGLES)
        {
          LOG( "  [changing to textured triangle mode]" );
        }
        else if (this->texture != texture)
        {
          LOG( "  [changing texture]" );
        }
        else
        {
          LOG( "  [changing alpha_mask]" );
        }
      }
      draw_mode = DRAW_TEXTURED_TRIANGLES;
      this->texture = texture;
      this->alpha_src = alpha_src;
    }
  }

  void set_solid_triangle_mode()
  {
    if (draw_mode != DRAW_SOLID_TRIANGLES) render();
    draw_mode = DRAW_SOLID_TRIANGLES;
  }

  void set_line_mode()
  {
    if (draw_mode != DRAW_LINES) render();
    draw_mode = DRAW_LINES;
  }
  
  void set_point_mode()
  {
    if (draw_mode != DRAW_POINTS) render();
    draw_mode = DRAW_POINTS;
  }

  void set_draw_target( DXTexture* target )
  {
    if (draw_target == target) return;

    if (log_drawing) LOG( "  [set_draw_target]" );

    if (draw_target)
    {
      d3d_device->EndScene();
    }

    draw_target = target;

    if (draw_target) 
    {
      d3d_device->SetRenderTarget( 0, draw_target->draw_target_surface );
      d3d_device->BeginScene();
    }
    else 
    {
      d3d_device->SetRenderTarget( 0, backbuffer );
      d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    }
  }
  
  void add( DXVertex2D v1, DXVertex2D v2, DXVertex2D v3 )
  {
    if (count == MAX_BUFFERED_VERTICES) render();

    if ((render_flags & RENDER_FLAG_FIXED_COLOR) && constant_color != v1.color)
    {
      render();
      constant_color = v1.color;
    }

    v1.x -= 0.5f;
    v1.y -= 0.5f;
    v2.x -= 0.5f;
    v2.y -= 0.5f;
    v3.x -= 0.5f;
    v3.y -= 0.5f;
    vertex_pos[0] = v1;
    vertex_pos[1] = v2;
    vertex_pos[2] = v3;
    vertex_pos += 3;
    count += 3;
  }

  void add( DXVertex2D v1, DXVertex2D v2 )
  {
    if (count == MAX_BUFFERED_VERTICES) render();
    vertex_pos[0] = v1;
    vertex_pos[1] = v2;
    vertex_pos += 2;
    count += 2;
  }

  void add( DXVertex2D v )
  {
    if (count == MAX_BUFFERED_VERTICES) render();
    *(vertex_pos++) = v;
    ++count;
  }

  void render()
  {
    if ( !count ) return;

    unlock();

    D3DBLEND src_factor, dest_factor;
    switch (src_blend)
    {
      case BLEND_ZERO:
        src_factor = D3DBLEND_ZERO;
        break;
      case BLEND_ONE:
        src_factor = D3DBLEND_ONE;
        break;
      case BLEND_SRC_ALPHA:
        src_factor = D3DBLEND_SRCALPHA;
        break;
      case BLEND_INVERSE_SRC_ALPHA:
        src_factor = D3DBLEND_INVSRCALPHA;
        break;
      case BLEND_DEST_ALPHA:
        src_factor = D3DBLEND_DESTALPHA;
        break;
      case BLEND_INVERSE_DEST_ALPHA:
        src_factor = D3DBLEND_INVDESTALPHA;
        break;
      case BLEND_DEST_COLOR:
        src_factor = D3DBLEND_DESTCOLOR;
        break;
      case BLEND_INVERSE_DEST_COLOR:
        src_factor = D3DBLEND_INVDESTCOLOR;
        break;
      case BLEND_OPAQUE:
        src_factor = D3DBLEND_SRCALPHASAT;
        break;
      default:
        src_factor = D3DBLEND_ONE;
    }

    switch (dest_blend)
    {
      case BLEND_ZERO:
        dest_factor = D3DBLEND_ZERO;
        break;
      case BLEND_ONE:
        dest_factor = D3DBLEND_ONE;
        break;
      case BLEND_SRC_ALPHA:
        dest_factor = D3DBLEND_SRCALPHA;
        break;
      case BLEND_INVERSE_SRC_ALPHA:
        dest_factor = D3DBLEND_INVSRCALPHA;
        break;
      case BLEND_DEST_ALPHA:
        dest_factor = D3DBLEND_DESTALPHA;
        break;
      case BLEND_INVERSE_DEST_ALPHA:
        dest_factor = D3DBLEND_INVDESTALPHA;
        break;
      case BLEND_SRC_COLOR:
        dest_factor = D3DBLEND_SRCCOLOR;
        break;
      case BLEND_INVERSE_SRC_COLOR:
        dest_factor = D3DBLEND_INVSRCCOLOR;
        break;
      default:
        dest_factor = D3DBLEND_INVSRCALPHA;
    }

    d3d_device->SetRenderState( D3DRS_SRCBLEND,  src_factor );
    d3d_device->SetRenderState( D3DRS_DESTBLEND, dest_factor );

    switch (draw_mode)
    {
      case DRAW_TEXTURED_TRIANGLES:
        // set colorization flags
        if (log_drawing) LOG( "  drawing %d textured triangles", (count/3) );

        d3d_device->SetTexture( 0, texture->data );

        if (render_flags & RENDER_FLAG_FIXED_COLOR)
        {
          d3d_device->SetRenderState( D3DRS_TEXTUREFACTOR, constant_color );
          d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
          d3d_device->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
          d3d_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        }
        else
        {
          d3d_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        }

        // set texture addressing to WRAP or CLAMP
        if (render_flags & RENDER_FLAG_TEXTURE_WRAP)
        {
          d3d_device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
          d3d_device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
        }
        else
        {
          d3d_device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
          d3d_device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        }

        if (render_flags & RENDER_FLAG_POINT_FILTER)
        {
          d3d_device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
          d3d_device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        }
        else
        {
          d3d_device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
          d3d_device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        }

        if (alpha_src) 
        {
          d3d_device->SetTexture( 1, alpha_src->data );

          d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
          d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

          // grab color from first stage
          d3d_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
          d3d_device->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT );

          // use the alpha from this stage
          d3d_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
          d3d_device->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

          d3d_device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 );
        }
        else
        {
          d3d_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE );
          d3d_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
          d3d_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        }


        d3d_device->SetStreamSource( 0, vertices, 0, sizeof(DXVertex2D) );
        d3d_device->SetFVF( PCORE_VERTEX_FVF2D );
        d3d_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, count/3 );   

        d3d_device->SetTexture( 0, NULL );
        d3d_device->SetTexture( 1, NULL );
        break;

      case DRAW_SOLID_TRIANGLES:
        if (log_drawing) LOG( "  drawing %d solid triangles", (count/3) );
        d3d_device->SetStreamSource( 0, vertices, 0, sizeof(DXVertex2D) );
        d3d_device->SetFVF( PCORE_VERTEX_FVF2D );
        d3d_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, count/3 );   
        break;

      case DRAW_LINES:
        if (log_drawing) LOG( "  drawing %d lines", (count/2) );
        d3d_device->SetStreamSource( 0, vertices, 0, sizeof(DXVertex2D) );
        d3d_device->SetFVF( PCORE_VERTEX_FVF2D );
        d3d_device->DrawPrimitive( D3DPT_LINELIST, 0, count/2 );   
        break;

      case DRAW_POINTS:
        if (log_drawing) LOG( "  drawing %d points", (count) );
        d3d_device->SetStreamSource( 0, vertices, 0, sizeof(DXVertex2D) );
        d3d_device->SetFVF( PCORE_VERTEX_FVF2D );
        d3d_device->DrawPrimitive( D3DPT_POINTLIST, 0, count );
        break;
    }

    count = 0;
    lock();
  }
};

DrawBuffer draw_buffer;

void set_clip( double x, double y, double width, double height )
{
  int max_w, max_h;
  if (draw_buffer.draw_target) 
  {
    max_w = draw_buffer.draw_target->image_width;
    max_h = draw_buffer.draw_target->image_height;
  }
  else
  {
    max_w = plasmacore.display_width;
    max_h = plasmacore.display_height;
  }

  if (x < 0) x = 0;
  if (y < 0) y = 0;

  if (x + width > max_w) width = max_w - x;
  if (y + height > max_h) height = max_h - y;

  D3DVIEWPORT9 viewport;
  viewport.X = (int) x;
  viewport.Y = (int) y;
  viewport.Width = (int) width;
  viewport.Height = (int) height;
  
  viewport.MinZ = 0.0f;
  viewport.MaxZ = 1.0f;
  cur_clip = PCoreBox( x, y, width, height );
  d3d_device->SetViewport( &viewport );
}

void set_clip( PCoreBox box )
{
  set_clip( box.top_left.x, box.top_left.y, box.size.x, box.size.y );
}

void clear_clip()
{
  int max_w, max_h;
  if (draw_buffer.draw_target) 
  {
    max_w = draw_buffer.draw_target->image_width;
    max_h = draw_buffer.draw_target->image_height;
  }
  else
  {
    max_w = plasmacore.display_width;
    max_h = plasmacore.display_height;
  }
  set_clip( 0.0, 0.0, max_w, max_h );
}


void WinCore_configure()
{
  RESTORE_FPU

  LOG( "Configuring Plasmacore" );
  plasmacore_configure(1024,768,false,false);  // 1024x768 is just the default

  // Create window
  WNDCLASSEX wc;

  // Register the window class
  wc.cbSize        = sizeof(WNDCLASSEX);
  wc.style         = 0;
  wc.lpfnWndProc   = plasmacore_window_listener;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = h_instance;
  wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = plasmacore_window_class_name;
  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

  if(!RegisterClassEx(&wc))
  {
    bard_throw_fatal_error( "Window registration failed." );
  }

  cur_clip = PCoreBox( 0, 0, plasmacore.display_width, plasmacore.display_height );

  // Create GL context
  LOG( "Resetting display" );
  reset_display();
  RESTORE_FPU

  LOG( "Initializing audio" );
  sound_manager_init();

  LOG( "Initializing Plasmacore" );
  plasmacore_launch();
  plasmacore_launched = true;

  LOG( "Flushing draw buffer" );
  draw_buffer.render();

  plasmacore_update();
}


//====================================================================


void configure_filesystem()
{
  // Step 1: Get the path+filename that we were executed with.  
  // Remove commandline args and quotes.  
  char* original_pathed_filename = new char[ strlen(GetCommandLine()) + 4 ];
  StrCpy( original_pathed_filename, GetCommandLine() );
  PathRemoveArgs( original_pathed_filename );
  PathUnquoteSpaces( original_pathed_filename );

  // Step 2: Get just the filename (no path or extension)
  char* program_name = new char[MAX_PATH+4];
  if (GetLongPathName(original_pathed_filename, program_name, MAX_PATH+4) == 0)
  {
    // GetLongPathName failed; use the name as given and hope for the best.
    strncpy_s( program_name, MAX_PATH+4, original_pathed_filename, MAX_PATH+4 );
  }
  PathStripPath(program_name);
  int i = strlen(program_name) - 1;
  while (i > 0 && program_name[i]==' ') program_name[i--] = 0;
  PathRemoveExtension(program_name);

  // Step 3: Get just the path
  char* launch_path = new char[ lstrlen(original_pathed_filename) + 4 ];
  StrCpy( launch_path, original_pathed_filename );

  for (i=0; launch_path[i]; ++i)
  {
    if (launch_path[i] == '/') launch_path[i] = '\\';
  }

  PathRemoveFileSpec( launch_path );
  if (strlen(launch_path) > 0 && launch_path[strlen(launch_path) - 1] != '\\') 
  {
    StrCat(launch_path, "\\");
  }

  // Step 4: Enter the directory the exe is in
#if !defined(PLASMACORE_NO_CHDIR)
  _chdir( launch_path );
#endif

  // Ensure a save\ directory exists.
  _mkdir( "save" );

  delete original_pathed_filename;
  delete program_name;
  delete launch_path;
}

void init_display()
{
  // Create the Direct3D object
  direct3d = Direct3DCreate9( D3D_SDK_VERSION );
  if ( !direct3d ) NativeLayer_alert( "Failed to create Direct3D object." );
}

void reset_display()
{
  create_window();
  if (d3d_device) 
  {
    reset_directx();
  }
  else create_directx();
}

void create_window()
{
  // Determine the correct window size to have a client area of the
  // desired size.
  RECT bounds;
  bounds.left   = 0;
  bounds.top    = 0;
  bounds.right  = plasmacore.display_width;
  bounds.bottom = plasmacore.display_height;
  int flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX;
  AdjustWindowRectEx( &bounds, flags, false, WS_EX_CLIENTEDGE );

  // Create the Window
  h_wnd = CreateWindowEx(
      WS_EX_CLIENTEDGE,
      plasmacore_window_class_name,
      "Plasmacore",
      flags,
      CW_USEDEFAULT, CW_USEDEFAULT, bounds.right-bounds.left, bounds.bottom-bounds.top,
      NULL, NULL, h_instance, NULL);

  if(h_wnd == NULL)
  {
    bard_throw_fatal_error( "Window creation failed." );
  }

  ShowWindow(h_wnd, n_cmd_show );
  UpdateWindow(h_wnd);

  /*
  int sdl_flags = (plasmacore.is_fullscreen ? SDL_FULLSCREEN : 0);

#define ICON_SIZE 32
  HDC hdc = CreateCompatibleDC(GetDC(NULL));
  
  HICON icon = (HICON) LoadImage( GetModuleHandle(NULL), MAKEINTRESOURCE(107), IMAGE_ICON, ICON_SIZE, ICON_SIZE, 0 );
  HBITMAP hbm = CreateCompatibleBitmap(GetDC(NULL), ICON_SIZE, ICON_SIZE);
  HBITMAP oldbm = (HBITMAP)SelectObject(hdc, hbm);

  BITMAPINFOHEADER bmi = {0};
  bmi.biSize = sizeof(bmi);
  bmi.biWidth = ICON_SIZE;
  bmi.biHeight = -ICON_SIZE;
  bmi.biPlanes = 1;
  bmi.biBitCount = ICON_SIZE;
  bmi.biCompression = BI_RGB;
  int* dib = new int[ICON_SIZE * ICON_SIZE];
  DrawIcon(hdc, 0, 0, icon);
  
  GetDIBits(hdc, hbm, 0, ICON_SIZE, dib, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);

  int bmask = 0x000000ff;
  int gmask = 0x0000ff00;
  int rmask = 0x00ff0000;
  int amask = 0xff000000;

  SDL_Surface * surf = SDL_CreateRGBSurfaceFrom(dib, ICON_SIZE, ICON_SIZE, ICON_SIZE, ICON_SIZE * 4, rmask, gmask, bmask, amask);

  SDL_WM_SetIcon(surf, NULL);

  SDL_FreeSurface(surf);

  delete[] dib;

  SelectObject(hdc, oldbm);
  DeleteObject(hbm);
  DestroyIcon(icon);
  DeleteDC(hdc);

  // Create window
  gScreen = SDL_SetVideoMode ( plasmacore.display_width, plasmacore.display_height, 0, sdl_flags);
  if (gScreen == NULL) {

    fprintf (stderr, "Couldn't set %dx%d OpenGL video mode: %s\n",
         plasmacore.display_width, plasmacore.display_height, SDL_GetError() );
    SDL_Quit();
    exit(2);
  }

  SDL_WM_SetCaption( "Plasmacore", "Plasmacore" );
  */
}

void set_up_directx_device_parameters()
{
  // Get the current desktop display mode, so we can set up a back
  // buffer of the same format
  D3DDISPLAYMODE display_mode;
  HRESULT r;
  r = direct3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display_mode );

  //if (plasmacore.borderless_window) ...

  ZeroMemory( &device_params, sizeof(device_params) );
  device_params.Windowed = !plasmacore.is_fullscreen;

  // For fullscreen, use DISCARD, which can lead to hosed content but is fast.
  // For windowed, do the pixel copy on vsync to reduce tearing.
  device_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
  device_params.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

  //device_params.EnableAutoDepthStencil = TRUE;
  //device_params.AutoDepthStencilFormat = D3DFMT_D24S8;
  device_params.BackBufferCount = 1;
  device_params.hDeviceWindow = h_wnd;

  device_params.BackBufferWidth = plasmacore.display_width;
  device_params.BackBufferHeight = plasmacore.display_height;

  if (plasmacore.is_fullscreen)
  {
    device_params.FullScreen_RefreshRateInHz = 60;
    device_params.BackBufferFormat = display_mode.Format;
  }
  else
  {
    //windowed
    device_params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    device_params.BackBufferFormat = D3DFMT_UNKNOWN;
  }

  //if ( !plasmacore.is_fullscreen )
  //{
    //device_params.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  //}

  //plasmacore.lockable_backbuffer = device_params.Flags & D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
}

void create_directx()
{
  set_up_directx_device_parameters();

  //----DirectX-------------------------------------------------------

  // Create the D3DDevice
  HRESULT result = direct3d->CreateDevice( 
      D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, h_wnd,
      D3DCREATE_MIXED_VERTEXPROCESSING,
      &device_params, &d3d_device );

  if (FAILED(result) && plasmacore.is_fullscreen)
  {
    // tried to set to 60 hz; use adapter default instead
    // and try again
    set_up_directx_device_parameters();
    device_params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    
    result = direct3d->CreateDevice( 
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, h_wnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &device_params, &d3d_device );

    if (FAILED(result))
    {
      bard_throw_fatal_error( "Failed to create Direct3D device." );
    }
  }

  d3d_device->GetRenderTarget( 0, &backbuffer );

  set_device_state();
  draw_buffer.init();
}

bool reset_directx()
{
  device_lost = true;

  for (int i=0; i<all_textures.count; ++i)
  {
    DXTexture* img = all_textures[i];
    img->release_render_targets();
  }

  if (backbuffer)
  {
    backbuffer->Release();
    backbuffer = NULL;
  }

  set_up_directx_device_parameters();
  HRESULT result = d3d_device->Reset( &device_params );

  if ( FAILED(result) )
  {
    if ( !plasmacore.is_fullscreen ) return false;

    // tried to set to 60 hz; use adapter default instead
    // and try again
    set_up_directx_device_parameters();
    device_params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    if ( FAILED(d3d_device->Reset( &device_params )) ) return false;
  }

  for (int i=0; i<all_textures.count; ++i)
  {
    DXTexture* img = all_textures[i];
    img->restore_render_targets();
  }

  device_lost = false;
  set_device_state();

  d3d_device->GetRenderTarget( 0, &backbuffer );

  if ( !textures_saved )
  {
    plasmacore_queue_signal( plasmacore.event_textures_lost );
    plasmacore_raise_pending_signals();
  }
  textures_saved = false;

  return true;
}

void set_device_state()
{
  d3d_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE ); 

  //Turn on Alpha blending (get some transparencies goin on)
  d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE,  true);

  d3d_device->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
  d3d_device->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
  d3d_device->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
  d3d_device->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

  // Turn off D3D lighting
  d3d_device->SetRenderState( D3DRS_LIGHTING, false );

  // Turn off the zbuffer
  d3d_device->SetRenderState( D3DRS_ZENABLE, false );

  d3d_device->Clear(0, 0, D3DCLEAR_TARGET, 0xff000000, 0, 0);
}

void NativeLayer_shut_down()
{
  timeEndPeriod(1); // No longer care about 1 ms timer resolution.

  sound_manager_shut_down();

  if (backbuffer)
  {
    backbuffer->Release();
    backbuffer = NULL;
  }

  if (d3d_device)
  {
    d3d_device->Release();
    d3d_device = NULL;
  }

  if (direct3d)
  {
    direct3d->Release();
    direct3d = NULL;
  }
}

void NativeLayer_begin_draw()
{
  // Clear the screen to Display.background.color unless that color's alpha=0.
  BARD_FIND_TYPE( type_display, "Display" );
  BARD_GET_INT32( bg_color, type_display->singleton(), "background_color" );
  int alpha = (bg_color >> 24) & 255;
  if (alpha) d3d_device->Clear(0, 0, D3DCLEAR_TARGET, bg_color, 0, 0);
}

void NativeLayer_end_draw() { }

bool begin_draw()
{
  if (log_drawing) LOG( "+draw()" );

  if (device_lost)
  {
    HRESULT result = d3d_device->TestCooperativeLevel();
    if (result == D3DERR_DEVICELOST) 
    {
	  Sleep( 500 );    
      return false;
    }

    if (result != D3D_OK)
    {
      reset_directx();
      if (device_lost) return false;
    }
    device_lost = false;
    set_device_state();
  }

  draw_buffer.set_draw_target( NULL );
  d3d_device->BeginScene();

  return true;
}

void end_draw()
{
  draw_buffer.render();
  d3d_device->EndScene();
  if (FAILED(d3d_device->Present(NULL, NULL, NULL, NULL)))
  {
    device_lost = true;
  }
}


void WinCore_update()
{
  RESTORE_FPU

  // Update keystate
  int* index_ptr = vk_list - 1;
  int index = *(++index_ptr);
  while (index)
  {
    bool state = (GetAsyncKeyState(index) & 0xf000) != 0;
    if (state != keystate[index])
    {
      int keycode = windows_to_plasmacore_keycode_map[index];
      keystate[index] = state;
      plasmacore_queue_signal( plasmacore.event_key, 0, keycode, state );
    }
    index = *(++index_ptr);
  }

  /*
  index = 0;
  while (index<256)
  {
    bool state = (GetAsyncKeyState(index) & 0xf000) != 0;
    if (state)
    {
      char buffer[80];
      sprintf(buffer,"VK:%d\n",index);
      LOG(buffer);
    }
    ++index;
  }
  */

  if (plasmacore_update() && begin_draw())
  {
    plasmacore_draw();
    end_draw();
  }
}

int WinCore_main_loop()
{
  // Draw, get events...
  LOG( "Starting main loop" );

  int ms_error = 0;
  BardInt64 next_time = bard_get_time_ms() + (1000/plasmacore.target_fps);

  UpdateCycleRegulator regulator;
  MSG Msg;
  plasmacore_running = true;
  while (plasmacore_running)
  {
    while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
    {
      if (Msg.message == WM_QUIT)
      {
        plasmacore_running = false;
        break;
      }
      else
      {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
      }
    }
    WinCore_update();
    Sleep( regulator.update() );
  }
  return 0;  //Msg.wparam;

  /*
  plasmacore_running = true;
  while ( plasmacore_running ) 
  {
    // Check for events 
    SDL_Event event;
    while ( SDL_PollEvent (&event) ) 
    {
      RESTORE_FPU
      switch (event.type)
      {
        case SDL_QUIT:
          plasmacore_on_exit_request();
          break;

        case SDL_VIDEORESIZE:
          reset_display();
          break;

        case SDL_KEYDOWN:
          plasmacore_queue_data_event( plasmacore.event_key, 0, event.key.keysym.sym, true );
          break;

        case SDL_KEYUP:
          plasmacore_queue_data_event( plasmacore.event_key, 0, event.key.keysym.sym, false );
          break;

        case SDL_MOUSEMOTION:
          {
            double x = event.motion.xrel / plasmacore.scale_factor;
            double y = event.motion.yrel / plasmacore.scale_factor;
            plasmacore_queue_data_event( plasmacore.event_mouse_move, 
              1, 0, false, x, y  );

            x = (event.motion.x - plasmacore.border_x) / plasmacore.scale_factor;
            y = (event.motion.y - plasmacore.border_y) / plasmacore.scale_factor;
            plasmacore_queue_data_event( plasmacore.event_mouse_move, 
              1, 0, true, x, y  );
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          switch (event.button.button)
          {
            case SDL_BUTTON_WHEELUP:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_wheel, 1, -1, false, x, y );
              }
              break;

            case SDL_BUTTON_WHEELDOWN:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_wheel, 1, 1, false, x, y );
              }
              break;

            case SDL_BUTTON_LEFT:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button, 1, 1, true, x, y );
              }
              break;

            case SDL_BUTTON_RIGHT:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button, 1, 2, true, x, y );
              }
              break;

            case SDL_BUTTON_MIDDLE:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button, 1, 3, true, x, y );
              }
              break;
          }
          break;

        case SDL_MOUSEBUTTONUP:
          switch (event.button.button)
          {
            case SDL_BUTTON_LEFT:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button, 1, 1, false, x, y );
              }
              break;

            case SDL_BUTTON_RIGHT:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button, 1, 2, false, x, y );
              }
              break;

            case SDL_BUTTON_MIDDLE:
              {
                double x = (event.button.x - plasmacore.border_x) / plasmacore.scale_factor;
                double y = (event.button.y - plasmacore.border_y) / plasmacore.scale_factor;
                plasmacore_queue_data_event( plasmacore.event_mouse_button, 1, 3, false, x, y );
              }
              break;
          }
          break;

      }
    }

    WinCore_update();

    BardInt64 cur_time = bard_get_time_ms();

    int kill_ms = (int) (next_time - cur_time);
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

    NativeLayer_sleep( kill_ms );
  }
  */
}

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

gdImagePtr NativeLayer_decode_image( char* raw_data, int data_size )
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

  return img;
}

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
    BardInt32* dest = ((BardInt32*) ((BardArray*)array)->data) - 1;
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


//=============================================================================

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

void Application__title__String()
{
  // Application::title(String) 
  BardString* mesg = (BardString*) BARD_POP_REF();  // title string 
  BARD_POP_REF();  // discard singleton

  if ( !mesg ) return;

  char buffer[100];
  ((BardString*)mesg)->to_ascii( buffer, 100 );
  //SDL_WM_SetCaption( buffer, buffer );
  //TODO
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

void Display__flush()
{
  draw_buffer.render();
}

void Display__fullscreen()
{
  // Display::fullscreen().Logical 
  BARD_DISCARD_REF();
  BARD_PUSH_INT32( plasmacore.is_fullscreen );
}

void Display__fullscreen__Logical()
{
  // Display::fullscreen(Logical) 
  bool setting = 0 < BARD_POP_INT32();
  BARD_POP_REF(); // singleton ref

  if (setting != plasmacore.is_fullscreen)
  {
    for (int i=0; i<all_textures.count; ++i)
    {
      DXTexture* img = all_textures[i];
      img->save_render_targets();
    }
    textures_saved = true;

    plasmacore.is_fullscreen = setting;
    reset_directx();
    //reset_display();
    create_window();
  }
}

void Display__native_set_clipping_region__Box()
{
  // Display::native_set_clipping_region(Box)
  draw_buffer.render();

  Vector2 top_left = BARD_POP(Vector2);
  Vector2 size = BARD_POP(Vector2);
  BARD_POP_REF();  // discard singleton
  Vector2 bottom_right( top_left.x+size.x, top_left.y+size.y );

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    top_left = Matrix2x3_transform( plasmacore.transform, top_left );
    bottom_right = Matrix2x3_transform( plasmacore.transform, bottom_right );
  }
  */

  double x1 = top_left.x;
  if (bottom_right.x < x1) x1 = bottom_right.x;
  double y1 = top_left.y;
  if (bottom_right.y < y1) y1 = bottom_right.y;

  double x2 = top_left.x;
  if (bottom_right.x > x2) x2 = bottom_right.x;
  double y2 = top_left.y;
  if (bottom_right.y > y2) y2 = bottom_right.y;

  DXVertex2D v1((float)x1,(float)y1);
  DXVertex2D v2((float)x2,(float)y2);
  v1.transform();
  v2.transform();

  set_clip( v1.x, v1.y, v2.x-v1.x, v2.y-v1.y );
}

void Display__native_set_draw_target__OffscreenBuffer_Logical()
{
  // Display::native_set_draw_target(OffscreenBuffer,Logical)
  BardInt32 blend = BARD_POP_INT32();
  BardObject* image_obj = BARD_POP_REF();
  BARD_POP_REF();  // discard singleton

  draw_buffer.render();

  if ( !image_obj )
  {
    draw_buffer.set_draw_target( NULL );
    draw_buffer.render();
    return;
  }

  BARD_GET_REF( texture_obj, image_obj, "texture" );

  BVM_NULL_CHECK( texture_obj, return ); 

  DXTexture* draw_target = NativeLayer_get_native_texture_data(texture_obj);
  draw_buffer.set_draw_target( draw_target );

  d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, blend );
}

void Display__screen_shot__Bitmap()
{
  // Display::screen_shot(Bitmap=null).Bitmap
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  BARD_DISCARD_REF();

  draw_buffer.render();

  if ( !backbuffer )
  {
    BARD_PUSH_REF(NULL);
    return;
  }

  // Get width & height
  D3DSURFACE_DESC desc;
  backbuffer->GetDesc( &desc );
  int w = desc.Width;
  int h = desc.Height;

  // Create or resize bitmap
  BARD_FIND_TYPE( type_bitmap, "Bitmap" );
  if ( !bitmap_obj )
  {
    bitmap_obj = (BardBitmap*) ( type_bitmap->create() );
  }
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // method result
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // for init(Int32,Int32)
  BARD_PUSH_INT32( w );
  BARD_PUSH_INT32( h );
  BARD_CALL( type_bitmap, "init(Int32,Int32)" );  // only reallocates if wrong size
  bitmap_obj = (BardBitmap*) BARD_PEEK_REF();  // reset ref in case a gc happened

  if (desc.Format != D3DFMT_A8R8G8B8 && desc.Format != D3DFMT_X8R8G8B8) return;

  IDirect3DSurface9 *temp_surface = NULL;
  if (SUCCEEDED(d3d_device->CreateOffscreenPlainSurface(
      w, h,
      desc.Format, //D3DFMT_A8R8G8B8,  // pixel format
      D3DPOOL_SYSTEMMEM,
      &temp_surface,
      NULL )))
  {
    HRESULT result = d3d_device->GetRenderTargetData( backbuffer, temp_surface );

    // Lock rect & copy data
    D3DLOCKED_RECT lock_rect;
    if (D3D_OK != temp_surface->LockRect( &lock_rect, NULL, D3DLOCK_READONLY )) return;
    memcpy( bitmap_obj->pixels->data, lock_rect.pBits, w*h*4 );
    temp_surface->UnlockRect();

    temp_surface->Release();
  }
}

void Input__mouse_position__Vector2()
{
  Vector2 new_pos = BARD_POP( Vector2 );
  BARD_DISCARD_REF();
  POINT pt;
  pt.x = (int) new_pos.x;
  pt.y = (int) new_pos.y;
  ClientToScreen( h_wnd, &pt );
  SetCursorPos( pt.x, pt.y );
}

void Input__mouse_visible__Logical()
{
  bool setting = 0 < BARD_POP_INT32();
  BARD_POP_REF();      // discard singleton

  if (setting == mouse_visible) return;
  mouse_visible = setting;

  //TODO
  //if (setting) SDL_ShowCursor( SDL_ENABLE );
  //else         SDL_ShowCursor( SDL_DISABLE );
}

void Input__input_capture__Logical()
{
  BARD_POP_INT32(); // ignore setting
  BARD_POP_REF();      // discard singleton
}

void LineManager__draw__Line_Color_Render()
{
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32 color  = BARD_POP_INT32();
  Vector2 pt1 = BARD_POP(Vector2); 
  Vector2 pt2 = BARD_POP(Vector2); 

  BARD_POP_REF(); // discard singleton ref 

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt1 = Matrix2x3_transform( plasmacore.transform, pt1 );
    pt2 = Matrix2x3_transform( plasmacore.transform, pt2 );
  }
  */

  color = plasmacore_argb_to_rgba( color );
  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_line_mode();
  DXVertex2D v1(FLOAT(pt1.x+1),FLOAT(pt1.y),color);
  DXVertex2D v2((FLOAT)(pt2.x+1),FLOAT(pt2.y),color);
  v1.transform();
  v2.transform();
  draw_buffer.add( v1, v2 );
}

//=============================================================================
//  Model
//=============================================================================
struct BardVertex : BardObject
{
  float x, y, z, u, v;
};

void Model__native_init__Int32_Int32_Int32()
{
  int frame_count    = BARD_POP_INT32();
  int vertex_count   = BARD_POP_INT32();
  int triangle_count = BARD_POP_INT32();
  BardObject* model_obj = BARD_POP_REF();

  DXModel* model = new DXModel( triangle_count, vertex_count, frame_count );
  BardObject* native_data = BardNativeData::create( model, BardNativeDataDeleteResource );
  BARD_SET_REF( model_obj, "native_data", native_data );
}

void Model__define_triangles__Array_of_Char()
{
  BardArray* array_obj = (BardArray*) BARD_POP_REF();
  BardObject* model_obj = BARD_POP_REF();
  BARD_GET_NATIVE_DATA( DXModel*, model, model_obj, "native_data" );
  memcpy( model->index_ptr, array_obj->data, model->triangle_count * 3 * 2 );
}

void Model__define_frame__Int32_Array_of_Vertex()
{
  BardArray* array_obj = (BardArray*) BARD_POP_REF();
  int frame = BARD_POP_INT32();
  BardObject* model_obj = BARD_POP_REF();

  BARD_GET_NATIVE_DATA( DXModel*, model, model_obj, "native_data" );
  if (model && frame < model->frame_count)
  {
    BardVertex** src = ((BardVertex**)(array_obj->data)) - 1;
    DXVertex3D* dest = (model->vertex_ptr + model->frame(frame));
    int count = model->vertex_count + 1;
    while (--count)
    {
      BardVertex* v = *(++src);
      dest->x = (FLOAT) v->x;
      dest->y = (FLOAT) v->y;
      dest->z = (FLOAT) v->z;
      dest->u = (FLOAT) v->u;
      dest->v = (FLOAT) v->v;
      ++dest;
    }
  }
}

void Model__native_draw__Texture()
{
  BardObject* texture_obj = BARD_POP_REF();
  BardObject* model_obj = BARD_POP_REF();

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  BARD_GET_NATIVE_DATA( DXModel*, model, model_obj, "native_data" );
  if ( !model ) return;
  model->unlock();

  /*
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

  glDisable(GL_LIGHTING);

  NativeLayer_set_view_transform();
  //glFrustum(-1,1,-1,1,1,10);

  glColor4f(1.0f,1.0f,1.0f,1.0f);
  glDisableClientState(GL_COLOR_ARRAY);
  glVertexPointer( 3, GL_FLOAT, 0, model->frames[0] );
  //glEnableClientState( GL_VERTEX_ARRAY );
  //glVertexPointer( 3, GL_FLOAT, 0, vertices );

  glTexCoordPointer( 2, GL_FLOAT, 0, model->uv );
  //glTexCoordPointer( 2, GL_FLOAT, 0, uv);
  //glColorPointer( 4, GL_UNSIGNED_BYTE, 0, draw_buffer.colors);
  */

  d3d_device->SetStreamSource( 0, model->vertices, 0, sizeof(DXVertex3D) );
  d3d_device->SetIndices( model->indices );
  d3d_device->SetFVF( PCORE_VERTEX_FVF3D );
  d3d_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 3, 0, 1 );   
  //glDrawElements( GL_TRIANGLES, model->triangle_count*3, GL_UNSIGNED_SHORT, model->triangles );

  //glVertexPointer( 2, GL_FLOAT, 0, draw_buffer.vertices );
  //glEnableClientState( GL_COLOR_ARRAY );
}


void OffscreenBuffer__clear__Color()
{
  draw_buffer.render();

  BardInt32 color = BARD_POP_INT32();
  BardObject* buffer_obj = BARD_POP_REF();

  BVM_NULL_CHECK( buffer_obj, return );

  BARD_GET( BardObject*, texture_obj, buffer_obj, "texture" );
  BVM_NULL_CHECK( texture_obj, return );

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture || !texture->draw_target_surface ) return;

  d3d_device->SetRenderTarget( 0, texture->draw_target_surface );
  d3d_device->Clear(0, 0, D3DCLEAR_TARGET, color, 0, 0);

  if (draw_buffer.draw_target)
  {
    d3d_device->SetRenderTarget( 0, draw_buffer.draw_target->draw_target_surface );
  }
  else
  {
    d3d_device->SetRenderTarget( 0, backbuffer );
  }
  set_clip( cur_clip );  // restore the clipping region
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

  BARD_POP_REF(); // discard singleton ref 

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt1 = Matrix2x3_transform( plasmacore.transform, pt1 );
    pt2 = Matrix2x3_transform( plasmacore.transform, pt2 );
    pt3 = Matrix2x3_transform( plasmacore.transform, pt3 );
    pt4 = Matrix2x3_transform( plasmacore.transform, pt4 );
  }
  */

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );
  color4 = plasmacore_argb_to_rgba( color4 );

  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_solid_triangle_mode();

  DXVertex2D v1( FLOAT(pt1.x), FLOAT(pt1.y), color1 );
  DXVertex2D v2( FLOAT(pt2.x), FLOAT(pt2.y), color2 );
  DXVertex2D v3( FLOAT(pt3.x), FLOAT(pt3.y), color3 );
  DXVertex2D v4( FLOAT(pt4.x), FLOAT(pt4.y), color4 );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  draw_buffer.add( v1, v2, v4 );
  draw_buffer.add( v4, v2, v3 );
}

//=============================================================================
//  Scene3D
//=============================================================================
void Scene3D__native_begin_draw()
{
  BARD_DISCARD_REF();
  //glEnable( GL_TEXTURE_2D );
  //glEnableClientState( GL_TEXTURE_COORD_ARRAY );
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);
    // Since the camera is adjusted so that up is +Y it reverses the necessary
    // culling order of triangles defined with clockwise vertices.
  plasmacore.mode_3d = true;
}

void Scene3D__native_end_draw()
{
  BARD_DISCARD_REF();
  //glDisable( GL_TEXTURE_2D );
  //glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  //glDisable(GL_CULL_FACE);
  plasmacore.mode_3d = false;
}

void System__country_name()
{
  BARD_POP_REF();  // singleton ptr

	char buffer[256];
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SCOUNTRY, buffer, 256 );
  BARD_PUSH_REF( BardString::create(buffer) );
}


void System__max_texture_size()
{
  // System.max_texture_size().Vector2
  BARD_POP_REF(); // System singleton

  D3DCAPS9 caps;
  d3d_device->GetDeviceCaps( &caps );
  BARD_PUSH_REAL64( caps.MaxTextureHeight );
  BARD_PUSH_REAL64( caps.MaxTextureWidth );
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

  BARD_POP_REF(); // discard singleton ref 

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt1 = Matrix2x3_transform( plasmacore.transform, pt1 );
    pt2 = Matrix2x3_transform( plasmacore.transform, pt2 );
    pt3 = Matrix2x3_transform( plasmacore.transform, pt3 );
  }
  */

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );

  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_solid_triangle_mode();

  DXVertex2D v1( FLOAT(pt1.x), FLOAT(pt1.y), color1 );
  DXVertex2D v2( FLOAT(pt2.x), FLOAT(pt2.y), color2 );
  DXVertex2D v3( FLOAT(pt3.x), FLOAT(pt3.y), color3 );

  v1.transform();
  v2.transform();
  v3.transform();

  draw_buffer.add( v1, v2, v3 );
}


void Vector2Manager__draw__Vector2_Color_Render()
{
  BardInt32 render_flags = BARD_POP_INT32();
  BardInt32 color  = BARD_POP_INT32();
  Vector2 pt  = BARD_POP(Vector2); 

  BARD_POP_REF(); // discard singleton ref 

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt = Matrix2x3_transform( plasmacore.transform, pt );
  }
  */

  draw_buffer.set_render_flags( render_flags, BLEND_SRC_ALPHA, BLEND_INVERSE_SRC_ALPHA );
  draw_buffer.set_point_mode();
  DXVertex2D v(FLOAT(pt.x+1.0/320.0), FLOAT(pt.y-1.0/480.0), plasmacore_argb_to_rgba(color));
  v.transform();
  draw_buffer.add(v);
}

void SoundManager__audio_supported()
{
  BARD_DISCARD_REF();
  BARD_PUSH_LOGICAL( true );
}

DXTexture* NativeLayer_get_native_texture_data( BardObject* texture_obj )
{
  BARD_GET_REF( native_data, texture_obj, "native_data" );
  if ( !native_data ) return NULL;

  return (DXTexture*) (((BardNativeData*)native_data)->data);
}

void NativeLayer_set_projection_transform()
{
  D3DMATRIX transform;
  plasmacore.projection_transform.to_floats( (float*) &transform );

  /*
  float r = 1024;
  float l = 0;
  float t = 0;
  float b = 768;
  float zf = 1;
  float zn = 0;
  float tx = (l+r)/(l-r);
  float ty = (t+b)/(b-t);
  float tz = zn/(zn-zf);
  float m[] =
  {
    2/(r-l), 0      , 0         , 0,
    0      , 2/(t-b), 0         , 0,
    0      , 0      , 1/(zn-zf) , 0,
    tx     , ty     , tz        , 1
  };
  */

  d3d_device->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*) &transform );
}

void NativeLayer_set_view_transform()
{
  if ( !plasmacore.mode_3d ) return;

  //D3DMATRIX transform;
  //plasmacore.transform.to_floats( (float*) transform.m );

  /*
  transform.m[0][0] = 1.0f;
  transform.m[0][1] = 0.0f;
  transform.m[0][2] = 0.0f;
  transform.m[0][3] = 0.0f;

  transform.m[1][0] = 0.0f;
  transform.m[1][1] = 1.0f;
  transform.m[1][2] = 0.0f;
  transform.m[1][3] = 0.0f;

  transform.m[2][0] = 0.0f;
  transform.m[2][1] = 0.0f;
  transform.m[2][2] = 1.0f;
  transform.m[2][3] = 0.0f;

  transform.m[3][0] = 0.0f;
  transform.m[3][1] = 0.0f;
  transform.m[3][2] = 0.0f;
  transform.m[3][3] = 1.0f;
  d3d_device->SetTransform( D3DTS_VIEW, &transform );
  */

  //d3d_device->SetTransform( D3DTS_VIEW, (D3DMATRIX*) m );
}

void Texture_init( BardInt32* data, int w, int h, int format )
{
  // Helper fn
  BardObject* texture_obj = BARD_POP_REF();

	if (w==0 || h==0) return;

  // this method is also used to assign new data to an existing texture
  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if (texture == NULL)
  {
    texture = new DXTexture(w,h,false);
    BardLocalRef gc_guard(texture_obj);
    BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );
    BARD_SET_REF( texture_obj, "native_data", data_obj );

    BARD_SET( Vector2, texture_obj, "texture_size", 
        Vector2(texture->texture_width,texture->texture_height) );
    BARD_SET( Vector2, texture_obj, "image_size",   Vector2(w,h) );
  }

  BVM_NULL_CHECK( texture, return );

  texture->resize( w, h, false );

  // copy pixel data to image
  D3DLOCKED_RECT lock_rect;
  if (D3D_OK == texture->data->LockRect( 0, &lock_rect, NULL, 0 ))
  {
    BardInt32* src = data;
    BardInt32* dest  = (BardInt32*)lock_rect.pBits;
    int tex_w = texture->texture_width;

    for (int y = 0; y<h; y++)
    {
      memcpy( dest, src, 4*w );
      dest += tex_w;
      src  += w;
    }

    texture->data->UnlockRect(0);
  }
}

void Texture__init__Bitmap_Int32()
{
  int format = BARD_POP_INT32();
  BardObject* bitmap_obj = BARD_POP_REF();
  // Leave Texture obj on stack.

  BARD_GET_REF( array, bitmap_obj, "data" );
  BardInt32* data = (BardInt32*) (((BardArray*)array)->data);

  BARD_GET_INT32( w, bitmap_obj, "width" );
  BARD_GET_INT32( h, bitmap_obj, "height" );

  Texture_init( data, w, h, format );
}

void Texture__init__String_Int32()
{
  int format = BARD_POP_INT32();
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  // Leave Texture obj on stack.

  int data_size;
  char* data = image_archive.load( filename_obj, &data_size );

  if ( !data ) 
  {
    BARD_POP_REF();  // Texture obj
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
    BardInt32* dest = pixels;
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

void Texture__init__Vector2()
{
  // Texture::init(Vector2)
  Vector2 size = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  BVM_NULL_CHECK( texture_obj, return );

  int w = (int) size.x;
  int h = (int) size.y;
	if (w==0 || h==0) return;

  DXTexture* texture = new DXTexture(w,h,true);
  BardLocalRef gc_guard(texture_obj);
  BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );
  BARD_SET_REF( texture_obj, "native_data", data_obj );

  int wp2 = texture->texture_width;
  int hp2 = texture->texture_height;

  BARD_SET( Vector2, texture_obj, "texture_size", Vector2(wp2,hp2) );
  BARD_SET( Vector2, texture_obj, "image_size",   Vector2(w,h) );
}

void Texture__init__Vector2_Int32()
{
  // Texture::init(Vector2,Int32)
  BARD_POP_INT32();  // pixel format suggestion 
  int w = (int) BARD_POP_REAL64();
  int h = (int) BARD_POP_REAL64();
  BardObject* texture_obj = BARD_POP_REF();

  DXTexture* texture = new DXTexture(w,h,false);
  BardLocalRef gc_guard(texture_obj);
  BardObject* data_obj = BardNativeData::create( texture, BardNativeDataDeleteResource );
  BARD_SET_REF( texture_obj, "native_data", data_obj );

  BARD_SET( Vector2, texture_obj, "texture_size", 
      Vector2(texture->texture_width,texture->texture_height) );
  BARD_SET( Vector2, texture_obj, "image_size",   Vector2(w,h) );

  texture->resize( w, h, false );
}

void Texture__set__Bitmap_Vector2()
{
  Vector2 pos  = BARD_POP(Vector2);
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  BVM_NULL_CHECK( bitmap_obj, return );
  BardObject* texture_obj = BARD_POP_REF();

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  int x = (int) pos.x;
  int y = (int) pos.y;
  int w  = bitmap_obj->width;
  int h  = bitmap_obj->height;

  if (x<0 || y<0 || x+w > texture->texture_width || y+h > texture->texture_height)
  {
    return;
  }

  // copy pixel data to image
  D3DLOCKED_RECT lock_rect;
  if (D3D_OK == texture->data->LockRect( 0, &lock_rect, NULL, 0 ))
  {
    BardInt32* src = ((BardInt32*)bitmap_obj->pixels->data);
    int tex_w = texture->texture_width;
    BardInt32* dest  = ((BardInt32*)lock_rect.pBits) + y*tex_w + x;

    while (h > 0)
    {
      memcpy( dest, src, 4*w );
      dest += tex_w;
      src  += w;
	  --h;
    }

    texture->data->UnlockRect(0);
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

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, NULL );

  Vector2 pt1( 0, 0 );
  Vector2 pt2( size.x, 0 );
  Vector2 pt3( size.x, size.y );
  Vector2 pt4( 0, size.y );

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt1 = Matrix2x3_transform( plasmacore.transform, pt1 );
    pt2 = Matrix2x3_transform( plasmacore.transform, pt2 );
    pt3 = Matrix2x3_transform( plasmacore.transform, pt3 );
    pt4 = Matrix2x3_transform( plasmacore.transform, pt4 );
  }
  */

  color = plasmacore_argb_to_rgba( color );

  DXVertex2D v1( FLOAT(pt1.x), FLOAT(pt1.y), color, FLOAT(uv_a.x), FLOAT(uv_a.y) );
  DXVertex2D v2( FLOAT(pt2.x), FLOAT(pt2.y), color, FLOAT(uv_b.x), FLOAT(uv_a.y) );
  DXVertex2D v3( FLOAT(pt3.x), FLOAT(pt3.y), color, FLOAT(uv_b.x), FLOAT(uv_b.y) );
  DXVertex2D v4( FLOAT(pt4.x), FLOAT(pt4.y), color, FLOAT(uv_a.x), FLOAT(uv_b.y) );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  draw_buffer.add( v1, v2, v4 );
  draw_buffer.add( v4, v2, v3 );
}


void Texture__draw__Corners_Vector2_Color_Render_Blend_Texture_Corners()
{
  // Texture::draw(Corners,Vector2,Color,Render,Blend)
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

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  DXTexture* alpha_texture = NativeLayer_get_native_texture_data(alpha_texture_obj);
  if ( !alpha_texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, alpha_texture );

  Vector2 pt1( 0, 0 );
  Vector2 pt2( size.x, 0 );
  Vector2 pt3( size.x, size.y );
  Vector2 pt4( 0, size.y );

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt1 = Matrix2x3_transform( plasmacore.transform, pt1 );
    pt2 = Matrix2x3_transform( plasmacore.transform, pt2 );
    pt3 = Matrix2x3_transform( plasmacore.transform, pt3 );
    pt4 = Matrix2x3_transform( plasmacore.transform, pt4 );
  }
  */

  color = plasmacore_argb_to_rgba( color );

  DXVertex2D v1( FLOAT(pt1.x), FLOAT(pt1.y), color, FLOAT(uv_a.x), FLOAT(uv_a.y), FLOAT(alpha_uv_a.x), FLOAT(alpha_uv_a.y) );
  DXVertex2D v2( FLOAT(pt2.x), FLOAT(pt2.y), color, FLOAT(uv_b.x), FLOAT(uv_a.y), FLOAT(alpha_uv_b.x), FLOAT(alpha_uv_a.y) );
  DXVertex2D v3( FLOAT(pt3.x), FLOAT(pt3.y), color, FLOAT(uv_b.x), FLOAT(uv_b.y), FLOAT(alpha_uv_b.x), FLOAT(alpha_uv_b.y) );
  DXVertex2D v4( FLOAT(pt4.x), FLOAT(pt4.y), color, FLOAT(uv_a.x), FLOAT(uv_b.y), FLOAT(alpha_uv_a.x), FLOAT(alpha_uv_b.y) );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  draw_buffer.add( v1, v2, v4 );
  draw_buffer.add( v4, v2, v3 );
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

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, NULL );

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt1 = Matrix2x3_transform( plasmacore.transform, pt1 );
    pt2 = Matrix2x3_transform( plasmacore.transform, pt2 );
    pt3 = Matrix2x3_transform( plasmacore.transform, pt3 );
    pt4 = Matrix2x3_transform( plasmacore.transform, pt4 );
  }
  */

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );
  color4 = plasmacore_argb_to_rgba( color4 );

  DXVertex2D v1( FLOAT(pt1.x), FLOAT(pt1.y), color1, FLOAT(uv_a.x), FLOAT(uv_a.y) );
  DXVertex2D v2( FLOAT(pt2.x), FLOAT(pt2.y), color2, FLOAT(uv_b.x), FLOAT(uv_a.y) );
  DXVertex2D v3( FLOAT(pt3.x), FLOAT(pt3.y), color3, FLOAT(uv_b.x), FLOAT(uv_b.y) );
  DXVertex2D v4( FLOAT(pt4.x), FLOAT(pt4.y), color4, FLOAT(uv_a.x), FLOAT(uv_b.y) );

  v1.transform();
  v2.transform();
  v3.transform();
  v4.transform();

  draw_buffer.add( v1, v2, v4 );
  draw_buffer.add( v4, v2, v3 );
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

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  draw_buffer.set_render_flags( render_flags, src_blend, dest_blend );
  draw_buffer.set_textured_triangle_mode( texture, NULL );

  //NativeLayer_update_transform();
  /*
  if (plasmacore_set_transform())
  {
    pt1 = Matrix2x3_transform( plasmacore.transform, pt1 );
    pt2 = Matrix2x3_transform( plasmacore.transform, pt2 );
    pt3 = Matrix2x3_transform( plasmacore.transform, pt3 );
  }
  */

  color1 = plasmacore_argb_to_rgba( color1 );
  color2 = plasmacore_argb_to_rgba( color2 );
  color3 = plasmacore_argb_to_rgba( color3 );

  DXVertex2D v1( FLOAT(pt1.x), FLOAT(pt1.y), color1, FLOAT(uv_a.x), FLOAT(uv_a.y) );
  DXVertex2D v2( FLOAT(pt2.x), FLOAT(pt2.y), color2, FLOAT(uv_b.x), FLOAT(uv_b.y) );
  DXVertex2D v3( FLOAT(pt3.x), FLOAT(pt3.y), color3, FLOAT(uv_c.x), FLOAT(uv_c.y) );

  v1.transform();
  v2.transform();
  v3.transform();

  draw_buffer.add( v1, v2, v3 );
}

void Texture__draw_tile__Corners_Vector2_Vector2_Int32()
{
  BardInt32 render_flags = BARD_POP_INT32();
  Vector2 size = BARD_POP(Vector2);
  Vector2 pos  = BARD_POP(Vector2);
  Vector2 uv_a = BARD_POP(Vector2);
  Vector2 uv_b = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  DXTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
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

  Vector2 pt1( pos.x, pos.y );
  Vector2 pt2( pos.x+size.x, pos.y );
  Vector2 pt3( pos.x+size.x, pos.y+size.y );
  Vector2 pt4( pos.x, pos.y+size.y );

  if (hflip)
  {
    Vector2 temp = pt1;
    pt1 = pt2;
    pt2 = temp;
    temp = pt3;
    pt3 = pt4;
    pt4 = temp;
  }

  if (vflip)
  {
    Vector2 temp = pt1;
    pt1 = pt4;
    pt4 = temp;
    temp = pt2;
    pt2 = pt3;
    pt3 = temp;
  }

  DXVertex2D v1( FLOAT(pt1.x), FLOAT(pt1.y), 0xffffffff, FLOAT(uv_a.x), FLOAT(uv_a.y) );
  DXVertex2D v2( FLOAT(pt2.x), FLOAT(pt2.y), 0xffffffff, FLOAT(uv_b.x), FLOAT(uv_a.y) );
  DXVertex2D v3( FLOAT(pt3.x), FLOAT(pt3.y), 0xffffffff, FLOAT(uv_b.x), FLOAT(uv_b.y) );
  DXVertex2D v4( FLOAT(pt4.x), FLOAT(pt4.y), 0xffffffff, FLOAT(uv_a.x), FLOAT(uv_b.y) );

  draw_buffer.add( v1, v2, v4 );
  draw_buffer.add( v4, v2, v3 );
}

void bard_adjust_filename_for_os( char* filename, int buffer_size )
{
}

