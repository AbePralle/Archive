//=============================================================================
// plasmacore.cpp
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

#include <string.h>
#include "unzip.h"
#include "plasmacore.h"

//=============================================================================
// GLOBALS
//=============================================================================
Plasmacore plasmacore;

bool mouse_visible = true;
bool log_drawing = false;

#ifdef ANDROID
extern char* android_etc_data;
extern int   android_etc_size;
#endif


#if defined(ANDROID)
void log(const char*);
#endif

void plasmacore_load_settings();
void plasmacore_register_native_methods();


//=============================================================================
//  TouchManager
//=============================================================================
int TouchManager::begin_touch( double x, double y )
{
  if (num_active_touches < PLASMACORE_MAX_TOUCHES)
  {
    for (int i=0; i<PLASMACORE_MAX_TOUCHES; ++i)
    {
      if ( !touches[i].active )
      {
        touches[i].active = true;
        touches[i].position.x = x;
        touches[i].position.y = y;
        ++num_active_touches;
        return i+1;
      }
    }
  }
  return 0;
}

int TouchManager::update_touch( double x, double y )
{
  int best_i = -1;
  double dx,dy;
  double best_r2 = 0;
  for (int i=0; i<PLASMACORE_MAX_TOUCHES; i++)
  {
    if (touches[i].active)
    {
      dx = touches[i].position.x - x;
      dy = touches[i].position.y - y;
      double r2 = dx*dx + dy*dy;
      if (best_i == -1 || r2 < best_r2)
      {
        best_r2 = r2;
        best_i = i;
      }
    }
  }

  if (best_i == -1)
  {
    printf( "ERROR: update_touch with no active touches!\n" );
    return 0;
  }

  touches[best_i].position.x = x;
  touches[best_i].position.y = y;
  return best_i+1;
}

int TouchManager::end_touch( double x, double y )
{
  int i = update_touch(x,y);
  touches[i-1].active = 0;
  --num_active_touches;
  return i;
}


//=============================================================================
//  Archive
//=============================================================================
char* Archive::load( BardString* filename, int* size )
{
  if (filename == NULL) return NULL;

  char buffer[PATH_MAX];
  filename->to_ascii( buffer, PATH_MAX );
  return load( buffer, size );
}

#ifndef ANDROID
char* Archive::load( const char* filename, int *size )
{
  if (strncmp(filename,"internal:",9) == 0)
  {
    filename += 9;
    if (0 == strcmp(filename,"font_system_17"))
    {
      *size = embedded_font_system_17_size;
      char* buffer = (char*) new char[ embedded_font_system_17_size ];
      memcpy( buffer, embedded_font_system_17, embedded_font_system_17_size );
      return buffer;
    }
  }

  unzFile zfp = unzOpen( archive_filename );
  if ( !zfp ) return NULL;

  int  filename_len = strlen(filename);
  char first_char = filename[0];

  int err = UNZ_OK;
  err = unzGoToFirstFile(zfp);
  while (UNZ_OK == err)
  {
    unz_file_info file_info;
    char current_filename[256+1];
    err = unzGetCurrentFileInfo( zfp, &file_info, current_filename, 256, NULL, 0, NULL, 0 );
    if (UNZ_OK == err)
    {
      int cur_len = strlen(current_filename);
      if (filename_len <= cur_len)
      {
        // do a substring match
        int diff = cur_len - filename_len;
        for (int i=0; i<=diff; i++)
        {
          if (first_char == current_filename[i] 
              && 0 == strncmp(filename,current_filename+i,filename_len))
          {
            int prev_char;
            if (i > 0) prev_char = current_filename[i-1];
            else       prev_char = 0;
            int next_char = current_filename[i+filename_len];

            if ( !plasmacore_is_id(prev_char) && (next_char==0 || next_char=='.') )
            {
              // found our match
              if (UNZ_OK == unzOpenCurrentFile(zfp))
              {
                unzGetCurrentFileInfo( zfp, &file_info, 0, 0, 0, 0, 0, 0 );

                int data_size = file_info.uncompressed_size;
                if (size) *size = data_size;

                char* buffer = (char*) new char[ data_size ];
                unzReadCurrentFile( zfp, buffer, data_size );
                unzClose(zfp);

                return buffer;
              }
            }
          }
        }
      }

      err = unzGoToNextFile(zfp);
    }
  }
  unzClose(zfp);

  return NULL;
}

#endif // ANDROID


void plasmacore_init()
{
  // Set up the Bard VM
  LOG( "Plasmacore initializing" );
  atexit( plasmacore_shut_down );
  bard_init();
}

void plasmacore_shut_down()
{
  LOG( "Plasmacore shutting down" );
  if (plasmacore.event_suspend)
  {
    plasmacore_queue_signal( plasmacore.event_suspend );
    plasmacore_queue_signal( plasmacore.event_shut_down );
    plasmacore_raise_pending_signals();
    NativeLayer_shut_down();
  }
  bard_shut_down();
}

void plasmacore_queue_signal( BardObject* type )
{
  BARD_FIND_TYPE( type_SignalManager, "SignalManager" );
  BARD_PUSH_REF( type_SignalManager->singleton() );
  BARD_PUSH_REF( type );
  BARD_PUSH_REF( NULL );
  BARD_CALL( type_SignalManager, "queue_native(String,Object)" );
}

void plasmacore_queue_signal( BardObject* type, BardObject* arg )
{
  BARD_FIND_TYPE( type_SignalManager, "SignalManager" );
  BARD_PUSH_REF( type_SignalManager->singleton() );
  BARD_PUSH_REF( type );
  BARD_PUSH_REF( arg );
  BARD_CALL( type_SignalManager, "queue_native(String,Object)" );
}

void plasmacore_queue_signal( const char* type, const char* arg )
{
  BARD_FIND_TYPE( type_SignalManager, "SignalManager" );
  BARD_PUSH_REF( type_SignalManager->singleton() );
  BARD_PUSH_REF( BardString::create(type) );
  BARD_PUSH_REF( BardString::create(arg) );
  BARD_CALL( type_SignalManager, "queue_native(String,Object)" );
}

void plasmacore_queue_signal( BardObject* type, BardObject* object, double x, double y )
{
  BARD_FIND_TYPE( type_SignalManager, "SignalManager" );
  BARD_PUSH_REF( type_SignalManager->singleton() );
  BARD_PUSH_REF( type );

  {
    BARD_FIND_TYPE( type_SignalObjectArg, "SignalObjectArg" );
    BARD_PUSH_REF( type_SignalObjectArg->create() );
    BARD_DUPLICATE_REF();

    BARD_PUSH_REF( object );
    BARD_PUSH_REAL64( x );
    BARD_PUSH_REAL64( y );
    BARD_CALL( type_SignalObjectArg, "init(Object,Real64,Real64)" );
  }

  BARD_CALL( type_SignalManager, "queue_native(String,Object)" );
}

void plasmacore_queue_signal( BardObject* type, int id, int index, bool flag,
    double x, double y )
{
  BARD_FIND_TYPE( type_SignalManager, "SignalManager" );
  BARD_PUSH_REF( type_SignalManager->singleton() );
  BARD_PUSH_REF( type );

  {
    BARD_FIND_TYPE( type_SignalDataArg, "SignalDataArg" );
    BARD_PUSH_REF( type_SignalDataArg->create() );
    BARD_DUPLICATE_REF();

    BARD_PUSH_INT32( id );
    BARD_PUSH_INT32( index );
    BARD_PUSH_LOGICAL( flag );
    BARD_PUSH_REAL64( x );
    BARD_PUSH_REAL64( y );
    BARD_CALL( type_SignalDataArg, "init(Int32,Int32,Logical,Real64,Real64)" );
  }

  BARD_CALL( type_SignalManager, "queue_native(String,Object)" );
}


void plasmacore_configure( int default_display_width, int default_display_height, 
    bool force_default_size, bool allow_new_orientation )
{
  char logmesg[100];
  sprintf( logmesg, "Plasmacore configuring with default display size %dx%d", 
           default_display_width, default_display_height );
  LOG( logmesg );

  plasmacore.set_defaults();

  // If the size is still zero later on then we'll set it to the default.
  plasmacore.display_width = 0;
  plasmacore.display_height = 0;

  plasmacore_register_native_methods();

  plasmacore_load_settings();
  if (force_default_size) plasmacore.display_width = 0;

  plasmacore.original_orientation = plasmacore.orientation;
  if (allow_new_orientation)
  {
    if (plasmacore.orientation == 1 || plasmacore.orientation == 3)
    {
      int temp = default_display_width;
      default_display_width = default_display_height;
      default_display_height = temp;
    }
  }
  else
  {
    plasmacore.orientation = 0;
  }

  if ( !plasmacore.display_width )
  {
    plasmacore.display_width = default_display_width;
    plasmacore.display_height = default_display_height;
  }
}

void log( const char* st );

void plasmacore_raise_pending_signals()
{
  mm.check_gc();
  BARD_FIND_TYPE( type_SignalManager, "SignalManager" );
  BARD_PUSH_REF( type_SignalManager->singleton() );
  BARD_CALL( type_SignalManager, "raise_pending()" );
  BARD_POP_LOGICAL();
}

void plasmacore_raise_signal( const char* type )
{
  plasmacore_queue_signal( BardString::create(type) );
  plasmacore_raise_pending_signals();
}

void plasmacore_raise_signal( const char* type, const char* value )
{
  BardString* type_obj  = BardString::create( type );
  BardString* value_obj = BardString::create( value );
  plasmacore_queue_signal( type_obj, value_obj );
  plasmacore_raise_pending_signals();
}


void plasmacore_launch()
{
#ifdef BARD_VM
#ifdef ANDROID
  BardLoader loader;
  loader.load( android_etc_data, android_etc_size );
#else
  char buffer[512];
  strcpy(buffer,"game.etc");
  bard_adjust_filename_for_os( buffer, 512 );

  BardLoader loader;
  loader.load( buffer );
#endif //ANDROID
#endif

  bard_configure();

  plasmacore.event_launch = plasmacore_find_event_key( "launch" );
  plasmacore.event_update = plasmacore_find_event_key( "update" );
  plasmacore.event_draw   = plasmacore_find_event_key( "draw" );
  plasmacore.event_key    = plasmacore_find_event_key( "key" );
  plasmacore.event_mouse_move = plasmacore_find_event_key( "mouse_move" );
  plasmacore.event_mouse_button = plasmacore_find_event_key(  "mouse_button" );
  plasmacore.event_mouse_wheel = plasmacore_find_event_key( "mouse_wheel" );
  plasmacore.event_textures_lost = plasmacore_find_event_key( "textures_lost" );
  plasmacore.event_suspend = plasmacore_find_event_key( "suspend" );
  plasmacore.event_resume = plasmacore_find_event_key( "resume" );
  plasmacore.event_shut_down = plasmacore_find_event_key( "shut_down" );

  plasmacore_queue_signal( plasmacore.event_launch, *bard_main_object, 
      plasmacore.display_width, plasmacore.display_height );
  plasmacore_raise_pending_signals();
}

bool plasmacore_update()
{
  // Returns "true" if the screen should be redrawn
  if (plasmacore.updates_per_second == 0.0) return false;

  BardInt64 time_ms = bard_get_time_ms();
  BardInt64 elapsed_ms = (time_ms - plasmacore.last_update_time_ms);
  if (elapsed_ms > 1000 || elapsed_ms == 0) elapsed_ms = 0;
  plasmacore.last_update_time_ms = time_ms;

  plasmacore.time_debt += elapsed_ms / 1000.0;

  if (plasmacore.time_debt > 0.1)
  {
    plasmacore.time_debt = 1.0 / plasmacore.updates_per_second;
  }

  bool draw_allowed = true;
  bool first = true;

  while (first || plasmacore.time_debt >= 1.0 / plasmacore.updates_per_second)
  {
    first = false;
    plasmacore.time_debt -= 1.0 / plasmacore.updates_per_second;
    plasmacore_queue_signal( plasmacore.event_update );
    plasmacore_raise_pending_signals();
  }

  return draw_allowed;
}

void plasmacore_draw()
{
  if (log_drawing) LOG( "+draw()" );

  BardInt64 start_ms = bard_get_time_ms();

  plasmacore_clear_transforms();
  NativeLayer_begin_draw();
  plasmacore_queue_signal( plasmacore.event_draw );
  plasmacore_raise_pending_signals();
  NativeLayer_end_draw();

  plasmacore.last_draw_time_ms = int(bard_get_time_ms() - start_ms);

  if (log_drawing) LOG( "-draw()" );
}

bool plasmacore_is_id( char ch )
{
  if (ch >= 'A' && ch <= 'Z') return true;
  if (ch >= 'a' && ch <= 'z') return true;
  if (ch >= '0' && ch <= '9') return true;
  if (ch == '_' || ch == '.' || ch == '-') return true;
  return false;
}

int plasmacore_argb_to_rgba( int argb )
{
  unsigned char bytes[4];
  bytes[0] = (argb >> 16);
  bytes[1] = (argb >> 8);
  bytes[2] = argb;
  bytes[3] = (argb >> 24);
  return *((BardInt32*)bytes);
}

void plasmacore_swap_red_and_blue( BardInt32* data, int count )
{
  --data;
  ++count;
  while (--count)
  {
    int color = *(++data);
    int ag = color & 0xff00ff00;
    int rb = color & 0x00ff00ff;
    *data = (ag | (rb>>16) | (rb<<16));
  }
}


void plasmacore_on_exit_request()
{
  BARD_FIND_TYPE( type_application, "Application" );
  BARD_PUSH_REF( type_application->singleton() );
  BARD_CALL( type_application, "on_exit_request()" );
}

BardObject* plasmacore_find_event_key( const char* event_type )
{
  BARD_FIND_TYPE( type_event_mgr, "SignalManager" );
  BARD_PUSH_REF( type_event_mgr->singleton() );
  BARD_PUSH_REF( BardString::create(event_type) );
  BARD_CALL( type_event_mgr, "find_signal_id(String)" );
  return BARD_POP_REF();
}

void plasmacore_consume_ws( char*& cur_line, int& count )
{
  while (count && (*cur_line == ' ' || *cur_line == '\t'))
  {
    --count;
    ++cur_line;
  }
}

void plasmacore_consume_eol ( char*& cur_line, int& count )
{
  plasmacore_consume_ws( cur_line, count );
  while (count && *cur_line == '\n')
  {
    --count;
    ++cur_line;
  }
}

void plasmacore_discard_line( char*& cur_line, int& count )
{
  while (count && *cur_line != '\n')
  {
    --count;
    ++cur_line;
  }
  plasmacore_consume_eol(cur_line,count);
}

bool plasmacore_consume_id( const char* st, char*& cur_line, int& count )
{
  plasmacore_consume_ws( cur_line, count );
  int len = strlen(st);
  if (count < len) return false;
  for (int i=0; i<len; i++)
  {
    if (cur_line[i] != st[i]) return false;
  }
  count -= len;
  cur_line += len;
  return true;
}

int plasmacore_read_int( char*& cur_line, int& count )
{
  plasmacore_consume_ws( cur_line, count );
  int result = 0;
  while (count && *cur_line>='0' && *cur_line<='9')
  {
    result = (result * 10) + ((*cur_line) - '0');
    ++cur_line;
    --count;
  }
  return result;
}

void plasmacore_load_settings()
{
  char filename_buffer[256];
  strcpy( filename_buffer, "project.properties" );
  bard_adjust_filename_for_os( filename_buffer, 256 );

  FILE* fp = fopen(filename_buffer,"rb");
  if ( !fp ) return;

  fseek( fp, 0, SEEK_END );
  int size = (int) ftell(fp);
  fseek( fp, 0, SEEK_SET );

  char* buffer = new char[size+1];
  fread( buffer, 1, size, fp );
  buffer[size] = 0;
  fclose(fp);

  char* cur_line = buffer;
  int count = size;
  while (count)
  {
    if (plasmacore_consume_id("DISPLAY_SIZE:",cur_line,count))
    {
      plasmacore.display_width = plasmacore_read_int(cur_line,count);
      plasmacore.display_height = plasmacore_read_int(cur_line,count);
    }
    else if (plasmacore_consume_id("DISPLAY_ORIENTATION:",cur_line,count))
    {
      int orientation = 0;
      if (plasmacore_consume_id("up",cur_line,count)) orientation = 0;
      else if (plasmacore_consume_id("portrait",cur_line,count)) orientation = 0;
      else if (plasmacore_consume_id("0",cur_line,count)) orientation = 0;
      else if (plasmacore_consume_id("right",cur_line,count)) orientation = 1;
      else if (plasmacore_consume_id("landscape",cur_line,count)) orientation = 1;
      else if (plasmacore_consume_id("1",cur_line,count)) orientation = 1;
      else if (plasmacore_consume_id("down",cur_line,count)) orientation = 2;
      else if (plasmacore_consume_id("2",cur_line,count)) orientation = 2;
      else if (plasmacore_consume_id("left",cur_line,count)) orientation = 3;
      else if (plasmacore_consume_id("3",cur_line,count)) orientation = 3;
      plasmacore.orientation = orientation;
    }
    else plasmacore_discard_line(cur_line,count);
  }

  delete buffer;
}



void plasmacore_clear_transforms()
{
  /*
  plasmacore.camera_transform_stack_count = 0;
  plasmacore.object_transform_stack_count = 0;
  plasmacore.camera_transform_stack_modified = true;
  plasmacore.object_transform_stack_modified = true;
  */
}


void Vector2::transform()
{
  Vector2 new_pos = plasmacore.view_transform.transform( *this );
  x = new_pos.x;
  y = new_pos.y;
}

#define MCOMBINE(j,i) r##j##c1 * m2.r1c##i  + r##j##c2 * m2.r2c##i + r##j##c3 * m2.r3c##i + r##j##c4 * m2.r4c##i


void Matrix4::print()
{
  printf( "%.4lf %.4lf %.4lf %.4lf\n", r1c1, r1c2, r1c3, r1c4 );
  printf( "%.4lf %.4lf %.4lf %.4lf\n", r2c1, r2c2, r2c3, r2c4 );
  printf( "%.4lf %.4lf %.4lf %.4lf\n", r3c1, r3c2, r3c3, r3c4 );
  printf( "%.4lf %.4lf %.4lf %.4lf\n", r4c1, r4c2, r4c3, r4c4 );
}

Matrix4 Matrix4::operator *( Matrix4 m2 )
{
  return Matrix4(
      MCOMBINE(1,1), MCOMBINE(1,2), MCOMBINE(1,3), MCOMBINE(1,4),
      MCOMBINE(2,1), MCOMBINE(2,2), MCOMBINE(2,3), MCOMBINE(2,4),
      MCOMBINE(3,1), MCOMBINE(3,2), MCOMBINE(3,3), MCOMBINE(3,4),
      MCOMBINE(4,1), MCOMBINE(4,2), MCOMBINE(4,3), MCOMBINE(4,4)
    );
}

Matrix4 Matrix4::transposed()
{
  return Matrix4(
      r1c1, r2c1, r3c1, r4c1,
      r1c2, r2c2, r3c2, r4c2,
      r1c3, r2c3, r3c3, r4c3,
      r1c4, r2c4, r3c4, r4c4
    );
}

double Matrix4::determinant()
{
  // Note: "missing" spots are r4c4 = 1
  return
    r1c4*r2c3*r3c2*r4c1 - r1c3*r2c4*r3c2*r4c1 - r1c4*r2c2*r3c3*r4c1 + r1c2*r2c4*r3c3*r4c1 +
    r1c3*r2c2*r3c4*r4c1 - r1c2*r2c3*r3c4*r4c1 - r1c4*r2c3*r3c1*r4c2 + r1c3*r2c4*r3c1*r4c2 +
    r1c4*r2c1*r3c3*r4c2 - r1c1*r2c4*r3c3*r4c2 - r1c3*r2c1*r3c4*r4c2 + r1c1*r2c3*r3c4*r4c2 +
    r1c4*r2c2*r3c1*r4c3 - r1c2*r2c4*r3c1*r4c3 - r1c4*r2c1*r3c2*r4c3 + r1c1*r2c4*r3c2*r4c3 +
    r1c2*r2c1*r3c4*r4c3 - r1c1*r2c2*r3c4*r4c3 - r1c3*r2c2*r3c1      + r1c2*r2c3*r3c1      +
    r1c3*r2c1*r3c2      - r1c1*r2c3*r3c2      - r1c2*r2c1*r3c3      + r1c1*r2c2*r3c3;
}

Matrix4 Matrix4::inverse()
{
  // Note: "missing" spots are r4c4 = 1
  double m11 = r2c3*r3c4*r4c2 - r2c4*r3c3*r4c2 + r2c4*r3c2*r4c3 - r2c2*r3c4*r4c3 - r2c3*r3c2      + r2c2*r3c3;
  double m12 = r1c4*r3c3*r4c2 - r1c3*r3c4*r4c2 - r1c4*r3c2*r4c3 + r1c2*r3c4*r4c3 + r1c3*r3c2      - r1c2*r3c3;
  double m13 = r1c3*r2c4*r4c2 - r1c4*r2c3*r4c2 + r1c4*r2c2*r4c3 - r1c2*r2c4*r4c3 - r1c3*r2c2      + r1c2*r2c3;
  double m14 = r1c4*r2c3*r3c2 - r1c3*r2c4*r3c2 - r1c4*r2c2*r3c3 + r1c2*r2c4*r3c3 + r1c3*r2c2*r3c4 - r1c2*r2c3*r3c4;
  double m21 = r2c4*r3c3*r4c1 - r2c3*r3c4*r4c1 - r2c4*r3c1*r4c3 + r2c1*r3c4*r4c3 + r2c3*r3c1      - r2c1*r3c3;
  double m22 = r1c3*r3c4*r4c1 - r1c4*r3c3*r4c1 + r1c4*r3c1*r4c3 - r1c1*r3c4*r4c3 - r1c3*r3c1      + r1c1*r3c3;
  double m23 = r1c4*r2c3*r4c1 - r1c3*r2c4*r4c1 - r1c4*r2c1*r4c3 + r1c1*r2c4*r4c3 + r1c3*r2c1      - r1c1*r2c3;
  double m24 = r1c3*r2c4*r3c1 - r1c4*r2c3*r3c1 + r1c4*r2c1*r3c3 - r1c1*r2c4*r3c3 - r1c3*r2c1*r3c4 + r1c1*r2c3*r3c4;
  double m31 = r2c2*r3c4*r4c1 - r2c4*r3c2*r4c1 + r2c4*r3c1*r4c2 - r2c1*r3c4*r4c2 - r2c2*r3c1      + r2c1*r3c2;
  double m32 = r1c4*r3c2*r4c1 - r1c2*r3c4*r4c1 - r1c4*r3c1*r4c2 + r1c1*r3c4*r4c2 + r1c2*r3c1      - r1c1*r3c2;
  double m33 = r1c2*r2c4*r4c1 - r1c4*r2c2*r4c1 + r1c4*r2c1*r4c2 - r1c1*r2c4*r4c2 - r1c2*r2c1      + r1c1*r2c2;
  double m34 = r1c4*r2c2*r3c1 - r1c2*r2c4*r3c1 - r1c4*r2c1*r3c2 + r1c1*r2c4*r3c2 + r1c2*r2c1*r3c4 - r1c1*r2c2*r3c4;
  double m41 = r2c3*r3c2*r4c1 - r2c2*r3c3*r4c1 - r2c3*r3c1*r4c2 + r2c1*r3c3*r4c2 + r2c2*r3c1*r4c3 - r2c1*r3c2*r4c3;
  double m42 = r1c2*r3c3*r4c1 - r1c3*r3c2*r4c1 + r1c3*r3c1*r4c2 - r1c1*r3c3*r4c2 - r1c2*r3c1*r4c3 + r1c1*r3c2*r4c3;
  double m43 = r1c3*r2c2*r4c1 - r1c2*r2c3*r4c1 - r1c3*r2c1*r4c2 + r1c1*r2c3*r4c2 + r1c2*r2c1*r4c3 - r1c1*r2c2*r4c3;
  double m44 = r1c2*r2c3*r3c1 - r1c3*r2c2*r3c1 + r1c3*r2c1*r3c2 - r1c1*r2c3*r3c2 - r1c2*r2c1*r3c3 + r1c1*r2c2*r3c3;

  double s = 1.0 / determinant();

  return Matrix4( 
      m11*s, m12*s, m13*s, m14*s,
      m21*s, m22*s, m23*s, m24*s,
      m31*s, m32*s, m33*s, m34*s,
      m41*s, m42*s, m43*s, m44*s );
}

/*
NOT FULLY WORKING - DOESN'T DO ROW EXCHANGES
Matrix4 Matrix4::inverse()
{
  //  Using Gauss/Jordan-ish variant as described here:
  //    http://math.uww.edu/~mcfarlat/inverse.htm
  // 
  //  Given original 4x4 matrix:
  //    a1 b1 c1 d1
  //    e1 f1 g1 h1
  //    i1 j1 k1 l1
  //    m1 n1 o1 p1
  // 
  //  Join on a 4x4 identity to create a 4x8 matrix:
  //    a1 b1 c1 d1   1  0  0  0
  //    e1 f1 g1 h1   0  1  0  0
  //    i1 j1 k1 l1   0  0  1  0
  //    m1 n1 o1 p1   0  0  0  1
  // 
  //  AKA:
  //    a1 b1 c1 d1  a2 b2 c2 d2    (row 1)
  //    e1 f1 g1 h1  e2 f2 g2 h2    (row 2)
  //    i1 j1 k1 l1  i2 j2 k2 l2    (row 3)
  //    m1 n1 o1 p1  m2 n2 o2 p2    (row 4)
  // 
  double a1=r1c1, b1=r1c2, c1=r1c3, d1=r1c4, a2=1, b2=0, c2=0, d2=0;
  double e1=r2c1, f1=r2c2, g1=r2c3, h1=r2c4, e2=0, f2=1, g2=0, h2=0;
  double i1=r3c1, j1=r3c2, k1=r3c3, l1=r3c4, i2=0, j2=0, k2=1, l2=0;
  double m1=r4c1, n1=r4c2, o1=r4c3, p1=r4c4, m2=0, n2=0, o2=0, p2=1;

  // The following series of transformations will gradually change the
  // original left-hand side into the identity matrix while the RHS
  // becomes the inverse of the original.  Since we use each column
  // from the LHS only once and since we discard the LHS at the end
  // we omit the step of actually setting those values to 0's and 1's
  // as a small optimization.

// {
// LOG("Initial Matrix:");
// char buffer[512];
// sprintf( buffer, "%.4lf %.4lf %.4lf %.4lf\n%.4lf %.4lf %.4lf %.4lf\n%.4lf %.4lf %.4lf %.4lf\n%.4lf %.4lf %.4lf %.4lf\n",
//     a1, b1, c1, d1,
//     e1, f1, g1, h1,
//     i1, j1, k1, l1,
//     m1, n1, o1, p1 );
// LOG(buffer);
// }

  // ----COLUMN 1----
  // Find x such that e1+(a1*x) = 0  ->  x = -e1/a1 and set row2 = row2 + row1*x
  double x = -e1/a1;
  f1+=x*b1; g1+=x*c1; h1+=x*d1; e2+=x*a2; f2+=x*b2; g2+=x*c2; h2+=x*d2;
  // e1 = 0;

  // Find x such that i1+(a1*x) = 0  ->  x = -i1/a1 and set row3 = row3 + row1*x
  x = -i1/a1;
  j1+=x*b1; k1+=x*c1; l1+=x*d1; i2+=x*a2; j2+=x*b2; k2+=x*c2; l2+=x*d2;
  // i1 = 0;

  // Find x such that m1+(a1*x) = 0  ->  x = -m1/a1 and set row4 = row4 + row1*x
  x = -m1/a1;
  n1+=x*b1; o1+=x*c1; p1+=x*d1; m2+=x*a2; n2+=x*b2; o2+=x*c2; p2+=x*d2;
  // m1 = 0;

  // Divide row 1 by a1 so that a1 becomes 1
  b1 /= a1; c1 /= a1; d1 /= a1;
  a2/=a1; b2/=a1; c2/=a1; c2/=a1;
  // a1 = 1;

  //  We now have:
  //     1 b1 c1 d1  a2 b2 c2 d2    (row 1)
  //     0 f1 g1 h1  e2 f2 g2 h2    (row 2)
  //     0 j1 k1 l1  i2 j2 k2 l2    (row 3)
  //     0 n1 o1 p1  m2 n2 o2 p2    (row 4)
  //

  // ----COLUMN 2----
  // Find x such that b1+(f1*x) = 0  ->  x = -b1/f1 and set row1 = row1 + row2*x
  x = -b1/f1;
  c1+=x*g1; d1+=x*h1; a2+=x*e2; b2+=x*f2; c2+=x*g2; d2+=x*h2;
  // b1 = 0;

  // Find x such that j1+(f1*x) = 0  ->  x = -j1/f1 and set row3 = row3 + row2*x
  x = -j1/f1;
  k1+=x*g1; l1+=x*h1; i2+=x*e2; j2+=x*f2; k2+=x*g2; l2+=x*h2;
  // j1 = 0;

  // Find x such that n1+(f1*x) = 0  ->  x = -n1/f1 and set row4 = row4 + row2*x
  x = -n1/f1;
  o1+=x*g1; p1+=x*h1; m2+=x*e2; n2+=x*f2; o2+=x*g2; p2+=x*h2;
  // n1 = 0;

  // Divide row 2 by f1 so that f1 becomes 1
  g1 /= f1; h1 /= f1;
  e2/=f1; f2/=f1; g2/=f1; h2/=f1;
  // f1 = 1;

  //  We now have:
  //     1  0 c1 d1  a2 b2 c2 d2    (row 1)
  //     0  1 g1 h1  e2 f2 g2 h2    (row 2)
  //     0  0 k1 l1  i2 j2 k2 l2    (row 3)
  //     0  0 o1 p1  m2 n2 o2 p2    (row 4)

  // ----COLUMN 3----
  // Find x such that c1+(k1*x) = 0  ->  x = -c1/k1 and set row1 = row1 + row3*x
  x = -c1/k1;
  d1+=x*l1; a2+=x*i2; b2+=x*j2; c2+=x*k2; d2+=x*l2;
  // c1 = 0;

  // Find x such that g1+(k1*x) = 0  ->  x = -g1/k1 and set row2 = row2 + row3*x
  x = -g1/k1;
  h1+=x*l1; e2+=x*i2; f2+=x*j2; g2+=x*k2; h2+=x*l2;
  // g1 = 0;

  // Find x such that o1+(k1*x) = 0  ->  x = -o1/k1 and set row4 = row4 + row3*x
  x = -o1/k1;
  p1+=x*l1; m2+=x*i2; n2+=x*j2; o2+=x*k2; p2+=x*l2;
  // o1 = 0;

  // Divide row 3 by k1 so that k1 becomes 1
  l1 /= k1;
  i2/=k1; j2/=k1; k2/=k1; l2/=k1;
  // k1 = 1;

  //  We now have:
  //     1  0  0 d1  a2 b2 c2 d2    (row 1)
  //     0  1  0 h1  e2 f2 g2 h2    (row 2)
  //     0  0  1 l1  i2 j2 k2 l2    (row 3)
  //     0  0  0 p1  m2 n2 o2 p2    (row 4)

  // ----COLUMN 4----
  // Find x such that d1+(p1*x) = 0  ->  x = -d1/p1 and set row1 = row1 + row4*x
  x = -d1/p1;
  a2+=x*m2; b2+=x*n2; c2+=x*o2; d2+=x*p2;
  // d1 = 0;

  // Find x such that h1+(p1*x) = 0  ->  x = -h1/p1 and set row2 = row2 + row4*x
  x = -h1/p1;
  e2+=x*m2; f2+=x*n2; g2+=x*o2; h2+=x*p2;
  // h1 = 0;

  // Find x such that l1+(p1*x) = 0  ->  x = -l1/p1 and set row3 = row3 + row4*x
  x = -l1/p1;
  i2+=x*m2; j2+=x*n2; k2+=x*o2; l2+=x*p2;
  // l1 = 0;

  // Divide row 4 by p1 so that p1 becomes 1
  m2/=p1; n2/=p1; o2/=p1; p2/=p1;
  // p1 = 1;

  //  We now have:
  //     1  0  0  0  a2 b2 c2 d2    (row 1)
  //     0  1  0  0  e2 f2 g2 h2    (row 2)
  //     0  0  1  0  i2 j2 k2 l2    (row 3)
  //     0  0  0  1  m2 n2 o2 p2    (row 4)

  // a2..p2 is the inverse (if an inverse exists)
  return Matrix4(
    a2, b2, c2, d2,
    e2, f2, g2, h2,
    i2, j2, k2, l2,
    m2, n2, o2, p2
  );
}
*/


Vector2 Matrix4::transform( Vector2 v )
{
  return Vector2( 
      v.x * r1c1 + v.y * r1c2 + r4c1, 
      v.x * r2c1 + v.y * r2c2 + r4c2
  );
}

Vector3 Matrix4::transform( Vector3 v )
{
  return Vector3( 
      v.x * r1c1 + v.y * r1c2 + v.z * r1c3 + r4c1, 
      v.x * r2c1 + v.y * r2c2 + v.z * r2c3 + r4c2,
      v.x * r3c1 + v.y * r3c2 + v.z * r3c3 + r4c3
    );
}

/*
void Matrix4::to_transposed_doubles( double* data )
{
  BardReal64* src = ((BardReal64*) this);
  data[0] =  src[0];
  data[1] =  src[4];
  data[2] =  src[8];
  data[3] =  src[12];
  data[4] =  src[1];
  data[5] =  src[5];
  data[6] =  src[9];
  data[7] =  src[13];
  data[8] =  src[2];
  data[9] =  src[6];
  data[10] = src[10];
  data[11] = src[14];
  data[12] = src[3];
  data[13] = src[7];
  data[14] = src[11];
  data[15] = src[15];
}

void Matrix4::to_transposed_floats( float* data )
{
  BardReal64* src = ((BardReal64*) this);
  data[0] =  (float) src[0];
  data[1] =  (float) src[4];
  data[2] =  (float) src[8];
  data[3] =  (float) src[12];
  data[4] =  (float) src[1];
  data[5] =  (float) src[5];
  data[6] =  (float) src[9];
  data[7] =  (float) src[13];
  data[8] =  (float) src[2];
  data[9] =  (float) src[6];
  data[10] = (float) src[10];
  data[11] = (float) src[14];
  data[12] = (float) src[3];
  data[13] = (float) src[7];
  data[14] = (float) src[11];
  data[15] = (float) src[15];
}
*/

void Matrix4::to_floats( float* data )
{
  BardReal64* src = ((BardReal64*) this);
  data[0] =  (float) src[0];
  data[1] =  (float) src[1];
  data[2] =  (float) src[2];
  data[3] =  (float) src[3];
  data[4] =  (float) src[4];
  data[5] =  (float) src[5];
  data[6] =  (float) src[6];
  data[7] =  (float) src[7];
  data[8] =  (float) src[8];
  data[9] =  (float) src[9];
  data[10] = (float) src[10];
  data[11] = (float) src[11];
  data[12] = (float) src[12];
  data[13] = (float) src[13];
  data[14] = (float) src[14];
  data[15] = (float) src[15];
}

void Matrix4::to_fixed( BardInt32* data )
{
  BardReal64* src = ((BardReal64*) this);
  data[0]  = (BardInt32) (src[0] * 65536);
  data[1]  = (BardInt32) (src[1] * 65536);
  data[2]  = (BardInt32) (src[2] * 65536);
  data[3]  = (BardInt32) (src[3] * 65536);
  data[4]  = (BardInt32) (src[4] * 65536);
  data[5]  = (BardInt32) (src[5] * 65536);
  data[6]  = (BardInt32) (src[6] * 65536);
  data[7]  = (BardInt32) (src[7] * 65536);
  data[8]  = (BardInt32) (src[8] * 65536);
  data[9]  = (BardInt32) (src[9] * 65536);
  data[10] = (BardInt32) (src[10] * 65536);
  data[11] = (BardInt32) (src[11] * 65536);
  data[12] = (BardInt32) (src[12] * 65536);
  data[13] = (BardInt32) (src[13] * 65536);
  data[14] = (BardInt32) (src[14] * 65536);
  data[15] = (BardInt32) (src[15] * 65536);
}

#if !defined(ANDROID)
void Application__native_set_menu_options__ArrayList_of_String()
{
  BARD_DISCARD_REF(); 
  BARD_DISCARD_REF(); 
  // no action
}
#endif

extern Archive image_archive;
void NativeLayer_init_bitmap( BardObject* bitmap_obj, char* raw_data, int data_size );

void Bitmap__init__ArrayList_of_Byte()
{
  // Bitmap::init(Byte[])
  BardArrayList* list = (BardArrayList*) BARD_POP_REF();
  BardObject* bitmap_obj = BARD_POP_REF();

  NativeLayer_init_bitmap( bitmap_obj, (char*)(list->array->data), list->count );
}

void Bitmap__init__String()
{
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BardObject* bitmap_obj = BARD_POP_REF();
  int data_size;
  char* data = image_archive.load( filename_obj, &data_size );

  if (data) 
  {
    NativeLayer_init_bitmap( bitmap_obj, data, data_size );
    delete data;
  }
  else
  {
    char buffer[256];
    filename_obj->to_ascii( buffer, 256 );
    bard_throw_file_not_found_error( buffer );
  }
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
  BardInt32 new_w = BARD_POP_INT32();
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
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // for init() call
  BARD_PUSH_INT32( new_w );
  BARD_PUSH_INT32( h );
  BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
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
  BardInt32 new_h = BARD_POP_INT32();
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
  BARD_PUSH_REF( (BardObject*) bitmap_obj );  // for init() call
  BARD_PUSH_INT32( w );
  BARD_PUSH_INT32( new_h );
  BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
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
  BardInt32 blend_alpha = BARD_POP_INT32();
  BardInt32 dest_y = BARD_POP_INT32();
  BardInt32 dest_x = BARD_POP_INT32();
  BardObject* dest_obj = BARD_POP_REF();
  BardInt32 height = BARD_POP_INT32();
  BardInt32 width  = BARD_POP_INT32();
  BardInt32 src_y  = BARD_POP_INT32();
  BardInt32 src_x  = BARD_POP_INT32();
  BardObject* src_obj = BARD_POP_REF();

  BVM_NULL_CHECK( src_obj, return );
  BVM_NULL_CHECK( dest_obj, return );

  BARD_GET_INT32( src_width, src_obj,   "width" );
  BARD_GET_INT32( dest_width, dest_obj, "width" );
  BARD_GET_REF( src_array,  src_obj,  "data" );
  BARD_GET_REF( dest_array, dest_obj, "data" );
  BardInt32* src_data = (BardInt32*) ((BardArray*)src_array)->data;
  BardInt32* dest_data = (BardInt32*) ((BardArray*)dest_array)->data;

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

void Display__last_draw_time_ms()
{
  BARD_DISCARD_REF();  // singleton
  BARD_PUSH_INT32( plasmacore.last_draw_time_ms );
}

void Display__orientation()
{
  BARD_DISCARD_REF();  // singleton
  BARD_PUSH_INT32( plasmacore.orientation );
}

#if !defined(ANDROID) && TARGET_OS_IPHONE == 0
void Input__keyboard_visible__Logical()
{
  BARD_POP_INT32();
  BARD_DISCARD_REF();  // discard singleton
  // no action
}

void Input__keyboard_visible()
{
  BARD_DISCARD_REF();  // discard singleton
  BARD_PUSH_INT32(0);
}
#endif

void Matrix4Manager__inverse__Matrix4()
{
  Matrix4 m = BARD_POP( Matrix4 );
  BARD_DISCARD_REF();  // singleton
  BARD_PUSH( Matrix4, m.inverse() );
}

void Matrix4Manager__transform__Matrix4_Vector2()
{
  Vector2 v = BARD_POP( Vector2 );
  Matrix4 m = BARD_POP( Matrix4 );
  BARD_DISCARD_REF();  // singleton
  BARD_PUSH( Vector2, m.transform(v) );
}

void Matrix4Manager__transform__Matrix4_Vector3()
{
  Vector3 v = BARD_POP( Vector3 );
  Matrix4 m = BARD_POP( Matrix4 );
  BARD_DISCARD_REF();  // singleton
  BARD_PUSH( Vector3, m.transform(v) );
}


extern Archive data_archive;

void ResourceManager__load_data_file__String()
{
  // ResourceManager::load_data_file(String).String
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();   // discard singleton

  BARD_PUSH_REF( (BardObject*) filename_obj );

  int count;
  char* archive_data = data_archive.load( filename_obj, &count );

  filename_obj = (BardString*) BARD_POP_REF();

  if (archive_data)
  {
    BARD_PUSH_REF( BardString::create( archive_data, count ) );
    delete archive_data;
  }
  else
  {
    char buffer[256];
    buffer[0]='!';
    filename_obj->to_ascii(buffer+1,255);
    bard_throw_file_not_found_error( buffer );
  }
}

#ifndef ANDROID
void ResourceManager__load_gamestate__String()
{
  // ResourceManager::load_gamestate(String).String
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();   // discard singleton

  char buffer[128];
  strcpy( buffer, "save/" );
  filename_obj->to_ascii( buffer+5, 128-5 );

  FILE* fp = fopen( buffer, "rb" );
  if (fp)
  {
    fseek( fp, 0, SEEK_END );
    int count = ftell(fp);
    fseek( fp, 0, SEEK_SET );

    BardString* content = BardString::create( count );
    BARD_PUSH_REF( content );

    BardChar* data = content->characters;
    --data;
    ++count;
    while (--count) *(++data) = (BardChar) getc(fp);

    fclose(fp);

    content->set_hash_code();
  }
  else
  {
    bard_throw_file_not_found_error( buffer );
  }
}


void ResourceManager__save_gamestate__String_String()
{
  // ResourceManager::save_gamestate(String,String)
  BardString* content = (BardString*) BARD_POP_REF();
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();   // discard singleton

  char buffer[128];
  strcpy( buffer, "save/" );
  filename_obj->to_ascii( buffer+5, 128-5 );

  FILE* fp = fopen( buffer, "wb" );
  if (fp)
  {
    BardChar* data = content->characters - 1;
    int count = content->count + 1;
    while (--count) putc( *(++data), fp );
    fclose(fp);
  }
  else
  {
    bard_throw_file_not_found_error( buffer );
    return;
  }
}

void ResourceManager__delete_gamestate__String()
{
  BardString* filename_obj = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();   // discard singleton

  char filepath[128];
  strcpy( filepath, "save/" );
  filename_obj->to_ascii( filepath+5, 128-5 );

#if defined(_WIN32)

  DeleteFile(filepath) || RemoveDirectory(filepath);
 
#elif defined(UNIX)
  remove(filepath);
#else
# error ResourceManager__delete_gamestate__String for this OS.
#endif
}
#endif //ANDROID

#if TARGET_OS_IPHONE == 0 && !defined(ANDROID)
void System__device_id()
{
  BARD_PEEK_REF() = NULL;
}

#endif // not TARGET_OS_IPHONE

#if TARGET_OS_IPHONE == 0 && !defined(ANDROID)
void System__open_url__String()
{
  BARD_DISCARD_REF();
  BARD_DISCARD_REF();
  // no action
}
#endif

void System__target_fps__Int32()
{
  plasmacore.target_fps = BARD_POP_INT32();
  BARD_DISCARD_REF();
}

void SystemMonitor__log_drawing__Logical()
{
  log_drawing = BARD_POP_INT32() > 0;
  BARD_DISCARD_REF(); /* singleton context */
}

void Texture__native_release()
{
  // Texture::native_release()
  BardObject* texture_obj = BARD_POP_REF();
  BVM_NULL_CHECK( texture_obj, return );

  BARD_GET_REF( native_data, texture_obj, "native_data" );
  if ( !native_data ) return;
  BARD_SET_REF( texture_obj, "native_data", NULL );

  ((BardNativeData*)native_data)->release();
}

void Texture__set__Bitmap()
{
  BARD_PUSH_INT32( 0 );  // dummy value for pixel format suggestion
  Texture__init__Bitmap_Int32();
}

void Transform__create__Vector2_Vector2_Radians_Vector2_Vector2_Logical_Logical()
{
  int vflip = BARD_POP_INT32();
  int hflip = BARD_POP_INT32();
  Vector2 pos   = BARD_POP(Vector2);
  Vector2 scale = BARD_POP(Vector2);
  BardReal64 angle = BARD_POP_REAL64();
  Vector2 handle = BARD_POP(Vector2);
  Vector2 size   = BARD_POP(Vector2);
  BARD_DISCARD_REF();

  int hflip_sign = 1;
  int vflip_sign = 1;

  if (hflip || vflip)
  {
    handle.x -= size.x / 2.0;
    handle.y -= size.y / 2.0;
    if (hflip) hflip_sign = -1;
    if (vflip) vflip_sign = -1;
  }

  double cost = cos(angle);
  double sint = sin(angle);

  double r1c1 = cost * scale.x * hflip_sign;
  double r1c2 = -sint * scale.y * vflip_sign;
  double r1c4 = pos.x - scale.x*handle.x*cost + sint*scale.y*handle.y;

  double r2c1 = sint * scale.x * hflip_sign;
  double r2c2 = cost * scale.y * vflip_sign;
  double r2c4 = pos.y - scale.x*handle.x*sint - cost*scale.y*handle.y;

  Matrix4 m( r1c1, r1c2,    0, r1c4,
             r2c1, r2c2,    0, r2c4,
                0,    0,    1,    0,
                0,    0,    0,    1
          );
  if (hflip || vflip)
  {
    // translate by -size/2
    Matrix4 t( 1, 0, 0, -size.x/2,
               0, 1, 0, -size.y/2,
               0, 0, 1,         0,
               0, 0, 0,         1 );
    m = m * t;
  }

  BARD_PUSH( Matrix4, m );
}


void ProjectionTransform__native_projection_transform__Matrix4()
{
  plasmacore.projection_transform = BARD_POP( Matrix4 );
  BARD_DISCARD_REF();
  NativeLayer_set_projection_transform();
}

void ViewTransform__native_view_transform__Matrix4()
{
  plasmacore.view_transform = BARD_POP( Matrix4 );

  BARD_DISCARD_REF();
  NativeLayer_set_view_transform();
}


void plasmacore_register_native_methods()
{
  bard_register_native_method( "Application","log(String)", Application__log__String );
  bard_register_native_method( "Application","title(String)",Application__title__String);

  bard_register_native_method("Bitmap","init(ArrayList<<Byte>>)", Bitmap__init__ArrayList_of_Byte );
  bard_register_native_method("Bitmap","init(String)", Bitmap__init__String );
  bard_register_native_method("Bitmap","to_png_bytes()", Bitmap__to_png_bytes );
  bard_register_native_method("Bitmap","to_jpg_bytes(Real64)", Bitmap__to_jpg_bytes__Real64 );

  bard_register_native_method( "Bitmap","copy_pixels_to(Int32,Int32,Int32,Int32,Bitmap,Int32,Int32,Logical)",
      Bitmap__copy_pixels_to__Int32_Int32_Int32_Int32_Bitmap_Int32_Int32_Logical );

  bard_register_native_method("Bitmap", "rotate_right()",      Bitmap__rotate_right );
  bard_register_native_method("Bitmap", "rotate_left()",       Bitmap__rotate_left );
  bard_register_native_method("Bitmap", "rotate_180()",        Bitmap__rotate_180 );
  bard_register_native_method("Bitmap", "flip_horizontal()",   Bitmap__flip_horizontal );
  bard_register_native_method("Bitmap", "flip_vertical()",     Bitmap__flip_vertical );
  bard_register_native_method("Bitmap", "resize_horizontal(Int32)", Bitmap__resize_horizontal__Int32 );
  bard_register_native_method("Bitmap", "resize_vertical(Int32)", Bitmap__resize_vertical__Int32 );

  bard_register_native_method("Display","flush()", Display__flush );
  bard_register_native_method("Display","fullscreen()",Display__fullscreen);
  bard_register_native_method("Display","fullscreen(Logical)",Display__fullscreen__Logical);
  bard_register_native_method("Display","last_draw_time_ms()", Display__last_draw_time_ms );
  bard_register_native_method("Display","native_set_clipping_region(Box)",
    Display__native_set_clipping_region__Box );
  bard_register_native_method("Display","orientation()", Display__orientation );
  bard_register_native_method("Display","screen_shot(Bitmap)",Display__screen_shot__Bitmap);

  bard_register_native_method("Display","native_set_draw_target(OffscreenBuffer,Logical)",
      Display__native_set_draw_target__OffscreenBuffer_Logical);


  bard_register_native_method("Input","mouse_position(Vector2)",Input__mouse_position__Vector2);
  bard_register_native_method("Input","mouse_visible(Logical)",Input__mouse_visible__Logical);
  bard_register_native_method("Input","keyboard_visible(Logical)",Input__keyboard_visible__Logical);
  bard_register_native_method("Input","keyboard_visible()",Input__keyboard_visible);
  bard_register_native_method("Input","input_capture(Logical)",Input__input_capture__Logical);

  bard_register_native_method("LineManager","draw(Line,Color,Render)",LineManager__draw__Line_Color_Render);

  bard_register_native_method("Matrix4Manager","inverse(Matrix4)", Matrix4Manager__inverse__Matrix4 );
  bard_register_native_method("Matrix4Manager","transform(Matrix4,Vector2)", 
      Matrix4Manager__transform__Matrix4_Vector2 );
  bard_register_native_method("Matrix4Manager","transform(Matrix4,Vector3)", 
      Matrix4Manager__transform__Matrix4_Vector3 );

  bard_register_native_method("Model","native_init(Int32,Int32,Int32)", Model__native_init__Int32_Int32_Int32 );
  bard_register_native_method("Model","define_triangles(Array<<Char>>)",     Model__define_triangles__Array_of_Char );
  bard_register_native_method("Model","define_frame(Int32,Array<<Vertex>>)", Model__define_frame__Int32_Array_of_Vertex );
  bard_register_native_method("Model","native_draw(Texture)",                Model__native_draw__Texture );

  bard_register_native_method("NativeSound","init(String)", NativeSound__init__String );
  bard_register_native_method("NativeSound","init(ArrayList<<Byte>>)", NativeSound__init__ArrayList_of_Byte);
  bard_register_native_method("NativeSound","create_duplicate()",NativeSound__create_duplicate);
  bard_register_native_method("NativeSound","play()",NativeSound__play);
  bard_register_native_method("NativeSound","pause()",NativeSound__pause);
  bard_register_native_method("NativeSound","is_playing()",NativeSound__is_playing);
  bard_register_native_method("NativeSound","volume(Real64)",NativeSound__volume__Real64);
  bard_register_native_method("NativeSound","pan(Real64)",NativeSound__pan__Real64);
  bard_register_native_method("NativeSound","pitch(Real64)",NativeSound__pitch__Real64);
  bard_register_native_method("NativeSound","repeats(Logical)",NativeSound__repeats__Logical);
  bard_register_native_method("NativeSound","current_time()",NativeSound__current_time);
  bard_register_native_method("NativeSound","current_time(Real64)",NativeSound__current_time__Real64);
  bard_register_native_method("NativeSound","duration()",NativeSound__duration);

  bard_register_native_method( "OffscreenBuffer","clear(Color)",OffscreenBuffer__clear__Color );

  bard_register_native_method("QuadManager","fill(Quad,ColorGradient,Render)",
      QuadManager__fill__Quad_ColorGradient_Render );

  bard_register_native_method("ResourceManager","load_data_file(String)", 
      ResourceManager__load_data_file__String );

  bard_register_native_method("ResourceManager","load_gamestate(String)", 
      ResourceManager__load_gamestate__String );
  bard_register_native_method("ResourceManager","save_gamestate(String,String)", 
      ResourceManager__save_gamestate__String_String );
  bard_register_native_method("ResourceManager","delete_gamestate(String)", 
      ResourceManager__delete_gamestate__String );

  bard_register_native_method( "Scene3D","native_begin_draw()", Scene3D__native_begin_draw );
  bard_register_native_method( "Scene3D","native_end_draw()", Scene3D__native_end_draw );

  bard_register_native_method( "System","device_id()",System__device_id);
  bard_register_native_method( "System","max_texture_size()", System__max_texture_size );
  bard_register_native_method( "System","open_url(String)", System__open_url__String );
  bard_register_native_method( "System","country_name()", System__country_name );
  bard_register_native_method( "System","target_fps(Int32)", System__target_fps__Int32 );

  bard_register_native_method( "SystemMonitor","log_drawing(Logical)", SystemMonitor__log_drawing__Logical );

  bard_register_native_method("Texture","init(Bitmap,Int32)",Texture__init__Bitmap_Int32);
  bard_register_native_method("Texture","init(String,Int32)",Texture__init__String_Int32);
  bard_register_native_method("Texture","init(Vector2,Int32)",Texture__init__Vector2_Int32 );
  bard_register_native_method("Texture","init(Vector2)",      Texture__init__Vector2 );
  bard_register_native_method("Texture","native_release()",Texture__native_release);
  bard_register_native_method("Texture","draw(Corners,Vector2,Color,Render,Blend)",Texture__draw__Corners_Vector2_Color_Render_Blend);
  bard_register_native_method("Texture","draw(Corners,Vector2,Color,Render,Blend,Texture,Corners)",
      Texture__draw__Corners_Vector2_Color_Render_Blend_Texture_Corners );
  bard_register_native_method("Texture","draw(Corners,Quad,ColorGradient,Render,Blend)",Texture__draw__Corners_Quad_ColorGradient_Render_Blend);
  bard_register_native_method("Texture","draw(Vector2,Vector2,Vector2,Triangle,Color,Color,Color,Render,Blend)",
      Texture__draw__Vector2_Vector2_Vector2_Triangle_Color_Color_Color_Render_Blend );
  bard_register_native_method("Texture","draw_tile(Corners,Vector2,Vector2,Int32)",
      Texture__draw_tile__Corners_Vector2_Vector2_Int32 );
  bard_register_native_method("Texture","set(Bitmap)",Texture__set__Bitmap);
  bard_register_native_method("Texture","set(Bitmap,Vector2)",Texture__set__Bitmap_Vector2);

  bard_register_native_method( "Transform", "create(Vector2,Vector2,Radians,Vector2,Vector2,Logical,Logical)", 
      Transform__create__Vector2_Vector2_Radians_Vector2_Vector2_Logical_Logical );
  bard_register_native_method( "ProjectionTransform", "native_projection_transform(Matrix4)", 
      ProjectionTransform__native_projection_transform__Matrix4 );
  bard_register_native_method( "ViewTransform", "native_view_transform(Matrix4)", ViewTransform__native_view_transform__Matrix4 );

  /*
  bard_register_native_method("TransformManager","current()", TransformManager__current );
  bard_register_native_method("TransformManager","inverse(Transform)",      
      TransformManager__inverse__Transform );
  bard_register_native_method("TransformManager","op*(Transform,Transform)",      
      TransformManager__opMUL__Transform_Transform);

  bard_register_native_method( "TransformManager","push_object_transform(Transform)", 
      TransformManager__push_object_transform__Transform );
  bard_register_native_method( "TransformManager","pop_object_transform()", 
      TransformManager__pop_object_transform );
  bard_register_native_method( "TransformManager","push_camera_transform(Transform)", 
      TransformManager__push_camera_transform__Transform );
  bard_register_native_method( "TransformManager","pop_camera_transform()", 
      TransformManager__pop_camera_transform );
  */

  bard_register_native_method("TriangleManager","fill(Triangle,Color,Color,Color,Render)",
      TriangleManager__fill__Triangle_Color_Color_Color_Render );

  bard_register_native_method("Vector2Manager","draw(Vector2,Color,Render)",Vector2Manager__draw__Vector2_Color_Render);

  NativeLayer_configure_graphics();
}

