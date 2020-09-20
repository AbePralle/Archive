#include "slag_version.h"
#define USAGE "GSlag Virtual Machine " VERSION "\nUsage: slag filename.etc [args]\n"

/*
 *=============================================================================
 *  gslag.cpp
 *
 *  $(SLAG_VERSION)
 *  ---------------------------------------------------------------------------
 *
 *  Copyright 2008-2011 Plasmaworks LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied. See the License for the specific
 *  language governing permissions and limitations under the License.
 *
 *  ---------------------------------------------------------------------------
 *
 *  History:
 *    2009.10.16 / Abe Pralle - Created
 *=============================================================================
*/

#include "slag.h"
#include "unzip.h"
#include "zip.h"
#include "gd.h"
#include "png.h"

#if defined(_WIN32)
#  define SLAGC_FILENAME  "slagc.exe"
#  define PATH_CHAR       "\\"
#else
#  define SLAGC_FILENAME  "slagc"
#  define PATH_CHAR       "/"
#endif

#define COMBINE_ARGB(a,r,g,b) ((a<<24)|(r<<16)|(g<<8)|b)

// patch for deprecated call in gd 2.0.35
extern "C" int png_check_sig( png_byte* sig, int num )
{
  return !png_sig_cmp(sig,0,num);
}

char** final_args  = 0;
char*  bin_path    = 0;
char*  src_path    = 0;
char*  slagc_path  = 0;
char*  etc_name = 0;

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

struct SlagBitmap : SlagObject
{
  SlagArray*    pixels;
  SlagInt32     width;
  SlagInt32     height;
};

void Bitmap__init__ArrayList_of_Byte();
void Bitmap__copy_pixels_to__Int32_Int32_Int32_Int32_Bitmap_Int32_Int32_Logical();
void Bitmap__to_png_bytes();
void Bitmap__to_jpg_bytes__Real64();
void Bitmap__rotate_right();
void Bitmap__rotate_left();
void Bitmap__rotate_180();
void Bitmap__flip_horizontal();
void Bitmap__flip_vertical();
void Bitmap__resize_horizontal__Int32();
void Bitmap__resize_vertical__Int32();

void NativeLayer_init_bitmap( SlagObject* bitmap_obj, char* raw_data, int data_size );

void ZipArchive__decompress__Int32();
void ZipArchive__load_entries();
void ZipArchive__load_raw__Int32();
void ZipArchive__add__String_Int64_ArrayList_of_Byte_Int32_Int32_Logical_Int32_Int32();

void error( const char* mesg );

int main( int argc, char** argv )
{
  try
  {
    slag_set_raw_exe_filepath( argv[0] );
    if (argc < 2) { fprintf( stderr, USAGE ); return 1; }

    SlagLoader loader;
    loader.load(argv[1]);

    vm.register_native_method("Bitmap","init(ArrayList<<Byte>>)", Bitmap__init__ArrayList_of_Byte );
    vm.register_native_method("Bitmap","to_png_bytes()", Bitmap__to_png_bytes );
    vm.register_native_method("Bitmap","to_jpg_bytes(Real64)", Bitmap__to_jpg_bytes__Real64 );
    vm.register_native_method( "Bitmap","copy_pixels_to(Int32,Int32,Int32,Int32,Bitmap,Int32,Int32,Logical)",
        Bitmap__copy_pixels_to__Int32_Int32_Int32_Int32_Bitmap_Int32_Int32_Logical );
    vm.register_native_method("Bitmap", "rotate_right()",      Bitmap__rotate_right );
    vm.register_native_method("Bitmap", "rotate_left()",       Bitmap__rotate_left );
    vm.register_native_method("Bitmap", "rotate_180()",        Bitmap__rotate_180 );
    vm.register_native_method("Bitmap", "flip_horizontal()",   Bitmap__flip_horizontal );
    vm.register_native_method("Bitmap", "flip_vertical()",     Bitmap__flip_vertical );
    vm.register_native_method("Bitmap", "resize_horizontal(Int32)", Bitmap__resize_horizontal__Int32 );
    vm.register_native_method("Bitmap", "resize_vertical(Int32)",   Bitmap__resize_vertical__Int32 );
    vm.register_native_method( "ZipArchive", "decompress(Int32)",  ZipArchive__decompress__Int32 );
    vm.register_native_method( "ZipArchive", "load_entries()",  ZipArchive__load_entries );
    vm.register_native_method( "ZipArchive", "load_raw(Int32)", ZipArchive__load_raw__Int32 );
    vm.register_native_method( "ZipArchive",
        "add(String,Int64,ArrayList<<Byte>>,Int32,Int32,Logical,Int32,Int32)",
        ZipArchive__add__String_Int64_ArrayList_of_Byte_Int32_Int32_Logical_Int32_Int32 );

    slag_configure();
    slag_set_command_line_args( argv+2, argc-2 );
    slag_launch();
    slag_shut_down();

    if (final_args)  delete final_args;
    if (slagc_path)  delete slagc_path;

    printf( "\n" );
    return 0;
  }
  catch (int err)
  {
    if (err && slag_error_message.value) fprintf( stderr, "%s\n", slag_error_message.value );
    return err;
  }
}

void error( const char* mesg )
{
  fprintf( stderr, "%s", mesg );
  fprintf( stderr, "\n" );
  //fprintf( stderr, "Visit http://plasmaworks.com/gogo for help.\n" );
  exit(1);
}

char* new_string( const char* st )
{
  int len = strlen(st);
  char* buffer = new char[len+1];
  strcpy(buffer,st);
  return buffer;
}

void slag_adjust_filename_for_os( char* filename, int len );

void slag_adjust_filename_for_os( char* filename, int len )
{
}

void Bitmap__init__ArrayList_of_Byte()
{
  // Bitmap::init(Byte[])
  SlagArrayList* list = (SlagArrayList*) SLAG_POP_REF();
  SlagObject* bitmap_obj = SLAG_POP_REF();

  NativeLayer_init_bitmap( bitmap_obj, (char*) list->array->data, list->count );
}

void Bitmap__rotate_right()
{
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();

  SlagInt32 w = bitmap_obj->width;
  SlagInt32 h = bitmap_obj->height;

  SlagInt32* rotate_buffer = new SlagInt32[w*h];
  SlagInt32* dest_start = rotate_buffer + h - 1;
  int di = h;
  int dj = -1;

  SlagInt32* src = (SlagInt32*) (bitmap_obj->pixels->data);
  --src;  // prepare for preincrement ahead
  for (int j=h; j>0; --j)
  {
    SlagInt32* dest = dest_start;
    for (int i=w; i>0; --i)
    {
      *(dest) = *(++src);
      dest += di;
    }
    dest_start += dj;
  }

  memcpy( bitmap_obj->pixels->data, rotate_buffer, w*h*4 );
  delete rotate_buffer;

  bitmap_obj->width = h;
  bitmap_obj->height = w;
}

void Bitmap__rotate_left()
{
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();

  SlagInt32 w = bitmap_obj->width;
  SlagInt32 h = bitmap_obj->height;

  SlagInt32* rotate_buffer = new SlagInt32[w*h];
  SlagInt32* dest_start = rotate_buffer + w*h - h;
  int di = -h;
  int dj = 1;

  SlagInt32* src = (SlagInt32*) (bitmap_obj->pixels->data);
  --src;  // prepare for preincrement ahead
  for (int j=h; j>0; --j)
  {
    SlagInt32* dest = dest_start;
    for (int i=w; i>0; --i)
    {
      *(dest) = *(++src);
      dest += di;
    }
    dest_start += dj;
  }

  memcpy( bitmap_obj->pixels->data, rotate_buffer, w*h*4 );
  delete rotate_buffer;

  bitmap_obj->width = h;
  bitmap_obj->height = w;
}

void Bitmap__rotate_180()
{
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();
  SlagInt32 w = bitmap_obj->width;
  SlagInt32 h = bitmap_obj->height;
  SlagInt32* src  = (SlagInt32*) (bitmap_obj->pixels->data);
  SlagInt32* dest = src + w*h;
  --src;

  int count = (w*h/2) + 1;
  while (--count)
  {
    SlagInt32 c = *(++src);
    *src = *(--dest);
    *dest = c;
  }
}

void Bitmap__flip_horizontal()
{
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();

  SlagInt32 w = bitmap_obj->width;
  SlagInt32 h = bitmap_obj->height;

  SlagInt32* src_start = (SlagInt32*) (bitmap_obj->pixels->data);

  int j = h + 1;
  while (--j)
  {
    SlagInt32* src = src_start - 1;
    SlagInt32* dest = src_start + w;
    int count = (w>>1) + 1;
    while (--count)
    {
      SlagInt32 c = *(++src);
      *src = *(--dest);
      *dest = c;
    }
    src_start += w;
  }
}

void Bitmap__flip_vertical()
{
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();

  SlagInt32 w = bitmap_obj->width;
  SlagInt32 h = bitmap_obj->height;

  SlagInt32* src  = ((SlagInt32*) (bitmap_obj->pixels->data)) - 1;
  SlagInt32* dest = ((SlagInt32*) (bitmap_obj->pixels->data)) + w*h - w - 1;

  int j = (h>>1) + 1;
  while (--j)
  {
    int i = w + 1;
    while (--i)
    {
      SlagInt32 c = *(++src);
      *src = *(++dest);
      *dest = c;
    }
    dest -= (w + w);
  }
}

void Bitmap__resize_horizontal__Int32()
{
  SlagInt32 new_w = (SlagInt32) SLAG_POP_INTEGER();
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();

  SlagInt32 w = bitmap_obj->width;
  SlagInt32 h = bitmap_obj->height;

  if (w == new_w) return;
  if (new_w <= 0) return;

  SlagArray* original_data = bitmap_obj->pixels;
  while (new_w <= w/2 && (w&1)==0)
  {
    // average every two horizontal pixels to speed up the process
    int count = w*h / 2 + 1;
    SlagInt32* src  = ((SlagInt32*) (original_data->data)) - 1;
    SlagInt32* dest  = src;
    while (--count)
    {
      int c = *(++src);
      int a = (c >> 24) & 255;
      int r = (c >> 16) & 255;
      int g = (c >>  8) & 255;
      int b = c & 255;

      c = *(++src);
      a = (a + ((c >> 24) & 255)) >> 1;
      r = (r + ((c >> 16) & 255)) >> 1;
      g = (g + ((c >>  8) & 255)) >> 1;
      b = (b + (c & 255)) >> 1;

      *(++dest) = COMBINE_ARGB(a,r,g,b);
    }
    w /= 2;
    bitmap_obj->width = w;
  }

  SLAG_PUSH_REF( (SlagObject*) original_data );
  SLAG_PUSH_REF( (SlagObject*) bitmap_obj );  // to retrieve after possible gc
  SLAG_PUSH_REF( (SlagObject*) bitmap_obj );  // for init() call
  SLAG_PUSH_INTEGER( new_w );
  SLAG_PUSH_INTEGER( h );
  SLAG_CALL( bitmap_obj->type, "init(Int32,Int32)" );
  bitmap_obj = (SlagBitmap*) SLAG_POP_REF();  // recover ref after possible gc
  original_data = (SlagArray*) SLAG_POP_REF();

  SlagArray* new_data = bitmap_obj->pixels;

  SlagInt32* src  = ((SlagInt32*) (original_data->data)) - 1;
  SlagInt32* dest = ((SlagInt32*) (new_data->data)) - 1;

  double sum_a, sum_r, sum_g, sum_b;
  sum_a = sum_r = sum_g = sum_b = 0.0;
  double cur_a, cur_r, cur_g, cur_b;
  cur_a = cur_r = cur_g = cur_b = 0.0;
  double sum_weight=0.0, cur_weight=0.0;
  double progress=0.0;

  double ratio = new_w / double(w);
  int lines = h + 1;
  while (--lines)
  {
    int columns = w;
    int dest_columns = new_w;
    while (columns--)
    {
      sum_a += cur_a;
      sum_r += cur_r;
      sum_g += cur_g;
      sum_b += cur_b;
      sum_weight += cur_weight;
      int c = *(++src);
      cur_a = (c >> 24) & 255;
      cur_r = (c >> 16) & 255;
      cur_g = (c >>  8) & 255;
      cur_b = c & 255;
      progress += ratio;

      cur_weight = 1.0;

      while (progress >= 2.0 && dest_columns > 1)
      {
        sum_weight += 1.0;
        *(++dest) = (int((sum_a+cur_a)/sum_weight)<<24)
          | (int((sum_r+cur_r)/sum_weight)<<16)
          | (int((sum_g+cur_g)/sum_weight)<<8)
          | int((sum_b+cur_b)/sum_weight);
        --dest_columns;
        sum_a = sum_r = sum_g = sum_b = sum_weight = 0.0;
        progress -= 1.0;
        cur_weight = 1.0 - (ratio - progress);
      }

      if (progress >= 1.0 || !columns)
      {
        double cur_part = ratio - (progress - 1.0);
        if (cur_part > 1.0) cur_part = 1.0;
        double a_part = cur_a * cur_part;
        double r_part = cur_r * cur_part;
        double g_part = cur_g * cur_part;
        double b_part = cur_b * cur_part;
        cur_a -= a_part;
        cur_r -= r_part;
        cur_g -= g_part;
        cur_b -= b_part;
        sum_a += a_part;
        sum_r += r_part;
        sum_g += g_part;
        sum_b += b_part;
        sum_weight += cur_part;
        cur_weight = 1.0 - cur_part;
        progress -= 1.0;

        *(++dest) = (int(sum_a/sum_weight)<<24)
          | (int(sum_r/sum_weight)<<16)
          | (int(sum_g/sum_weight)<<8)
          | int(sum_b/sum_weight);
        --dest_columns;

        sum_a = sum_r = sum_g = sum_b = sum_weight = 0.0;
      }
    }
  }
}

void Bitmap__resize_vertical__Int32()
{
  SlagInt32 new_h = (SlagInt32) SLAG_POP_INTEGER();
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();

  SlagInt32 w = bitmap_obj->width;
  SlagInt32 h = bitmap_obj->height;

  if (h == new_h) return;
  if (new_h <= 0) return;

  SlagArray* original_data = bitmap_obj->pixels;
  while (new_h <= h/2 && (h&1)==0)
  {
    // average every two vertical pixels to speed up the process
    SlagInt32* src  = ((SlagInt32*) (original_data->data)) - 1;
    SlagInt32* dest = ((SlagInt32*) (original_data->data)) - 1;
    int lines = h/2 + 1;
    while (--lines)
    {
      int count = w + 1;
      while (--count)
      {
        int c = *(++src);
        int a = (c >> 24) & 255;
        int r = (c >> 16) & 255;
        int g = (c >>  8) & 255;
        int b = c & 255;

        c = *(src+w);
        a = (a + ((c >> 24) & 255)) >> 1;
        r = (r + ((c >> 16) & 255)) >> 1;
        g = (g + ((c >>  8) & 255)) >> 1;
        b = (b + (c & 255)) >> 1;

        *(++dest) = COMBINE_ARGB(a,r,g,b);
      }
      src += w;
    }
    h /= 2;
    bitmap_obj->height = h;
  }

  SLAG_PUSH_REF( (SlagObject*) original_data );
  SLAG_PUSH_REF( (SlagObject*) bitmap_obj );  // to retrieve after possible gc
  SLAG_PUSH_REF( (SlagObject*) bitmap_obj );  // for init() call
  SLAG_PUSH_INTEGER( w );
  SLAG_PUSH_INTEGER( new_h );
  SLAG_CALL( bitmap_obj->type, "init(Int32,Int32)" );
  bitmap_obj = (SlagBitmap*) SLAG_POP_REF();  // recover ref after possible gc
  original_data = (SlagArray*) SLAG_POP_REF();

  SlagArray* new_data = bitmap_obj->pixels;

  SlagInt32* src_start  = ((SlagInt32*) (original_data->data));
  SlagInt32* dest_start = ((SlagInt32*) (new_data->data));

  double sum_a, sum_r, sum_g, sum_b;
  sum_a = sum_r = sum_g = sum_b = 0.0;
  double cur_a, cur_r, cur_g, cur_b;
  cur_a = cur_r = cur_g = cur_b = 0.0;
  double sum_weight=0.0, cur_weight=0.0;
  double progress=0.0;

  double ratio = new_h / double(h);
  int columns = w + 1;
  while (--columns)
  {
    SlagInt32* src  = src_start;
    SlagInt32* dest = dest_start;
    int rows = h;
    int dest_rows = new_h;
    while (rows--)
    {
      sum_a += cur_a;
      sum_r += cur_r;
      sum_g += cur_g;
      sum_b += cur_b;
      sum_weight += cur_weight;
      int c = *src;
      src += w;
      cur_a = (c >> 24) & 255;
      cur_r = (c >> 16) & 255;
      cur_g = (c >>  8) & 255;
      cur_b = c & 255;
      progress += ratio;

      cur_weight = 1.0;

      while (progress >= 2.0 && dest_rows > 1)
      {
        sum_weight += 1.0;
        *dest = (int((sum_a+cur_a)/sum_weight)<<24)
          | (int((sum_r+cur_r)/sum_weight)<<16)
          | (int((sum_g+cur_g)/sum_weight)<<8)
          | int((sum_b+cur_b)/sum_weight);
        dest += w;
        --dest_rows;
        sum_a = sum_r = sum_g = sum_b = sum_weight = 0.0;
        progress -= 1.0;
        cur_weight = 1.0 - (ratio - progress);
      }

      if (progress >= 1.0 || !rows)
      {
        double cur_part = ratio - (progress - 1.0);
        if (cur_part > 1.0) cur_part = 1.0;
        double a_part = cur_a * cur_part;
        double r_part = cur_r * cur_part;
        double g_part = cur_g * cur_part;
        double b_part = cur_b * cur_part;
        cur_a -= a_part;
        cur_r -= r_part;
        cur_g -= g_part;
        cur_b -= b_part;
        sum_a += a_part;
        sum_r += r_part;
        sum_g += g_part;
        sum_b += b_part;
        sum_weight += cur_part;
        cur_weight = 1.0 - cur_part;
        progress -= 1.0;

        *dest = (int(sum_a/sum_weight)<<24)
          | (int(sum_r/sum_weight)<<16)
          | (int(sum_g/sum_weight)<<8)
          | int(sum_b/sum_weight);
        dest += w;
        --dest_rows;

        sum_a = sum_r = sum_g = sum_b = sum_weight = 0.0;
      }
    }
    ++src_start;
    ++dest_start;
  }
}


void Bitmap__copy_pixels_to__Int32_Int32_Int32_Int32_Bitmap_Int32_Int32_Logical()
{
  SlagInt32 blend_alpha = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 dest_y = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 dest_x = (SlagInt32) SLAG_POP_INTEGER();
  SlagObject* dest_obj = SLAG_POP_REF();
  SlagInt32 height = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 width  = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 src_y  = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 src_x  = (SlagInt32) SLAG_POP_INTEGER();
  SlagObject* src_obj = SLAG_POP_REF();

  VM_NULL_CHECK( src_obj,  return );
  VM_NULL_CHECK( dest_obj, return );

  SLAG_GET( SlagInt32, src_width, src_obj,   "width" );
  SLAG_GET( SlagInt32, dest_width, dest_obj,  "width" );
  SLAG_GET( SlagArray*, src_array, src_obj,   "data" );
  SLAG_GET( SlagArray*, dest_array, dest_obj, "data" );
  SlagInt32* src_data = (SlagInt32*) src_array->data;
  SlagInt32* dest_data = (SlagInt32*) dest_array->data;

  SlagInt32 dest_skip_width = dest_width - width;
  SlagInt32 src_skip_width  = src_width - width;

  src_data  += src_y * src_width + src_x - 1;
  dest_data += dest_y * dest_width + dest_x - 1;

  if (blend_alpha)
  {
    for (int j=height; j>0; --j)
    {
      for (int i=width; i>0; --i)
      {
        int bottom = *(++dest_data);
        int top    = *(++src_data);
        int tr = (top >> 16) & 255;
        int tg = (top >> 8) & 255;
        int tb = (top & 255);
        int r = (bottom >> 16) & 255;
        int g = (bottom >> 8) & 255;
        int b = (bottom & 255);
        int inv_alpha = 255 - ((top >> 24) & 255);

        // we assume that tr, tg, and tb are premultiplied
        tr += ((r * inv_alpha) / 255);
        tg += ((g * inv_alpha) / 255);
        tb += ((b * inv_alpha) / 255);
        *dest_data = (SlagInt32) (0xff000000 | (tr<<16) | (tg<<8) | tb);
      }
      dest_data += dest_skip_width;
      src_data  += src_skip_width;
    }
  }
  else
  {
    for (int j=height; j>0; --j)
    {
      for (int i=width; i>0; --i)
      {
        *(++dest_data) = *(++src_data);
      }
      dest_data += dest_skip_width;
      src_data  += src_skip_width;
    }
  }
}

void Bitmap__to_png_bytes()
{
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();
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

  SLAG_PUSH_REF( slag_create_byte_list( bytes, size ) );
  gdFree( bytes );
}

void Bitmap__to_jpg_bytes__Real64()
{
  double compression = SLAG_POP_REAL();
  SlagBitmap* bitmap_obj = (SlagBitmap*) SLAG_POP_REF();
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
  SLAG_PUSH_REF( slag_create_byte_list( bytes, size ) );
  gdFree( bytes );
}


void NativeLayer_init_bitmap( SlagObject* bitmap_obj, char* raw_data, int data_size )
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
      SlagByte* src = ((SlagByte*) (img->pixels[j])) - 1;
      SlagInt32* dest = ((SlagInt32*) (true_color_img->tpixels[j])) - 1;
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

    SLAG_PUSH_REF( bitmap_obj );
    SLAG_PUSH_REF( bitmap_obj );
    SLAG_PUSH_INTEGER( width );
    SLAG_PUSH_INTEGER( height );
    SLAG_CALL( bitmap_obj->type, "init(Int32,Int32)" );
    SLAG_GET_REF( array_obj, SLAG_PEEK_REF(), "data" );
    SlagArray* array = (SlagArray*) array_obj;
    SLAG_POP_REF();

    // premultiply the alpha
    SlagInt32* dest = ((SlagInt32*) array->data) - 1;
    for (int j=0; j<height; ++j)
    {
      SlagInt32* cur = ((SlagInt32*) img->tpixels[j]) - 1;
      int count = width + 1;
      while (--count)
      {
        SlagInt32 color = *(++cur);
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

void ZipArchive__decompress__Int32()
{
  // ZipArchive::decompress(Int32).Byte[]
  SlagInt32 index = (SlagInt32) SLAG_POP_INTEGER();
  SlagObject* archive_obj = SLAG_POP_REF();
  VM_NULL_CHECK( archive_obj, vm.type_null_reference_error );

  SLAG_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  VM_NULL_CHECK( archive_filename_obj, vm.type_null_reference_error );

  char archive_filename[PATH_MAX];
  ((SlagString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

  unzFile zfp = unzOpen( archive_filename );

  if (zfp)
  {
    int status = unzGoToFirstFile(zfp);
    while (UNZ_OK == status)
    {
      if (index == 0)
      {
        unz_file_info file_info;
        status = unzGetCurrentFileInfo( zfp, &file_info, NULL, 0, NULL, 0, NULL, 0 );
        if (UNZ_OK != status) break;

        //int crc32 = file_info.crc;
        //int compressed_count = file_info.compressed_size;
        int uncompressed_count = file_info.uncompressed_size;

        SlagInt64 ztime = file_info.tmu_date.tm_year;  // year eg 2009
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mon + 1);  // month 1..12
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mday);  // day 1..31
        ztime = (ztime * 100) + (file_info.tmu_date.tm_hour);  // hour 0..23
        ztime = (ztime * 100) + (file_info.tmu_date.tm_min);   // minute 0..59
        ztime = (ztime * 100) + (file_info.tmu_date.tm_sec);   // second 0..59

        SLAG_FIND_TYPE( type_byte_array, "Array<<Byte>>" );
        SlagArray*    bytes = type_byte_array->create( uncompressed_count );
        SLAG_PUSH_REF( bytes );

        unzOpenCurrentFile( zfp );
        unzReadCurrentFile( zfp, ((char*) bytes->data), uncompressed_count );
        unzClose(zfp);
        return;
      }
      status = unzGoToNextFile(zfp);
      --index;
    }
  }

printf("error 1\n");
  slag_throw_file_error( archive_filename );
  return;
}


void ZipArchive__load_entries()
{
  SlagObject* archive_obj = SLAG_PEEK_REF();
  VM_NULL_CHECK( archive_obj, vm.type_null_reference_error );

  SLAG_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  VM_NULL_CHECK( archive_filename_obj, vm.type_null_reference_error );

  char archive_filename[PATH_MAX];
  ((SlagString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

  unzFile zfp = unzOpen( archive_filename );
  if ( !zfp )
  {
    zfp = zipOpen( archive_filename, APPEND_STATUS_CREATE );
    zipClose( zfp, NULL );
    SLAG_POP_REF();
    return;
  }

  int index = 0;
  int status = unzGoToFirstFile(zfp);
  while (UNZ_OK == status)
  {
    char current_filename[256+1];
    unz_file_info file_info;
    status = unzGetCurrentFileInfo( zfp, &file_info, current_filename, 256, NULL, 0, NULL, 0 );
    if (UNZ_OK == status)
    {
      SlagInt64 ztime = file_info.tmu_date.tm_year;  // year eg 2009
      ztime = (ztime * 100) + (file_info.tmu_date.tm_mon + 1);  // month 1..12
      ztime = (ztime * 100) + (file_info.tmu_date.tm_mday);  // day 1..31
      ztime = (ztime * 100) + (file_info.tmu_date.tm_hour);  // hour 0..23
      ztime = (ztime * 100) + (file_info.tmu_date.tm_min);   // minute 0..59
      ztime = (ztime * 100) + (file_info.tmu_date.tm_sec);   // second 0..59

      SLAG_PUSH_REF( archive_obj );
      SLAG_PUSH_INTEGER( index );
      SLAG_PUSH_REF( SlagString::create(current_filename) );
      SLAG_PUSH_INTEGER( ztime );  // timestamp
      SLAG_CALL( archive_obj->type, "add_entry(Int32,String,Int64)" );

      status = unzGoToNextFile(zfp);
    }
    ++index;
  }
  unzClose(zfp);

  SLAG_POP_REF();  // archive_obj
}

void ZipArchive__load_raw__Int32()
{
  // ZipArchive::load_raw(Int32).RawZipData
  SlagInt32 index = (SlagInt32) SLAG_POP_INTEGER();
  SlagObject* archive_obj = SLAG_POP_REF();
  VM_NULL_CHECK( archive_obj, vm.type_null_reference_error );

  SLAG_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  VM_NULL_CHECK( archive_filename_obj, vm.type_null_reference_error );

  char archive_filename[PATH_MAX];
  ((SlagString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

  unzFile zfp = unzOpen( archive_filename );

  int original_index = index;
  if (zfp)
  {
    int status = unzGoToFirstFile(zfp);
    while (UNZ_OK == status)
    {
      if (index == 0)
      {
        unz_file_info file_info;
        status = unzGetCurrentFileInfo( zfp, &file_info, NULL, 0, NULL, 0, NULL, 0 );
        if (UNZ_OK != status)
        {
          printf( "[Internal] Failed to get current file info." );
          break;
        }

        int crc32 = file_info.crc;
        int compressed_count = file_info.compressed_size;
        int uncompressed_count = file_info.uncompressed_size;

        SlagInt64 ztime = file_info.tmu_date.tm_year;  // year eg 2009
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mon + 1);  // month 1..12
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mday);  // day 1..31
        ztime = (ztime * 100) + (file_info.tmu_date.tm_hour);  // hour 0..23
        ztime = (ztime * 100) + (file_info.tmu_date.tm_min);   // minute 0..59
        ztime = (ztime * 100) + (file_info.tmu_date.tm_sec);   // second 0..59

        int compression_method, compression_level;

        unzOpenCurrentFile2( zfp, &compression_method, &compression_level, 1 );

        SLAG_FIND_TYPE( type_raw_zip_data, "RawZipData" );
        SLAG_PUSH_REF( type_raw_zip_data->create() );
        SLAG_DUPLICATE_REF();

        SLAG_FIND_TYPE( type_byte_array, "Array<<Byte>>" );
        SlagArray* array = type_byte_array->create(compressed_count);
        SLAG_PUSH_REF( array );
        char* data = (char*) array->data;

        unzReadCurrentFile( zfp, data, compressed_count );

        SLAG_PUSH_INTEGER( uncompressed_count );
        SLAG_PUSH_INTEGER( ztime );
        SLAG_PUSH_INTEGER( compression_method );
        SLAG_PUSH_INTEGER( compression_level );
        SLAG_PUSH_INTEGER( crc32 );

        SLAG_CALL( type_raw_zip_data, "init(Array<<Byte>>,Int32,Int64,Int32,Int32,Int32)" )
        unzClose(zfp);
        return;
      }
      status = unzGoToNextFile(zfp);
      --index;
    }
    printf( "[Internal] Failed to advance to next file." );
  }
  else
  {
    printf( "[Internal] Failed to open archive." );
  }

  // This happens once in a while - gradually tracking down what's happening
  // with the printfs above.
  printf( "Please try again\n" );
  slag_throw_file_error( archive_filename );
  return;
}

void ZipArchive__add__String_Int64_ArrayList_of_Byte_Int32_Int32_Logical_Int32_Int32()
{
  SlagInt32 crc32 = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 uncompressed_count = (SlagInt32) SLAG_POP_INTEGER();
  SlagInt32 raw = (SlagInt32) SLAG_POP_INTEGER();

  SlagInt32 compression_level = (SlagInt32) SLAG_POP_INTEGER();
  if (compression_level < 0) compression_level = 0;
  else if (compression_level > 9) compression_level = 9;

  SlagInt32 compression_method = (SlagInt32) SLAG_POP_INTEGER();
  if (compression_method != 0) compression_method = Z_DEFLATED;

  SlagArrayList* byte_list = (SlagArrayList*) SLAG_POP_REF();
  SlagInt64 ztime = SLAG_POP_INTEGER();
  SlagString* filename_obj = (SlagString*) SLAG_POP_REF();
  SlagObject* archive_obj = SLAG_PEEK_REF();

  VM_NULL_CHECK( archive_obj, vm.type_null_reference_error );
  VM_NULL_CHECK( filename_obj, vm.type_null_reference_error );
  VM_NULL_CHECK( byte_list, vm.type_null_reference_error );

  SLAG_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  VM_NULL_CHECK( archive_filename_obj, vm.type_null_reference_error );
  char archive_filename[PATH_MAX];
  ((SlagString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

  char filename[PATH_MAX];
  filename_obj->to_ascii( filename, PATH_MAX );

  tm_zip timestamp;
  timestamp.tm_year = (int) ((ztime/10000000000LL) % 10000);
  timestamp.tm_mon  = (int) ((ztime/100000000) % 100) - 1;
  timestamp.tm_mday = (int) ((ztime/1000000) % 100);
  timestamp.tm_hour = (int) ((ztime/10000) % 100);
  timestamp.tm_min  = (int) ((ztime/100) % 100);
  timestamp.tm_sec  = (int) (ztime % 100);

  int count = byte_list->count;
  SlagByte* data = (SlagByte*) byte_list->array->data;

  zipFile zfp = zipOpen( archive_filename, APPEND_STATUS_ADDINZIP );
  if ( !zfp ) zfp = zipOpen( archive_filename, APPEND_STATUS_CREATE );

  if ( !zfp )
  {
printf("error 3\n");
    slag_throw_file_error( archive_filename );
    return;
  }

  zip_fileinfo file_info;
  memset( &file_info, 0, sizeof(zip_fileinfo) );
  file_info.tmz_date = timestamp;

  int error = zipOpenNewFileInZip2( zfp, filename, &file_info,
      NULL, 0, NULL, 0, NULL,
      compression_method, compression_level, raw );

  if (0 == error) error |= zipWriteInFileInZip( zfp, data, count );

  if (raw) zipCloseFileInZipRaw( zfp, uncompressed_count, crc32 );
  else     zipCloseFileInZip(zfp);

  zipClose( zfp, NULL );

  if (error)
  {
printf("error 4\n");
    slag_throw_file_error( archive_filename );
    return;
  }

  SLAG_POP_REF();
}

