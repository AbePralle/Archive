#include "bard_version.h"
#define USAGE "GoGo Build System " VERSION "\nUsage: gogo [target-name]\n"

/*
 *=============================================================================
 *  gogo.cpp
 *
 *  $(BARD_VERSION)
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

#include "bard.h"
#include "unzip.h"
#include "zip.h"
#include "gd.h"
#include "png.h"

#if defined(_WIN32)
#  define BARDIC_FILENAME  "bardic.exe"
#  define PATH_CHAR       "\\"
#else
#  define BARDIC_FILENAME  "bardic"
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
char*  bardic_path  = 0;
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

struct BardBitmap : BardObject
{
  BardArray*    pixels;
  BardInt32     width;
  BardInt32     height;
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

void NativeLayer_init_bitmap( BardObject* bitmap_obj, char* raw_data, int data_size );

void ZipArchive__decompress__Int32();
void ZipArchive__load_entries();
void ZipArchive__load_raw__Int32();
void ZipArchive__add__String_Int64_ArrayList_of_Byte_Int32_Int32_Logical_Int32_Int32();

void error( const char* mesg );
int  process_args( int argc, char** argv );
bool exists( const char* filename );
void find_executables();
void check_build_etc();
void compile_buildfile();

int main( int argc, char** argv )
{
  try
  {
    bard_set_raw_exe_filepath( argv[0] );

    argc = process_args( argc-1, argv+1 );
    argv = final_args;

    find_executables();
    check_build_etc();

    BardLoader loader;
    if (etc_name) loader.load(etc_name);
    else          loader.load("build.etc");

    bvm.register_native_method("Bitmap","init(ArrayList<<Byte>>)", Bitmap__init__ArrayList_of_Byte );
    bvm.register_native_method("Bitmap","to_png_bytes()", Bitmap__to_png_bytes );
    bvm.register_native_method("Bitmap","to_jpg_bytes(Real64)", Bitmap__to_jpg_bytes__Real64 );
    bvm.register_native_method( "Bitmap","copy_pixels_to(Int32,Int32,Int32,Int32,Bitmap,Int32,Int32,Logical)",
        Bitmap__copy_pixels_to__Int32_Int32_Int32_Int32_Bitmap_Int32_Int32_Logical );
    bvm.register_native_method("Bitmap", "rotate_right()",      Bitmap__rotate_right );
    bvm.register_native_method("Bitmap", "rotate_left()",       Bitmap__rotate_left );
    bvm.register_native_method("Bitmap", "rotate_180()",        Bitmap__rotate_180 );
    bvm.register_native_method("Bitmap", "flip_horizontal()",   Bitmap__flip_horizontal );
    bvm.register_native_method("Bitmap", "flip_vertical()",     Bitmap__flip_vertical );
    bvm.register_native_method("Bitmap", "resize_horizontal(Int32)", Bitmap__resize_horizontal__Int32 );
    bvm.register_native_method("Bitmap", "resize_vertical(Int32)",   Bitmap__resize_vertical__Int32 );
    bvm.register_native_method( "ZipArchive", "decompress(Int32)",  ZipArchive__decompress__Int32 );
    bvm.register_native_method( "ZipArchive", "load_entries()",  ZipArchive__load_entries );
    bvm.register_native_method( "ZipArchive", "load_raw(Int32)", ZipArchive__load_raw__Int32 );
    bvm.register_native_method( "ZipArchive", 
        "add(String,Int64,ArrayList<<Byte>>,Int32,Int32,Logical,Int32,Int32)",
        ZipArchive__add__String_Int64_ArrayList_of_Byte_Int32_Int32_Logical_Int32_Int32 );

    bard_configure();
    bard_set_command_line_args( argv, argc );
    bard_launch();
    bard_shut_down();

    if (final_args)  delete final_args;
    if (bardic_path)  delete bardic_path;

    printf( "\n" );
    return 0;
  }
  catch (int err)
  {
    if (err && bard_error_message.value) fprintf( stderr, "%s\n", bard_error_message.value );
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

int process_args( int argc, char** argv )
{
  final_args = new char*[argc];
  memset( final_args, 0, sizeof(char*)*argc );
  int final_count = 0;
  for (int i=0; i<argc; ++i)
  {
    if (0==strcmp(argv[i],"-bin"))
    {
      if (i+1 == argc)
      {
        error( "Path expected after -bin" );
      }
      bin_path = argv[++i];
    }
    else if (0==strcmp(argv[i],"-etc"))
    {
      if (i+1 == argc)
      {
        error( "Path expected after -etc" );
      }
      etc_name = argv[++i];
    }
    else if (0==strcmp(argv[i],"-src_path"))
    {
      if (i+1 == argc)
      {
        error( "Path expected after -src_path" );
      }
      src_path = argv[++i];
    }
    else if (0==strcmp(argv[i],"-create"))
    {
      ++i;
      FILE* fp = fopen( "build.bard", "rb" );
      if (fp)
      {
        printf( "-create: build.bard already exists!\n" );
      }
      else
      {
        fp = fopen( "build.bard", "w" );
        fprintf( fp, "class Main : GoGo\n" );
        fprintf( fp, "  METHODS\n" );
        fprintf( fp, "    method build( String cmd )\n" );
        fprintf( fp, "      which (cmd)\n" );
        fprintf( fp, "        case \"hi\":\n" );
        fprintf( fp, "          execute(\"echo hi\")\n" );
        fprintf( fp, "        others:\n" );
        fprintf( fp, "          println( \"default build (try \\\"gogo hi\\\")\" )\n" );
        fprintf( fp, "      endWhich\n" );
        fprintf( fp, "endClass\n" );

        fclose(fp);
        printf( "Created build.bard starter file.\n" );
        exit(1);
      }
    }
    else
    {
      final_args[final_count++] = argv[i];
    }
  }
  return final_count;
}

bool exists( const char* filename )
{
  FILE* fp = fopen(filename,"rb");
  if ( !fp ) return false;
  fclose(fp);
  return true;
}

char* new_string( const char* st )
{
  int len = strlen(st);
  char* buffer = new char[len+1];
  strcpy(buffer,st);
  return buffer;
}

void find_executables()
{
  if (bin_path)
  {
    char filepath[PATH_MAX];
    sprintf( filepath, "%s%s%s", bin_path, PATH_CHAR, BARDIC_FILENAME );
    if ( !exists(filepath) ) error( BARDIC_FILENAME " not found on executable path." );
    bardic_path = new_string(filepath);
  }
  else
  {
    bardic_path  = new_string(BARDIC_FILENAME);
  }
}

void check_build_etc()
{
  // If we're running a custom etc (as a Bard VM more than as a build system),
  // skip the whole build.bard check.
  if (etc_name) return;

  if (!exists("build_core.bard") && !exists("build.bard"))
  {
    error( "No build file found.\nType \"gogo -create\" to create a build.bard starter file." );
  }

  if (!exists("build.bard"))
  {
    FILE* fp = fopen("build.bard","wb");

    fprintf( fp, "[include \"build_core.bard\"]\n" );
    fprintf( fp, "\n" );
    fprintf( fp, "class CustomBuild : BuildCore\n" );
    fprintf( fp, "  METHODS\n" );
    fprintf( fp, "    method build( String cmd ):\n" );
    fprintf( fp, "      prior.build(cmd)\n" );
    fprintf( fp, "endClass\n" );
    fclose(fp);
  }

  compile_buildfile();
}

void compile_buildfile()
{
  char cmd[PATH_MAX];
  sprintf( cmd, "%s build.bard -include gogo.bard -quiet", bardic_path );
  if (bin_path && !src_path) 
  {
    src_path = new char[strlen(bin_path)*2 + 64];
    sprintf( src_path, 
      "%s" PATH_CHAR "libraries;%s" PATH_CHAR "libraries" PATH_CHAR "standard",
      bin_path, bin_path );
  }
  if (src_path)
  {
    sprintf( cmd + strlen(cmd), " -src_path \"%s\"", src_path );
  }
  //printf( "\n> %s\n", cmd );  // DEBUG
  if (0 != system(cmd))
  {
    error( "BUILD FAILED\n" );
  }
  //printf( "\n" );
}

void bard_adjust_filename_for_os( char* filename, int len );

void bard_adjust_filename_for_os( char* filename, int len )
{
}

void Bitmap__init__ArrayList_of_Byte()
{
  // Bitmap::init(Byte[])
  BardArrayList* list = (BardArrayList*) BARD_POP_REF();
  BardObject* bitmap_obj = BARD_POP_REF();

  NativeLayer_init_bitmap( bitmap_obj, (char*) list->array->data, list->count );
}

void Bitmap__rotate_right()
{
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();

  BardInt32 w = bitmap_obj->width;
  BardInt32 h = bitmap_obj->height;

  BardInt32* rotate_buffer = new BardInt32[w*h];
  BardInt32* dest_start = rotate_buffer + h - 1;
  int di = h;
  int dj = -1;

  BardInt32* src = (BardInt32*) (bitmap_obj->pixels->data);
  --src;  // prepare for preincrement ahead
  for (int j=h; j>0; --j)
  {
    BardInt32* dest = dest_start;
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
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();

  BardInt32 w = bitmap_obj->width;
  BardInt32 h = bitmap_obj->height;

  BardInt32* rotate_buffer = new BardInt32[w*h];
  BardInt32* dest_start = rotate_buffer + w*h - h;
  int di = -h;
  int dj = 1;

  BardInt32* src = (BardInt32*) (bitmap_obj->pixels->data);
  --src;  // prepare for preincrement ahead
  for (int j=h; j>0; --j)
  {
    BardInt32* dest = dest_start;
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
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  BardInt32 w = bitmap_obj->width;
  BardInt32 h = bitmap_obj->height;
  BardInt32* src  = (BardInt32*) (bitmap_obj->pixels->data);
  BardInt32* dest = src + w*h;
  --src;

  int count = (w*h/2) + 1;
  while (--count)
  {
    BardInt32 c = *(++src);
    *src = *(--dest);
    *dest = c;
  }
}

void Bitmap__flip_horizontal()
{
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();

  BardInt32 w = bitmap_obj->width;
  BardInt32 h = bitmap_obj->height;

  BardInt32* src_start = (BardInt32*) (bitmap_obj->pixels->data);

  int j = h + 1;
  while (--j)
  {
    BardInt32* src = src_start - 1;
    BardInt32* dest = src_start + w;
    int count = (w>>1) + 1;
    while (--count)
    {
      BardInt32 c = *(++src);
      *src = *(--dest);
      *dest = c;
    }
    src_start += w;
  }
}

void Bitmap__flip_vertical()
{
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();

  BardInt32 w = bitmap_obj->width;
  BardInt32 h = bitmap_obj->height;

  BardInt32* src  = ((BardInt32*) (bitmap_obj->pixels->data)) - 1;
  BardInt32* dest = ((BardInt32*) (bitmap_obj->pixels->data)) + w*h - w - 1;

  int j = (h>>1) + 1;
  while (--j)
  {
    int i = w + 1;
    while (--i)
    {
      BardInt32 c = *(++src);
      *src = *(++dest);
      *dest = c;
    }
    dest -= (w + w);
  }
}

void Bitmap__resize_horizontal__Int32()
{
  BardInt32 new_w = (BardInt32) BARD_POP_INTEGER();
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();

  BardInt32 w = bitmap_obj->width;
  BardInt32 h = bitmap_obj->height;

  if (w == new_w) return;
  if (new_w <= 0) return;

  BardArray* original_data = bitmap_obj->pixels;
  while (new_w <= w/2 && (w&1)==0)
  {
    // average every two horizontal pixels to speed up the process
    int count = w*h / 2 + 1;
    BardInt32* src  = ((BardInt32*) (original_data->data)) - 1;
    BardInt32* dest  = src;
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

  BARD_PUSH_REF( (BardObject*) original_data );
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // to retrieve after possible gc
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // for init() call
  BARD_PUSH_INTEGER( new_w );
  BARD_PUSH_INTEGER( h );
  BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
  bitmap_obj = (BardBitmap*) BARD_POP_REF();  // recover ref after possible gc
  original_data = (BardArray*) BARD_POP_REF();

  BardArray* new_data = bitmap_obj->pixels;

  BardInt32* src  = ((BardInt32*) (original_data->data)) - 1;
  BardInt32* dest = ((BardInt32*) (new_data->data)) - 1;

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
  BardInt32 new_h = (BardInt32) BARD_POP_INTEGER();
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();

  BardInt32 w = bitmap_obj->width;
  BardInt32 h = bitmap_obj->height;

  if (h == new_h) return;
  if (new_h <= 0) return;

  BardArray* original_data = bitmap_obj->pixels;
  while (new_h <= h/2 && (h&1)==0)
  {
    // average every two vertical pixels to speed up the process
    BardInt32* src  = ((BardInt32*) (original_data->data)) - 1;
    BardInt32* dest = ((BardInt32*) (original_data->data)) - 1;
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

  BARD_PUSH_REF( (BardObject*) original_data );
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // to retrieve after possible gc
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // for init() call
  BARD_PUSH_INTEGER( w );
  BARD_PUSH_INTEGER( new_h );
  BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
  bitmap_obj = (BardBitmap*) BARD_POP_REF();  // recover ref after possible gc
  original_data = (BardArray*) BARD_POP_REF();

  BardArray* new_data = bitmap_obj->pixels;

  BardInt32* src_start  = ((BardInt32*) (original_data->data));
  BardInt32* dest_start = ((BardInt32*) (new_data->data));

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
    BardInt32* src  = src_start;
    BardInt32* dest = dest_start;
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
  BardInt32 blend_alpha = (BardInt32) BARD_POP_INTEGER();
  BardInt32 dest_y = (BardInt32) BARD_POP_INTEGER();
  BardInt32 dest_x = (BardInt32) BARD_POP_INTEGER();
  BardObject* dest_obj = BARD_POP_REF();
  BardInt32 height = (BardInt32) BARD_POP_INTEGER();
  BardInt32 width  = (BardInt32) BARD_POP_INTEGER();
  BardInt32 src_y  = (BardInt32) BARD_POP_INTEGER();
  BardInt32 src_x  = (BardInt32) BARD_POP_INTEGER();
  BardObject* src_obj = BARD_POP_REF();

  BVM_NULL_CHECK( src_obj,  return );
  BVM_NULL_CHECK( dest_obj, return );

  BARD_GET( BardInt32, src_width, src_obj,   "width" );
  BARD_GET( BardInt32, dest_width, dest_obj,  "width" );
  BARD_GET( BardArray*, src_array, src_obj,   "data" );
  BARD_GET( BardArray*, dest_array, dest_obj, "data" );
  BardInt32* src_data = (BardInt32*) src_array->data;
  BardInt32* dest_data = (BardInt32*) dest_array->data;

  BardInt32 dest_skip_width = dest_width - width;
  BardInt32 src_skip_width  = src_width - width;

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
        *dest_data = (BardInt32) (0xff000000 | (tr<<16) | (tg<<8) | tb);
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

  BARD_PUSH_REF( bard_create_byte_list( bytes, size ) );
  gdFree( bytes );
}

void Bitmap__to_jpg_bytes__Real64()
{
  double compression = BARD_POP_REAL();
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
  BARD_PUSH_REF( bard_create_byte_list( bytes, size ) );
  gdFree( bytes );
}


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
    BARD_PUSH_INTEGER( width );
    BARD_PUSH_INTEGER( height );
    BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
    BARD_GET_REF( array_obj, BARD_PEEK_REF(), "data" );
    BardArray* array = (BardArray*) array_obj;
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

void ZipArchive__decompress__Int32()
{
  // ZipArchive::decompress(Int32).Byte[]
  BardInt32 index = (BardInt32) BARD_POP_INTEGER();
  BardObject* archive_obj = BARD_POP_REF();
  BVM_NULL_CHECK( archive_obj, bvm.type_null_reference_error );

  BARD_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  BVM_NULL_CHECK( archive_filename_obj, bvm.type_null_reference_error );

  char archive_filename[PATH_MAX];
  ((BardString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

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

        BardInt64 ztime = file_info.tmu_date.tm_year;  // year eg 2009
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mon + 1);  // month 1..12
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mday);  // day 1..31
        ztime = (ztime * 100) + (file_info.tmu_date.tm_hour);  // hour 0..23
        ztime = (ztime * 100) + (file_info.tmu_date.tm_min);   // minute 0..59
        ztime = (ztime * 100) + (file_info.tmu_date.tm_sec);   // second 0..59

        BARD_FIND_TYPE( type_byte_array, "Array<<Byte>>" );
        BardArray*    bytes = type_byte_array->create( uncompressed_count );
        BARD_PUSH_REF( bytes );

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
  bard_throw_file_error( archive_filename );
  return;
}


void ZipArchive__load_entries()
{
  BardObject* archive_obj = BARD_PEEK_REF();
  BVM_NULL_CHECK( archive_obj, bvm.type_null_reference_error );

  BARD_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  BVM_NULL_CHECK( archive_filename_obj, bvm.type_null_reference_error );

  char archive_filename[PATH_MAX];
  ((BardString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

  unzFile zfp = unzOpen( archive_filename );
  if ( !zfp ) 
  {
    zfp = zipOpen( archive_filename, APPEND_STATUS_CREATE );
    zipClose( zfp, NULL );
    BARD_POP_REF();
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
      BardInt64 ztime = file_info.tmu_date.tm_year;  // year eg 2009
      ztime = (ztime * 100) + (file_info.tmu_date.tm_mon + 1);  // month 1..12
      ztime = (ztime * 100) + (file_info.tmu_date.tm_mday);  // day 1..31
      ztime = (ztime * 100) + (file_info.tmu_date.tm_hour);  // hour 0..23
      ztime = (ztime * 100) + (file_info.tmu_date.tm_min);   // minute 0..59
      ztime = (ztime * 100) + (file_info.tmu_date.tm_sec);   // second 0..59

      BARD_PUSH_REF( archive_obj );
      BARD_PUSH_INTEGER( index );
      BARD_PUSH_REF( BardString::create(current_filename) );
      BARD_PUSH_INTEGER( ztime );  // timestamp
      BARD_CALL( archive_obj->type, "add_entry(Int32,String,Int64)" );

      status = unzGoToNextFile(zfp);
    }
    ++index;
  }
  unzClose(zfp);

  BARD_POP_REF();  // archive_obj
}

void ZipArchive__load_raw__Int32()
{
  // ZipArchive::load_raw(Int32).RawZipData
  BardInt32 index = (BardInt32) BARD_POP_INTEGER();
  BardObject* archive_obj = BARD_POP_REF();
  BVM_NULL_CHECK( archive_obj, bvm.type_null_reference_error );

  BARD_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  BVM_NULL_CHECK( archive_filename_obj, bvm.type_null_reference_error );

  char archive_filename[PATH_MAX];
  ((BardString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

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

        BardInt64 ztime = file_info.tmu_date.tm_year;  // year eg 2009
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mon + 1);  // month 1..12
        ztime = (ztime * 100) + (file_info.tmu_date.tm_mday);  // day 1..31
        ztime = (ztime * 100) + (file_info.tmu_date.tm_hour);  // hour 0..23
        ztime = (ztime * 100) + (file_info.tmu_date.tm_min);   // minute 0..59
        ztime = (ztime * 100) + (file_info.tmu_date.tm_sec);   // second 0..59

        int compression_method, compression_level;

        unzOpenCurrentFile2( zfp, &compression_method, &compression_level, 1 );

        BARD_FIND_TYPE( type_raw_zip_data, "RawZipData" );
        BARD_PUSH_REF( type_raw_zip_data->create() );
        BARD_DUPLICATE_REF();

        BARD_FIND_TYPE( type_byte_array, "Array<<Byte>>" );
        BardArray* array = type_byte_array->create(compressed_count);
        BARD_PUSH_REF( array );
        char* data = (char*) array->data;

        unzReadCurrentFile( zfp, data, compressed_count );

        BARD_PUSH_INTEGER( uncompressed_count );
        BARD_PUSH_INTEGER( ztime );
        BARD_PUSH_INTEGER( compression_method );
        BARD_PUSH_INTEGER( compression_level );
        BARD_PUSH_INTEGER( crc32 );

        BARD_CALL( type_raw_zip_data, "init(Array<<Byte>>,Int32,Int64,Int32,Int32,Int32)" )
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
  bard_throw_file_error( archive_filename );
  return;
}

void ZipArchive__add__String_Int64_ArrayList_of_Byte_Int32_Int32_Logical_Int32_Int32()
{
  BardInt32 crc32 = (BardInt32) BARD_POP_INTEGER();
  BardInt32 uncompressed_count = (BardInt32) BARD_POP_INTEGER();
  BardInt32 raw = (BardInt32) BARD_POP_INTEGER();

  BardInt32 compression_level = (BardInt32) BARD_POP_INTEGER();
  if (compression_level < 0) compression_level = 0;
  else if (compression_level > 9) compression_level = 9;

  BardInt32 compression_method = (BardInt32) BARD_POP_INTEGER();
  if (compression_method != 0) compression_method = Z_DEFLATED;

  BardArrayList* byte_list = (BardArrayList*) BARD_POP_REF();
  BardInt64 ztime = BARD_POP_INTEGER();
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BardObject* archive_obj = BARD_PEEK_REF();

  BVM_NULL_CHECK( archive_obj, bvm.type_null_reference_error );
  BVM_NULL_CHECK( filename_obj, bvm.type_null_reference_error );
  BVM_NULL_CHECK( byte_list, bvm.type_null_reference_error );

  BARD_GET_REF( archive_filename_obj, archive_obj, "archive_filename" );
  BVM_NULL_CHECK( archive_filename_obj, bvm.type_null_reference_error );
  char archive_filename[PATH_MAX];
  ((BardString*)archive_filename_obj)->to_ascii( archive_filename, PATH_MAX );

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
  BardByte* data = (BardByte*) byte_list->array->data;

  zipFile zfp = zipOpen( archive_filename, APPEND_STATUS_ADDINZIP );
  if ( !zfp ) zfp = zipOpen( archive_filename, APPEND_STATUS_CREATE );

  if ( !zfp )
  {
printf("error 3\n");
    bard_throw_file_error( archive_filename );
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
    bard_throw_file_error( archive_filename );
    return;
  }

  BARD_POP_REF();
}

