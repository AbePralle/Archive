//=============================================================================
// android_core.cpp
//
// $(PLASMACORE_VERSION)
//
// -----------------------------------------------------------------------------
//
// Copyright 2010-2011 Plasmaworks LLC
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
//=============================================================================

#include <jni.h>
#include <string.h>
#include <fcntl.h>

#include "android_core.h"
#include "png.h"

void perform_custom_setup();  // defined in custom.cpp

void Android__is_tablet();
void Android__memory_class();
void Android__app_version();

void Network__is_connected();
void Network__is_mobile();
void Network__mac_address();

void VideoPlayer__play__String();
void VideoPlayer__update__NativeData();

void WebView__view__URL();
void WebView__view__String();
void WebView__close();
void WebView__bounds__Box();
void WebView__visible__Logical();
void WebView__visible();
void WebView__loaded();
void WebView__failed();

#define NATIVE_DEF(m) Java_com_$(DEVELOPER_PACKAGE_ID)_$(PROJECT_PACKAGE_ID)_Plasmacore_##m

JavaVM* jvm = 0;
jobject plasmacore_obj = 0;
jclass  class_CustomPlasmacore = 0;
char*   android_etc_data=0;
int     android_etc_size=0;
BardGlobalRef android_setup_ref;

bool plasmacore_initialized = false;

Archive data_archive( DATA_ARCHIVE );
Archive image_archive( IMAGES_ARCHIVE );

jmethodID m_jniLog = 0;
jmethodID m_jniAndroidMemoryClass = 0;
jmethodID m_jniAndroidIsTablet    = 0;
jmethodID m_jniAndroidAppVersion  = 0;
jmethodID m_jniExitProgram = 0;

jmethodID m_jniDecodeBitmapData = 0;
jmethodID m_jniEncodeBitmapData = 0;

jmethodID m_jniGetCountryName = 0;
jmethodID m_jniGetDeviceID = 0;
jmethodID m_jniGetDeviceType = 0;
jmethodID m_jniGetLanguage = 0;
jmethodID m_jniOpenURL = 0;

jmethodID m_jniLoadResource = 0;
jmethodID m_jniIsDirectory = 0;
jmethodID m_jniFileExists = 0;
jmethodID m_jniDirectoryListing = 0;
jmethodID m_jniAbsoluteFilePath = 0;
jmethodID m_jniFileCopy = 0;
jmethodID m_jniFileRename = 0;
jmethodID m_jniFileDelete = 0;
jmethodID m_jniFileTimestamp = 0;
jmethodID m_jniFileTouch = 0;
jmethodID m_jniFileMkdir = 0;
jmethodID m_jniFileReaderOpen = 0;
jmethodID m_jniFileReaderClose = 0;
jmethodID m_jniFileReaderReadBytes = 0;
jmethodID m_jniFileReaderAvailable = 0;
jmethodID m_jniGetIOBuffer = 0;
jmethodID m_jniFileWriterOpen = 0;
jmethodID m_jniFileWriterClose = 0;
jmethodID m_jniFileWriterWriteBytes = 0;

jmethodID m_jniLoadGamestate = 0;
jmethodID m_jniSaveGamestate = 0;
jmethodID m_jniDeleteGamestate = 0;

jmethodID m_jniNetworkIsConnected = 0;
jmethodID m_jniNetworkIsMobile = 0;
jmethodID m_jniNetworkMacAddress = 0;

jmethodID m_jniSHA1HMAC = 0;

jmethodID m_jniSoundLoad = 0;
jmethodID m_jniSoundDuplicate = 0;
jmethodID m_jniSoundPlay = 0;
jmethodID m_jniSoundPause = 0;
jmethodID m_jniSoundIsPlaying = 0;
jmethodID m_jniSoundSetVolume = 0;
jmethodID m_jniSoundSetRepeats = 0;
jmethodID m_jniSoundGetCurrentTime = 0;
jmethodID m_jniSoundSetCurrentTime = 0;
jmethodID m_jniSoundDuration = 0;
jmethodID m_jniSoundRelease = 0;

jmethodID m_jniShowKeyboard = 0;
jmethodID m_jniKeyboardVisible = 0;

jmethodID m_jniVideoPlay = 0;
jmethodID m_jniVideoUpdate = 0;
jmethodID m_jniVideoStop = 0;

jmethodID m_jniWebViewGet = 0;
jmethodID m_jniWebViewURL = 0;
jmethodID m_jniWebViewHTML = 0;
jmethodID m_jniWebViewClose = 0;
jmethodID m_jniWebViewSetBounds = 0;
jmethodID m_jniWebViewSetVisible = 0;
jmethodID m_jniWebViewGetVisible = 0;
jmethodID m_jniWebViewGetLoaded = 0;
jmethodID m_jniWebViewGetFailed = 0;


//-----------------------------------------------------------------------------

void LOG( const char* st )
{
  GET_JAVA_ENV();
  jobject mesg_obj = java_env->NewStringUTF(st);
  java_env->CallVoidMethod( plasmacore_obj, m_jniLog, mesg_obj );
  java_env->DeleteLocalRef( mesg_obj );
}

void LOG( BardString* string_obj )
{
  int count = string_obj->count;
  if (count >= 512)
  {
    char* buffer = string_obj->to_new_ascii();
    LOG( buffer );
    delete buffer;
  }
  else
  {
    char buffer[512];
    string_obj->to_ascii( buffer, 512 );
    LOG( buffer );
  }
}

void GLTexture::on_textures_lost()
{
}

GLTexture::GLTexture( int w, int h, bool offscreen_buffer )
{
  next_texture = all_textures;
  all_textures = this;

  frame_buffer = 0;
  if (offscreen_buffer)
  {
    glGenFramebuffers( 1, &frame_buffer );
    glBindFramebuffer( GL_FRAMEBUFFER, frame_buffer );
  }
  glGenTextures( 1, &id );
  glBindTexture( GL_TEXTURE_2D, id );
  texture_width = texture_height = 0;
  resize( w, h );
}

void GLTexture::destroy()
{
  // unlink from all_textures list
  if (this == all_textures)
  {
    all_textures = this->next_texture;
  }
  else
  {
    GLTexture* prev = all_textures;
    GLTexture* cur =  all_textures->next_texture;
    while (cur != this)
    {
      prev = cur;
      cur = cur->next_texture;
    }
    prev->next_texture = this->next_texture;
  }

  if (id)
  {
    glDeleteTextures( 1, &id );
    if (frame_buffer)
    {
      glDeleteFramebuffers( 1, &frame_buffer );
    }
  }
}

//-----------------------------------------------------------------------------

BardString* to_bard_string( jstring jst )
{
  GET_JAVA_ENV();
  if (jst == NULL) return NULL;

  int count = java_env->GetStringLength(jst);
  const jchar* char_data = java_env->GetStringChars( jst, NULL );

  BardString* result = BardString::create( count );
  memcpy( result->characters, char_data, count*2 );
  result->set_hash_code();

  java_env->ReleaseStringChars( jst, char_data );
  return result;
}

jstring to_jstring( BardObject* string_obj )
{
  GET_JAVA_ENV();
  if (string_obj == NULL) return NULL;

  BardString* str_obj = (BardString*) string_obj;
  int count = str_obj->count;
  return (jstring) java_env->NewString((jchar*)(str_obj->characters),count);
}

void plasmacore_reset_opengl()
{
  draw_buffer.reset();
  shader_list.reset_shaders();

  glViewport( 0, 0, plasmacore.display_width, plasmacore.display_height );
#ifdef OLD_OPENGL_V1
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrthof( 0, plasmacore.display_width, plasmacore.display_height, 0, -1, 1 );
  glMatrixMode(GL_MODELVIEW);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

  // Sets up pointers and enables states needed for using vertex arrays and textures
  glClientActiveTexture(GL_TEXTURE0);
#endif
#ifdef OLD_OPENGL_V1
  glTexCoordPointer( 2, GL_FLOAT, 0, draw_buffer.uv);
  glColorPointer( 4, GL_UNSIGNED_BYTE, 0, draw_buffer.colors);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glClientActiveTexture(GL_TEXTURE1);
  glTexCoordPointer( 2, GL_FLOAT, 0, draw_buffer.alpha_uv);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glClientActiveTexture(GL_TEXTURE0);
#endif
}

//-----------------------------------------------------------------------------
void shut_down_android_plasmacore()
{
  plasmacore_initialized = false;

  plasmacore_queue_signal( plasmacore.event_shut_down );
  plasmacore_raise_pending_signals();
  NativeLayer_shut_down();
  bard_shut_down();
}

extern "C" jboolean NATIVE_DEF(bardCreate)( JNIEnv* env, jobject THIS,
    int screen_width, int screen_height, jbyteArray etc_data )
{
  JNIEnv* java_env = env;
  java_env->GetJavaVM( &jvm );

  plasmacore_obj = THIS;

  // Get a global ref to class Plasmacore for finding Java methods.
  jclass local_class = java_env->FindClass( 
      "com/$(DEVELOPER_PACKAGE_ID)/$(PROJECT_PACKAGE_ID)/CustomPlasmacore" );
  class_CustomPlasmacore = (jclass) java_env->NewGlobalRef( local_class );
  java_env->DeleteLocalRef( local_class );

  // Get info for the Java methods we need.
	m_jniLog = java_env->GetMethodID( class_CustomPlasmacore, "jniLog", "(Ljava/lang/String;)V" );
  m_jniAndroidIsTablet    = java_env->GetMethodID( class_CustomPlasmacore, "jniAndroidIsTablet", "()I" );
  m_jniAndroidMemoryClass = java_env->GetMethodID( class_CustomPlasmacore, "jniAndroidMemoryClass", "()I" );
  m_jniAndroidAppVersion  = java_env->GetMethodID( class_CustomPlasmacore, "jniAndroidAppVersion", "()Ljava/lang/String;" );

  m_jniExitProgram = java_env->GetMethodID( class_CustomPlasmacore, "jniExitProgram", "()V" );

  m_jniDecodeBitmapData = java_env->GetMethodID( class_CustomPlasmacore, "jniDecodeBitmapData", "([B)[I" );
  m_jniEncodeBitmapData = java_env->GetMethodID( class_CustomPlasmacore, "jniEncodeBitmapData", "(II[III)[B" );

  m_jniGetCountryName = java_env->GetMethodID( class_CustomPlasmacore, "jniGetCountryName", "()Ljava/lang/String;" );
  m_jniGetDeviceID = java_env->GetMethodID( class_CustomPlasmacore,    "jniGetDeviceID", "()Ljava/lang/String;" );
  m_jniGetDeviceType = java_env->GetMethodID( class_CustomPlasmacore,    "jniGetDeviceType", "()Ljava/lang/String;" );
  m_jniGetLanguage = java_env->GetMethodID( class_CustomPlasmacore,    "jniGetLanguage", "()Ljava/lang/String;" );
  m_jniOpenURL = java_env->GetMethodID( class_CustomPlasmacore, "jniOpenURL", "(Ljava/lang/String;)V" );

  m_jniLoadResource = java_env->GetMethodID( class_CustomPlasmacore, "jniLoadResource", 
      "(ILjava/lang/String;)[B" );
  m_jniIsDirectory = java_env->GetMethodID(class_CustomPlasmacore, "jniIsDirectory", "(Ljava/lang/String;)Z");
  m_jniFileExists = java_env->GetMethodID(class_CustomPlasmacore, "jniFileExists", "(Ljava/lang/String;)Z");
  m_jniDirectoryListing = java_env->GetMethodID(class_CustomPlasmacore, "jniDirectoryListing", 
      "(Ljava/lang/String;)[Ljava/lang/String;");
  m_jniAbsoluteFilePath = java_env->GetMethodID(class_CustomPlasmacore, "jniAbsoluteFilePath", 
      "(Ljava/lang/String;)Ljava/lang/String;");
  m_jniFileCopy = java_env->GetMethodID(class_CustomPlasmacore, "jniFileCopy", 
      "(Ljava/lang/String;Ljava/lang/String;)V");
  m_jniFileRename = java_env->GetMethodID(class_CustomPlasmacore, "jniFileRename", 
      "(Ljava/lang/String;Ljava/lang/String;)V");
  m_jniFileDelete = java_env->GetMethodID(class_CustomPlasmacore, "jniFileDelete", 
      "(Ljava/lang/String;)V");
  m_jniFileTimestamp = java_env->GetMethodID(class_CustomPlasmacore, "jniFileTimestamp", 
      "(Ljava/lang/String;)J");
  m_jniFileTouch = java_env->GetMethodID(class_CustomPlasmacore, "jniFileTouch", "(Ljava/lang/String;)V");
  m_jniFileMkdir = java_env->GetMethodID(class_CustomPlasmacore, "jniFileMkdir", "(Ljava/lang/String;)V");

  m_jniFileReaderOpen = java_env->GetMethodID(class_CustomPlasmacore,"jniFileReaderOpen",
      "(Ljava/lang/String;)I");
  m_jniFileReaderClose     = java_env->GetMethodID(class_CustomPlasmacore,"jniFileReaderClose","(I)V");
  m_jniFileReaderReadBytes = java_env->GetMethodID(class_CustomPlasmacore,"jniFileReaderReadBytes","(II)[B");
  m_jniFileReaderAvailable = java_env->GetMethodID(class_CustomPlasmacore,"jniFileReaderAvailable","(I)I");

  m_jniGetIOBuffer = java_env->GetMethodID(class_CustomPlasmacore,"jniGetIOBuffer","(I)[B");
  m_jniFileWriterOpen = java_env->GetMethodID(class_CustomPlasmacore,"jniFileWriterOpen",
      "(Ljava/lang/String;Z)I");
  m_jniFileWriterClose     = java_env->GetMethodID(class_CustomPlasmacore,"jniFileWriterClose","(I)V");
  m_jniFileWriterWriteBytes = java_env->GetMethodID(class_CustomPlasmacore,"jniFileWriterWriteBytes",
      "(I[BI)V");

	m_jniLoadGamestate = java_env->GetMethodID( class_CustomPlasmacore, "jniLoadGamestate", "(Ljava/lang/String;)[B" );
	m_jniSaveGamestate = java_env->GetMethodID( class_CustomPlasmacore, "jniSaveGamestate", "(Ljava/lang/String;Ljava/lang/String;)Z" );
	m_jniDeleteGamestate = java_env->GetMethodID( class_CustomPlasmacore, "jniDeleteGamestate", "(Ljava/lang/String;)Z" );
	
	m_jniNetworkIsConnected = java_env->GetMethodID( class_CustomPlasmacore, "jniNetworkIsConnected", "()Z" );
	m_jniNetworkIsMobile = java_env->GetMethodID( class_CustomPlasmacore, "jniNetworkIsMobile", "()Z" );
	m_jniNetworkMacAddress = java_env->GetMethodID( class_CustomPlasmacore, "jniNetworkMacAddress", "()Ljava/lang/String;" );

	m_jniSHA1HMAC = java_env->GetMethodID( class_CustomPlasmacore, "jniSHA1HMAC", 
            "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;" );

	m_jniSoundLoad = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundLoad", "(Ljava/lang/String;)I" );
	m_jniSoundDuplicate = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundDuplicate", "(I)I" );
	m_jniSoundPlay = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundPlay", "(I)V" );
	m_jniSoundPause = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundPause", "(I)V" );
	m_jniSoundIsPlaying = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundIsPlaying", "(I)Z" );
	m_jniSoundSetVolume = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundSetVolume", "(ID)V" );
	m_jniSoundSetRepeats = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundSetRepeats", "(IZ)V" );
	m_jniSoundGetCurrentTime = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundGetCurrentTime", "(I)D" );
	m_jniSoundSetCurrentTime = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundSetCurrentTime", "(ID)V" );
	m_jniSoundDuration = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundDuration", "(I)D" );
	m_jniSoundRelease = java_env->GetMethodID( class_CustomPlasmacore, "jniSoundRelease", "(I)V" );

	m_jniShowKeyboard = java_env->GetMethodID( class_CustomPlasmacore, "jniShowKeyboard", "(Z)V" );
	m_jniKeyboardVisible = java_env->GetMethodID( class_CustomPlasmacore, "jniKeyboardVisible", "()Z" );

  m_jniVideoPlay = java_env->GetMethodID( class_CustomPlasmacore,"jniVideoPlay",
      "(Ljava/lang/String;)I");
  m_jniVideoUpdate = java_env->GetMethodID( class_CustomPlasmacore,"jniVideoUpdate", "(I)Z");
  m_jniVideoStop = java_env->GetMethodID( class_CustomPlasmacore,"jniVideoStop", "(I)V");

  m_jniWebViewGet = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewGet", "(I)I" );
  m_jniWebViewURL = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewURL", "(ILjava/lang/String;)V" );
  m_jniWebViewHTML = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewHTML", "(ILjava/lang/String;)V" );
  m_jniWebViewClose = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewClose", "(I)V" );
  m_jniWebViewSetBounds = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewSetBounds", "(IIIII)V" );
  m_jniWebViewSetVisible = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewSetVisible", "(IZ)V" );
  m_jniWebViewGetVisible = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewGetVisible", "(I)Z" );
  m_jniWebViewGetLoaded = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewGetLoaded", "(I)Z" );
  m_jniWebViewGetFailed = java_env->GetMethodID( class_CustomPlasmacore, "jniWebViewGetFailed", "(I)Z" );

  android_setup_ref = NULL;

  int err = setjmp(bard_fatal_jumppoint);
  if ( !err )
  {
    if (plasmacore_initialized)
    {
      // The app got killed without getting a chance to shut down - go ahead and shut down.
      LOG( "Shutting down previous Plasmacore session before starting new session." );
      shut_down_android_plasmacore();
    }

    plasmacore_initialized = true;

    if (etc_data)
    {
      LOG( "game.etc loaded" );
      JavaByteArray array( etc_data );
      android_etc_data = array.data;
      android_etc_size = array.count;
    }

    plasmacore_init();

    bard_hook_native( "Android", "memory_class()", Android__memory_class );
    bard_hook_native( "Android", "is_tablet()",    Android__is_tablet );
    bard_hook_native( "Android", "app_version()",  Android__app_version );

    bard_hook_native( "VideoPlayer", "play(String)", VideoPlayer__play__String );
    bard_hook_native( "VideoPlayer", "update(NativeData)", VideoPlayer__update__NativeData );

    bard_hook_native( "WebView", "view(URL)",        WebView__view__URL );
    bard_hook_native( "WebView", "view(String)",     WebView__view__String );
    bard_hook_native( "WebView", "close()",          WebView__close );
    bard_hook_native( "WebView", "bounds(Box)",      WebView__bounds__Box );
    bard_hook_native( "WebView", "visible(Logical)", WebView__visible__Logical );
    bard_hook_native( "WebView", "visible()",        WebView__visible );
    bard_hook_native( "WebView", "loaded()",         WebView__loaded );
    bard_hook_native( "WebView", "failed()",         WebView__failed );

    perform_custom_setup();

    plasmacore_configure(screen_width,screen_height,true,false);

    //if (is_fast_hardware) plasmacore.fast_hardware = 1;
    //else plasmacore.fast_hardware = 0;

    plasmacore_configure_graphics();
    plasmacore_reset_opengl();

    plasmacore_launch();
    draw_buffer.render();
  }
  else
  {
    LOG( "------------------FATAL ERROR------------------" );
    LOG( bard_error_message.value );
  }
}

extern "C" jint NATIVE_DEF(bardUpdateDrawEvent)( JNIEnv* env, jobject THIS )
{
  if ( plasmacore_update() ) plasmacore_draw();
  return plasmacore.target_fps;
}

extern "C" void NATIVE_DEF(bardTexturesLostEvent)( JNIEnv* env, jobject THIS )
{
  GLTexture::on_textures_lost();
  plasmacore_reset_opengl();
  plasmacore_queue_signal( plasmacore.event_textures_lost );
  plasmacore_queue_signal( plasmacore.event_resume );
  plasmacore_raise_pending_signals();
}

extern "C" void NATIVE_DEF(bardKeyEvent)( JNIEnv* env, jobject THIS, jboolean is_press, 
    jint code, jboolean is_unicode)
{
  if (is_press) 
  {
    plasmacore_queue_signal( plasmacore.event_key, is_unicode ? 1 : 0, code, true  );
  }
  else
  {
    plasmacore_queue_signal( plasmacore.event_key, is_unicode ? 1 : 0, code, false  );
  }
}

extern "C" void NATIVE_DEF(bardTouchEvent)( JNIEnv* env, jobject THIS, jint stage,
    jint id, jdouble x, jdouble y )
{
  if (id == 0) return;

  if (stage == 0)
  {
    plasmacore_queue_signal( plasmacore.event_mouse_button, id, 1, true, x, y );
  }
  else if (stage == 1)
  {
    plasmacore_queue_signal( plasmacore.event_mouse_move, id, 1, true, x, y );
  }
  else
  {
    plasmacore_queue_signal( plasmacore.event_mouse_button, id, 1, false, x, y );
  }
}

extern "C" void NATIVE_DEF(bardCustomEventWithValue)( JNIEnv* env, jobject THIS, jobject jcustom_id,
    jdouble value, jobject jmessage )
{
  plasmacore_queue_signal( 
      to_bard_string( (jstring) jcustom_id ),
      to_bard_string( (jstring) jmessage ),
      value, 0.0 );
}

extern "C" void NATIVE_DEF(bardCustomEvent)( JNIEnv* env, jobject THIS, jobject jcustom_id,
    jobject jmessage )
{
  plasmacore_queue_signal( 
      to_bard_string( (jstring) jcustom_id ),
      to_bard_string( (jstring) jmessage ) 
    );
}

extern "C" void NATIVE_DEF(bardAccelerationEvent)( JNIEnv* env, jobject THIS,
    jdouble x, jdouble y, jdouble z )
{
  //if (plasmacore.original_orientation == 1)
  //{
    //double temp = x;
    //x = y;
    //y = -temp;
  //}

  BARD_FIND_TYPE( type_input, "Input" );
  BardObject* input_obj = type_input->singleton();
  BARD_SET_REAL64( input_obj, "acceleration_x", x );
  BARD_SET_REAL64( input_obj, "acceleration_y", y );
  BARD_SET_REAL64( input_obj, "acceleration_z", z );
}

extern "C" void NATIVE_DEF(bardRaisePendingSignals)( JNIEnv* env, jobject THIS )
{
  plasmacore_raise_pending_signals(); 
}


extern "C" void NATIVE_DEF(bardOnResourceDownloaderProgress)( JNIEnv* env, jobject THIS, jdouble progress )
{
  if (*android_setup_ref == NULL) return;

  if (progress == -1.0)
  {
    // signal that actual downloading is taking place
    BARD_SET_LOGICAL( *android_setup_ref, "downloading", 1 );
  }
  else
  {
    BARD_SET_REAL64( *android_setup_ref, "progress", progress );
  }
}

extern "C" void NATIVE_DEF(bardOnPause)( JNIEnv* env, jobject THIS )
{
  //plasmacore_on_exit_request();
  plasmacore_queue_signal( plasmacore.event_suspend );
  plasmacore_raise_pending_signals();

  // 1) Droid X doesn't resume when offscreen buffers exist.
  // 2) Textures will need to be recreated on resume.
  //
  // Therefore we delete all textures BEFORE pausing.  We'll give Plasmacore 
  // a notification to recreate them.
  GLTexture* cur = all_textures;
  while (cur)
  {
    glDeleteTextures( 1, &(cur->id) );
    if (cur->frame_buffer) glDeleteFramebuffers( 1, &(cur->frame_buffer) );
    cur->id = 0;
    cur->frame_buffer = 0;
    cur = cur->next_texture;
  }
}

extern "C" void NATIVE_DEF(bardOnShutDown)( JNIEnv* env, jobject THIS )
{
  LOG( "Plasmacore shutting down" );

  if (plasmacore_initialized)
  {
    shut_down_android_plasmacore();
  }
}

//-----------------------------------------------------------------------------

// local helper
void NativeLayer_init_bitmap( BardObject* bitmap_obj, char* raw_data, int data_size )
{
  GET_JAVA_ENV();
  jbyteArray src_obj = java_env->NewByteArray( data_size );
  JavaByteArray src_wrapper( src_obj );
  memcpy( src_wrapper.data, raw_data, data_size );
  src_wrapper.release();
  jintArray decoded_obj = (jintArray) java_env->CallObjectMethod( plasmacore_obj,
      m_jniDecodeBitmapData , src_obj );

  java_env->DeleteLocalRef( src_obj );

  if (decoded_obj)
  {
    JavaIntArray decoded_array( decoded_obj );
    int width  = decoded_array.data[decoded_array.count-1];
    int height = (decoded_array.count - 1) / width;

    // premultiply the alpha and swap red with blue
    int count = (width * height) + 1;
    BardInt32* cur = ((BardInt32*) decoded_array.data);
    while (--count)
    {
      BardInt32 color = *cur;
      int a = (color >> 24) & 255;
      int r = (color >> 16) & 255;
      int g = (color >> 8) & 255;
      int b = color & 255;

      r = (r * a) / 255;
      g = (g * a) / 255;
      b = (b * a) / 255;

      *(cur++) = (a<<24) | (r<<16) | (g<<8) | b;
    }

    BARD_PUSH_REF( bitmap_obj );
    BARD_PUSH_REF( bitmap_obj );
    BARD_PUSH_INT32( width );
    BARD_PUSH_INT32( height );
    BARD_CALL( bitmap_obj->type, "init(Int32,Int32)" );
    BARD_GET( BardArray*, array, bitmap_obj, "data" );
    memcpy( array->data, decoded_array.data, width*height*4 );
		BARD_POP_REF();
  }
  else
  {
    LOG("Bitmap not found\n");
    bard_throw_file_error();
  }
}

void NativeLayer_shut_down()
{
  // TODO
}

void NativeLayer_begin_draw()
{
  // Prepare for drawing.
  glDisable( GL_SCISSOR_TEST );

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

  glEnable( GL_BLEND );

  draw_buffer.set_draw_target( NULL );
}

void NativeLayer_end_draw()
{
  draw_buffer.render();
}


//====================================================================

Archive::Archive( int type )
{
  archive_type = type;
}


char* Archive::load( const char* filename, int *size )
{
  GET_JAVA_ENV();
  if (strncmp(filename,"internal:",9) == 0)
  {
    filename += 9;
    if (0 == strncmp(filename,"font_system_17",14))
    {
      *size = embedded_font_system_17_size;
      char* buffer = (char*) new char[( *size )];
      memcpy( buffer, embedded_font_system_17, *size );
      return buffer;
    }


    return NULL;
  }
	
  jobject filename_obj = java_env->NewStringUTF(filename);
  jbyteArray array_obj = (jbyteArray) java_env->CallObjectMethod( plasmacore_obj, m_jniLoadResource, 
      archive_type, filename_obj );
  java_env->DeleteLocalRef( filename_obj );

  if (array_obj)
  {
		JavaByteArray wrapper = JavaByteArray(array_obj);
		char* data = (char*)new char[(wrapper.count)];
		memcpy(data, wrapper.data, wrapper.count);
		*size = wrapper.count;
		
    return data;
  }
  else
  {
    return NULL;
  }
}

void Android__memory_class()
{
  GET_JAVA_ENV();
  BARD_POP_REF();  // singleton
  static int result = 0;
  if ( !result ) result = java_env->CallIntMethod( plasmacore_obj, m_jniAndroidMemoryClass );
  BARD_PUSH_INT32( result );
}

void Android__is_tablet()
{
  GET_JAVA_ENV();
  BARD_POP_REF();  // singleton
  static int result = 0;
  if ( !result) result = java_env->CallIntMethod( plasmacore_obj, m_jniAndroidIsTablet );
  BARD_PUSH_INT32( result );
}

void Android__app_version()
{
  GET_JAVA_ENV();
  BARD_POP_REF();
  jstring jstr = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniAndroidAppVersion );
  BARD_PUSH_REF( to_bard_string(jstr) );
}

/*
void Android__fast_hardware()
{
  BARD_POP_REF();
  BARD_PUSH_INT32(plasmacore.fast_hardware);
}

void Android__email__String_String_String_String_String()
{
  GET_JAVA_ENV();
  jstring jmime_type = to_jstring( BARD_POP_REF() );
  jstring jfilename   = to_jstring( BARD_POP_REF() );
  jstring jrecipient = to_jstring( BARD_POP_REF() );
  jstring jbody      = to_jstring( BARD_POP_REF() );
  jstring jsubject   = to_jstring( BARD_POP_REF() );
  BARD_POP_REF();  // singleton

  java_env->CallVoidMethod( plasmacore, m_jniEmail, jsubject, jbody, jrecipient, jfilename, jmime_type );

  if (jsubject)   java_env->DeleteLocalRef( jsubject );
  if (jbody)      java_env->DeleteLocalRef( jbody );
  if (jrecipient) java_env->DeleteLocalRef( jrecipient );
  if (jfilename)  java_env->DeleteLocalRef( jfilename );
  if (jmime_type) java_env->DeleteLocalRef( jmime_type );
}

void Android__choose_photo__PhotoChoiceListener()
{
  photo_chooser_delegate.retain( BARD_POP_REF() );
  BARD_POP_REF(); // discard singleton
	java_env->CallVoidMethod( plasmacore, m_jniChoosePhoto );
}

void AdMob__init()
{
}

void AdMob__configure__String_Logical_Color_Color_Color()
{
}

void AdMob__bounds__Vector2_Vector2_Radians()
{
}

void AdMob__visible__Logical()
{
}
*/

void Application__log__String()
{
  BardString* mesg = (BardString*) BARD_POP_REF();  // ignore title string 
  BARD_POP_REF();  // discard singleton
  LOG( mesg );
}

void Application__title__String()
{
  // Application::title(String) 
  BardString* mesg = (BardString*) BARD_POP_REF();  // ignore title string 
  BARD_POP_REF();  // discard singleton

  if ( !mesg ) return;
}

void encode_bitmap( int to_type, double quality )
{
  GET_JAVA_ENV();

  // to_type: 1=png, 2=jpg
  // quality: 0.0 to 1.0
  BardBitmap* bitmap_obj = (BardBitmap*) BARD_POP_REF();
  int w = bitmap_obj->width;
  int h = bitmap_obj->height;

  jintArray src_obj = java_env->NewIntArray( w*h );
  JavaIntArray src_wrapper( src_obj );

  // unmultiply the color components by the alpha and swap red with blue
  BardInt32* src = ((BardInt32*) bitmap_obj->pixels->data) - 1;
  jint* dest = ((jint*) src_wrapper.data) - 1;
  int count = w*h + 1;
  while (--count)
  {
    BardInt32 color = *(++src);
    int a = (color >> 24) & 255;
    int r = (color >> 16) & 255;
    int g = (color >> 8) & 255;
    int b = color & 255;

    if (a)
    {
      r = (r * 255) / a;
      g = (g * 255) / a;
      b = (b * 255) / a;
    }

    *(++dest) = (a<<24) | (b<<16) | (g<<8) | r;
  }

  src_wrapper.release();

  jbyteArray png_bytes = (jbyteArray) java_env->CallObjectMethod( plasmacore_obj, 
      m_jniEncodeBitmapData , w, h, src_obj, to_type, (int)(quality*100) );

  java_env->DeleteLocalRef( src_obj );

  if (png_bytes)
  {
    JavaByteArray encoded_array( png_bytes );
    count = encoded_array.count;

    BardArrayList* list = bard_create_byte_list( encoded_array.data, count );
    BARD_PUSH_REF( list );
  }
  else
  {
    BARD_PUSH_REF(NULL);
  }
}

void Bitmap__to_png_bytes()
{
  encode_bitmap(1,1.0);
}

void Bitmap__to_jpg_bytes__Real64()
{
  double quality = BARD_POP_REAL64();
  encode_bitmap(2,quality);
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

  // no action
}

void Input__keyboard_visible__Logical()
{
  GET_JAVA_ENV();
  BardInt32 setting = BARD_POP_INT32();
  BARD_POP_REF();  // discard singleton
	
  if (setting)
  {
    java_env->CallVoidMethod( plasmacore_obj, m_jniShowKeyboard, true );
  }
  else
  {
    java_env->CallVoidMethod( plasmacore_obj, m_jniShowKeyboard, false );
  }
}

void Input__keyboard_visible()
{
  GET_JAVA_ENV();
  jboolean result = java_env->CallBooleanMethod( plasmacore_obj, m_jniKeyboardVisible );
  BARD_PUSH_INT32( result ? 1 : 0 );
}

void Input__mouse_visible__Logical()
{
  BARD_POP_INT32();
  BARD_POP_REF();      // discard singleton

  // no action
}

void Input__mouse_position__Vector2()
{
  BARD_POP( Vector2 );
  BARD_POP_REF();      // discard singleton

  // no action
}

void Input__input_capture__Logical()
{
  BARD_POP_INT32(); // ignore setting
  BARD_POP_REF();      // discard singleton
}


/*
void Email__available()
{
  BARD_POP_REF();
  BARD_PUSH_INT32( 1 );
}
*/

struct AndroidSoundInfo : BardResource
{
  int id;

  AndroidSoundInfo( int id ) : id(id)
  {
  }
  
  ~AndroidSoundInfo()
  {
    GET_JAVA_ENV();
    java_env->CallVoidMethod( plasmacore_obj, m_jniSoundRelease, id );
    id = 0;
  }
};


void NativeSound__init__ArrayList_of_Byte()
{
  // NativeSound::init(Byte[])

  BardArrayList* list = (BardArrayList*) BARD_POP_REF();
  BardObject* sound_obj = BARD_POP_REF();

  // no action - can't make a sound from a byte list
}

void NativeSound__init__String()
{
  GET_JAVA_ENV();
  BardString* filename = (BardString*) BARD_POP_REF();
  BardObject* sound_obj = BARD_POP_REF();
	
  int count = filename->count;
	char* buffer = new char[count+1];
	filename->to_ascii( buffer, count+1 );
		
	jobject filename_obj = java_env->NewStringUTF(buffer);

  jint sound_id = java_env->CallIntMethod( plasmacore_obj, m_jniSoundLoad, filename_obj );
  java_env->DeleteLocalRef( filename_obj );
	
  if (sound_id > 0)
  {
    BardLocalRef gc_guard( sound_obj );
    AndroidSoundInfo* info = new AndroidSoundInfo(sound_id);
    BardNativeData* data_obj = BardNativeData::create( info, BardNativeDataDeleteResource );
    BARD_SET_REF(sound_obj,"native_data",data_obj);
  }
  else
  {
    LOG("Sound file not found\n");
    bard_throw_file_not_found_error( buffer );
  }
	delete buffer;
}

int get_sound_id( BardObject* sound_obj )
{
  if ( !sound_obj ) return 0;
  BARD_GET( BardNativeData*, data_obj, sound_obj, "native_data" );
  if (data_obj == NULL) return 0;
  return ((AndroidSoundInfo*)data_obj->data)->id;
}


void NativeSound__create_duplicate()
{
  // NativeSound::create_duplicate().Sound 
  GET_JAVA_ENV();
  BardObject* sound_obj = BARD_POP_REF();

  int sound_id = get_sound_id(sound_obj);
	
	jint new_sound_id = java_env->CallIntMethod( plasmacore_obj, m_jniSoundDuplicate, sound_id );
	
  if (new_sound_id != 0)
	{
		BARD_FIND_TYPE( type_sound, "NativeSound" );
		BardObject* new_sound_obj = type_sound->create();
    BARD_PUSH_REF( new_sound_obj );

    AndroidSoundInfo* info = new AndroidSoundInfo(new_sound_id);
    BardNativeData* data_obj = BardNativeData::create( info, BardNativeDataDeleteResource );
    BARD_SET_REF(new_sound_obj,"native_data",data_obj);
	}
	else 
	{
		BARD_PUSH_REF( NULL );
	}
}

void NativeSound__play()
{
  GET_JAVA_ENV();
  BardObject* sound_obj = BARD_POP_REF();
  java_env->CallVoidMethod(plasmacore_obj, m_jniSoundPlay, get_sound_id(sound_obj));
}

void NativeSound__pause()
{
  GET_JAVA_ENV();
  // NativeSound::pause() 
  BardObject* sound_obj = BARD_POP_REF();
  java_env->CallVoidMethod(plasmacore_obj, m_jniSoundPause, get_sound_id(sound_obj));
}

void NativeSound__is_playing()
{
  GET_JAVA_ENV();
  // NativeSound::is_playing().Logical 
  BardObject* sound_obj = BARD_POP_REF();
  jboolean playing = java_env->CallBooleanMethod(plasmacore_obj, m_jniSoundIsPlaying, get_sound_id(sound_obj));

  if (playing)
  {
    BARD_PUSH_INT32(1);
  }
  else
  {
    BARD_PUSH_INT32(0);
  }
}

void NativeSound__volume__Real64()
{
  GET_JAVA_ENV();
  // NativeSound::volume(Real64) 
  double volume = BARD_POP_REAL64();
  BardObject* sound_obj = BARD_POP_REF();
  java_env->CallVoidMethod(plasmacore_obj, m_jniSoundSetVolume, get_sound_id(sound_obj), volume);
}

void NativeSound__pan__Real64()
{
  // NativeSound::pan(Real64) 

  BARD_POP_REAL64();
  BARD_POP_REF(); // sound object 

  // no action
}

void NativeSound__pitch__Real64()
{
  // NativeSound::pitch(Real64) 

  BARD_POP_REAL64();
  BARD_POP_REF(); // sound object 

  // no action
}

void NativeSound__repeats__Logical()
{
  GET_JAVA_ENV();
  // NativeSound::repeats(Logical) 
  int setting = BARD_POP_INT32();
  BardObject* sound_obj = BARD_POP_REF();
  java_env->CallVoidMethod(plasmacore_obj, m_jniSoundSetRepeats, get_sound_id(sound_obj), setting);
}

void NativeSound__current_time()
{
  // NativeSound::current_time().Real64 
  GET_JAVA_ENV();
  BardObject* sound_obj = BARD_POP_REF();
  jdouble curtime = java_env->CallDoubleMethod( plasmacore_obj, m_jniSoundGetCurrentTime, get_sound_id(sound_obj) );
  BARD_PUSH_REAL64( curtime );
}

void NativeSound__current_time__Real64()
{
  // NativeSound::current_time(Real64) 
  GET_JAVA_ENV();
  BardReal64 new_time = BARD_POP_REAL64();
  BardObject* sound_obj = BARD_POP_REF();
  java_env->CallVoidMethod(plasmacore_obj, m_jniSoundSetCurrentTime, get_sound_id(sound_obj), new_time);
}

void NativeSound__duration()
{
  // NativeSound::duration().Real64 
  GET_JAVA_ENV();
  BardObject* sound_obj = BARD_POP_REF();
  jdouble duration = java_env->CallDoubleMethod( plasmacore_obj, m_jniSoundDuration, get_sound_id(sound_obj) );
  BARD_PUSH_REAL64( duration );
}


GLTexture* NativeLayer_get_native_texture_data( BardObject* texture_obj );
// defined in gl_core.cpp


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

  glBindFramebuffer( GL_FRAMEBUFFER, texture->frame_buffer );
  glDisable( GL_SCISSOR_TEST );
  glClearColor( ((color>>16)&255)/255.0f,
      ((color>>8)&255)/255.0f,
      ((color)&255)/255.0f,
      ((color>>24)&255)/255.0f );
  glClear(GL_COLOR_BUFFER_BIT);
  if (use_scissor) glEnable( GL_SCISSOR_TEST );

  if (draw_buffer.draw_target)
  {
    glBindFramebuffer( GL_FRAMEBUFFER, draw_buffer.draw_target->frame_buffer );
  }
  else
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  }
}

void Network__is_connected()
{
  GET_JAVA_ENV();
  BARD_POP_REF();
  jboolean result = java_env->CallBooleanMethod( plasmacore_obj, m_jniNetworkIsConnected );
  BARD_PUSH_LOGICAL( result ? 1 : 0 );
}

void Network__is_mobile()
{
  GET_JAVA_ENV();
  BARD_POP_REF();
  jboolean result = java_env->CallBooleanMethod( plasmacore_obj, m_jniNetworkIsMobile );
  BARD_PUSH_LOGICAL( result ? 1 : 0 );
}

void Network__mac_address()
{
  GET_JAVA_ENV();
  BARD_POP_REF();
  jstring jstr = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniNetworkMacAddress );
  BARD_PUSH_REF( to_bard_string(jstr) );
}

void ResourceManager__load_gamestate__String()
{
  GET_JAVA_ENV();
  BardString* filename = (BardString*) BARD_POP_REF();
  BARD_POP_REF();   // discard singleton

  int count = filename->count;
  char* filename_buffer = new char[count+1];
  filename->to_ascii( filename_buffer, count+1 );
  
  jobject filename_obj = java_env->NewStringUTF(filename_buffer);
  jbyteArray array_obj = (jbyteArray) java_env->CallObjectMethod( plasmacore_obj, m_jniLoadGamestate, filename_obj );
  java_env->DeleteLocalRef( filename_obj );
	
  if (array_obj)
  {
    delete filename_buffer;

		JavaByteArray wrapper = JavaByteArray(array_obj);
		count = wrapper.count;
		
    BardString* result = BardString::create( count );
    BARD_PUSH_REF( result );

    BardChar* data = (BardChar*) result->characters;
    --data;
    ++count;
    while (--count) *(++data) = (BardChar) wrapper.data[wrapper.count - count];

    result->set_hash_code();
  }
  else
  {
    bard_throw_file_not_found_error( filename_buffer );
    delete filename_buffer;
    return;
  }
}

void ResourceManager__save_gamestate__String_String()
{
  // ResourceManager::save_gamestate(String,String)
  GET_JAVA_ENV();
  BardString* content = (BardString*) BARD_POP_REF();
  BardString* filename = (BardString*) BARD_POP_REF();
  BARD_POP_REF();   // discard singleton

  char filename_buffer[128];
  filename->to_ascii( filename_buffer, 128 );
  
  jobject filename_obj = java_env->NewStringUTF(filename_buffer);
  
  int count = content->count;
  jobject content_obj = java_env->NewString((jchar*)(content->characters),count);
	
  jboolean success = java_env->CallBooleanMethod( plasmacore_obj, m_jniSaveGamestate, filename_obj, content_obj );
  java_env->DeleteLocalRef( filename_obj );
	java_env->DeleteLocalRef( content_obj );
	
  if (!success)
  {
    bard_throw_file_error( filename_buffer );
  }
}

void ResourceManager__delete_gamestate__String()
{
  GET_JAVA_ENV();
  BardString* filename = (BardString*) BARD_POP_REF();
  BARD_POP_REF();   // discard singleton

  char filename_buffer[128];
  filename->to_ascii( filename_buffer, 128 );
  
  jobject filename_obj = java_env->NewStringUTF(filename_buffer);
  
  jboolean success = java_env->CallBooleanMethod( plasmacore_obj, m_jniDeleteGamestate, filename_obj );
  java_env->DeleteLocalRef( filename_obj );
}

void SHA1__hmac__String_String()
{
  GET_JAVA_ENV();

  jstring key = to_jstring( BARD_POP_REF() );
  jstring message = to_jstring( BARD_POP_REF() );
  BARD_POP_REF(); // singleton

  jstring hash = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniSHA1HMAC, message, key );
  BARD_PUSH_REF( to_bard_string(hash) );

  java_env->DeleteLocalRef( message );
  java_env->DeleteLocalRef( key );
  java_env->DeleteLocalRef( hash );
}

void System__device_id()
{
  GET_JAVA_ENV();
  BARD_POP_REF();  // discard singleton
  jstring jstr = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniGetDeviceID );
  BARD_PUSH_REF( to_bard_string(jstr) );
}

void System__device_type()
{
  GET_JAVA_ENV();
  BARD_POP_REF();  // discard singleton
  jstring jstr = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniGetDeviceType );
  BARD_PUSH_REF( to_bard_string(jstr) );
}

void System__language()
{
  GET_JAVA_ENV();
  BARD_POP_REF();  // discard singleton
  jstring jstr = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniGetLanguage );
  BARD_PUSH_REF( to_bard_string(jstr) );
}

void System__open_url__String()
{
  GET_JAVA_ENV();
  jstring url = to_jstring( BARD_POP_REF() );
  BARD_POP_REF();  // discard singleton

  java_env->CallVoidMethod( plasmacore_obj, m_jniOpenURL, url );
  java_env->DeleteLocalRef( url );
}

void System__country_name()
{
  GET_JAVA_ENV();
  BARD_POP_REF();  // discard singleton
  jstring id_obj = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniGetCountryName );

  int count = java_env->GetStringLength(id_obj);
  const jchar* id_data = java_env->GetStringChars( id_obj, NULL );

  BardString* result = BardString::create( count );
  memcpy( result->characters, id_data, count*2 );
  BARD_PUSH_REF( (BardObject*) result );  // leave result on stack
  result->set_hash_code();

  java_env->ReleaseStringChars( id_obj, id_data );
}

void System__exit_program__Int32_String()
{
  GET_JAVA_ENV();
  BardString* error_mesg = (BardString*) BARD_POP_REF();
  if (error_mesg) 
  {
    LOG( "ERROR - EXITING PROGRAM" );
    LOG( error_mesg );
  }

  BARD_POP_INT32();  // ignore error code
  BARD_POP_REF();    // singleton

  java_env->CallVoidMethod( plasmacore_obj, m_jniExitProgram );
}

void Texture_init( BardInt32* data, int w, int h, int format );

jmp_buf plasmacore_png_jmp_buffer;
int     plasmacore_png_width;
int     plasmacore_png_height;
int*    plasmacore_png_buffer = NULL;
int     plasmacore_png_buffer_size = 0;
int     plasmacore_png_texture_format = 0;

static void plasmacore_png_error_handler( png_structp png_ptr, png_const_charp msg )
{
  fprintf(stderr, "libpng error: %s\n", msg);
  fflush(stderr);

  longjmp( plasmacore_png_jmp_buffer, 1 );
}

static void plasmacore_png_info_callback( png_structp png_ptr, png_infop info_ptr )
{
  int         color_type, bit_depth;
  png_uint_32 width, height;
  double      gamma;

  png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
      NULL, NULL, NULL );
  plasmacore_png_width  = width;
  plasmacore_png_height = height;

  Texture_init( NULL, width, height, plasmacore_png_texture_format );

  if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand( png_ptr );
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand( png_ptr );
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand( png_ptr );
  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
  {
    png_set_gray_to_rgb( png_ptr );
  }

  png_read_update_info( png_ptr, info_ptr );
}

static void plasmacore_png_row_callback(png_structp png_ptr, png_bytep new_row,
                                  png_uint_32 row_num, int pass)
{
    if ( !new_row ) return;

    if (plasmacore_png_buffer && plasmacore_png_buffer_size < plasmacore_png_width)
    {
      delete plasmacore_png_buffer;
      plasmacore_png_buffer = NULL;
    }

    if ( !plasmacore_png_buffer )
    {
      plasmacore_png_buffer = new int[ plasmacore_png_width ];
      plasmacore_png_buffer_size = plasmacore_png_width;
    }

    // The appropriate GL texture was prepped just before this PNG decoding
    if (plasmacore_png_texture_format == 1)
    {
      // premultiply the alpha
      int count = plasmacore_png_width + 1;
      unsigned char* data = new_row;
      while (--count)
      {
        int a = data[3];
        data[0] = (data[0] * a) / 255;
        data[1] = (data[1] * a) / 255;
        data[2] = (data[2] * a) / 255;
        data += 4;
      }
      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, row_num, plasmacore_png_width, 1, GL_RGBA, GL_UNSIGNED_BYTE, new_row );
    }
    else
    {
      // premultiply the alpha and convert it to 16-bit
      int count = plasmacore_png_width + 1;
      unsigned char* src = new_row;
      unsigned short* dest = ((unsigned short*) new_row) - 1;
      while (--count)
      {
        int a = src[3];
        int r = ((src[0] * a) / 255) >> 4;
        int g = ((src[1] * a) / 255) >> 4;
        int b = ((src[2] * a) / 255) >> 4;
        a >>= 4;
        *(++dest) = (unsigned short) ((r<<12) | (g<<8) | (b<<4) | a);
        src  += 4;
      }
      glTexSubImage2D( GL_TEXTURE_2D, 0, 0, row_num, plasmacore_png_width, 1, GL_RGBA, GL_UNSIGNED_SHORT_4_4_4_4, new_row );
    }
//printf( "PNG read row #%d (%02x,%02x,%02x,%02x)\n",row_num,new_row[0],new_row[1],new_row[2],new_row[3]);

    //png_progressive_combine_row(png_ptr, mainprog_ptr->row_pointers[row_num],
      //new_row);
}

bool plasmacore_decode_png( char* data, int data_size, int* width_ptr, int* height_ptr )
{
  png_structp  png_ptr;
  png_infop    info_ptr;

  png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL,
      plasmacore_png_error_handler, NULL );
  if ( !png_ptr ) return false; // Out of memory

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct( &png_ptr, NULL, NULL );
    return false;  // Out of memory
  }

  if (setjmp(plasmacore_png_jmp_buffer))
  {
    png_destroy_read_struct( &png_ptr, &info_ptr, NULL );
    return false;
  }

#ifdef PNG_HANDLE_AS_UNKNOWN_SUPPORTED
  // Prepare the reader to ignore all recognized chunks whose data won't be
  // used, i.e., all chunks recognized by libpng except for IHDR, PLTE, IDAT,
  // IEND, tRNS, bKGD, gAMA, and sRGB (small performance improvement).
  {
    static png_byte chunks_to_ignore[] = 
    {
       99,  72,  82,  77, '\0',  // cHRM
      104,  73,  83,  84, '\0',  // hIST
      105,  67,  67,  80, '\0',  // iCCP
      105,  84,  88, 116, '\0',  // iTXt
      111,  70,  70, 115, '\0',  // oFFs
      112,  67,  65,  76, '\0',  // pCAL
      112,  72,  89, 115, '\0',  // pHYs
      115,  66,  73,  84, '\0',  // sBIT
      115,  67,  65,  76, '\0',  // sCAL
      115,  80,  76,  84, '\0',  // sPLT
      115,  84,  69,  82, '\0',  // sTER
      116,  69,  88, 116, '\0',  // tEXt
      116,  73,  77,  69, '\0',  // tIME
      122,  84,  88, 116, '\0'   // zTXt
    };

    png_set_keep_unknown_chunks( png_ptr, PNG_HANDLE_CHUNK_NEVER,
        chunks_to_ignore, sizeof(chunks_to_ignore)/5 );
  }
#endif // PNG_HANDLE_AS_UNKNOWN_SUPPORTED

  png_set_progressive_read_fn( png_ptr, NULL,
      plasmacore_png_info_callback, plasmacore_png_row_callback, NULL);

  png_process_data( png_ptr, info_ptr, (unsigned char*) data, (unsigned long) data_size );
  png_destroy_read_struct( &png_ptr, &info_ptr, NULL );

  *width_ptr = plasmacore_png_width;
  *height_ptr = plasmacore_png_height;

  return true;
}

void Texture__init__String_Int32()
{
  plasmacore_png_texture_format = BARD_POP_INT32();
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

  int width=0, height=0;
  if (plasmacore_decode_png( data, data_size, &width, &height ))
  {
/*
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
    Texture_init( pixels, width, height, plasmacore_png_texture_format );
    delete pixels;
*/
  }
  else
  {
    BARD_DISCARD_REF();
  }

  delete data;
}

/*
void Texture__init__String_Int32()
{
  GET_JAVA_ENV();
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

  jbyteArray src_obj = java_env->NewByteArray( data_size );
  JavaByteArray src_wrapper( src_obj );
  memcpy( src_wrapper.data, data, data_size );
  src_wrapper.release();
  jintArray decoded_obj = (jintArray) java_env->CallObjectMethod( plasmacore_obj,
      m_jniDecodeBitmapData , src_obj );

  java_env->DeleteLocalRef( src_obj );

  if (decoded_obj)
  {
    {
      JavaIntArray decoded_array( decoded_obj );
      int width  = decoded_array.data[decoded_array.count-1];
      int height = (decoded_array.count - 1) / width;

      // premultiply the alpha and swap red with blue
      int count = (width * height) + 1;
      BardInt32* cur = ((BardInt32*) decoded_array.data);
      while (--count)
      {
        BardInt32 color = *cur;
        int a = (color >> 24) & 255;
        int r = (color >> 16) & 255;
        int g = (color >> 8) & 255;
        int b = color & 255;

        r = (r * a) / 255;
        g = (g * a) / 255;
        b = (b * a) / 255;

        *(cur++) = (a<<24) | (r<<16) | (g<<8) | b;
      }

      Texture_init( (BardInt32*)decoded_array.data, width, height, format );
    }
    java_env->DeleteLocalRef( decoded_obj );
  }
  else
  {
    BARD_DISCARD_REF();
  }

  delete data;
}
*/

void Texture__draw_tile__Corners_Vector2_Vector2_Int32()
{
  BardInt32 render_flags = BARD_POP_INT32();
  Vector2 size = BARD_POP(Vector2);
  Vector2 pos  = BARD_POP(Vector2);
  Vector2 uv_a = BARD_POP(Vector2);
  Vector2 uv_b = BARD_POP(Vector2);
  BardObject* texture_obj = BARD_POP_REF();

  GLTexture* texture = NativeLayer_get_native_texture_data(texture_obj);
  if ( !texture ) return;

  double texture_width  = texture->texture_width;
  double texture_height = texture->texture_height;

  GLint src_rect[4];

  if (size.x < 0)
  {
    // hflip
    size.x = -size.x;
    src_rect[0] = (GLint) (uv_b.x * texture_width);   // right
    src_rect[2] = (GLint) -((uv_b.x - uv_a.x) * texture_width);   // width
  }
  else
  {
    src_rect[0] = (GLint) (uv_a.x * texture_width);   // left
    src_rect[2] = (GLint) ((uv_b.x - uv_a.x) * texture_width);   // width
  }

  if (size.y < 0)
  {
    // vflip
    size.y = -size.y;
    src_rect[1] = (GLint) (uv_a.y * texture_height);  // top
    src_rect[3] = (GLint) ((uv_b.y - uv_a.y) * texture_height); // height
  }
  else
  {
    src_rect[1] = (GLint) (uv_b.y * texture_height);  // bottom
    src_rect[3] = (GLint) -((uv_b.y - uv_a.y) * texture_height); // height
  }


  draw_buffer.render();

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

#ifdef OLD_OPENGL_V1
  glClientActiveTexture(GL_TEXTURE1);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glClientActiveTexture(GL_TEXTURE0);

  glClientActiveTexture(GL_TEXTURE0);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, texture->id );
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);

  glTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, src_rect );

  double y_pos = (plasmacore.display_height - size.y) - pos.y;
  glDrawTexfOES( (GLfloat) pos.x, (GLfloat)y_pos, 0.0f, (GLfloat) size.x, (GLfloat) size.y );
  glDisable(GL_TEXTURE_2D);
#endif //OLD_OPENGL_V1
}

//=============================================================================
//  File I/O
//=============================================================================

void bard_adjust_filename_for_os( char* filename, int len )
{
  // no action
}

int bard_is_directory( const char* filename )
{
  GET_JAVA_ENV();
  jobject filename_obj = java_env->NewStringUTF(filename);
  jboolean result = java_env->CallBooleanMethod( plasmacore_obj, m_jniIsDirectory, filename_obj );
  java_env->DeleteLocalRef( filename_obj );

  return result ? 1 : 0;
}

int bard_file_exists( const char* filename )
{
  GET_JAVA_ENV();
  jobject filename_obj = java_env->NewStringUTF(filename);
  jboolean result = java_env->CallBooleanMethod( plasmacore_obj, m_jniFileExists, filename_obj );
  java_env->DeleteLocalRef( filename_obj );

  return result ? 1 : 0;
}

bool bard_file_helper_retrieve_filepath( char* buffer, int count )
{
  BardObject* file_obj = BARD_POP_REF();
  BVM_NULL_CHECK( file_obj, return false );
  BardString* filepath_obj = ((BardFile*)file_obj)->property_filepath;
  filepath_obj->to_ascii( buffer, count );
  return true;
}


void File__is_directory()
{
  char filename[512];

  bard_file_helper_retrieve_filepath( filename, 512 );
  BARD_PUSH_INT32( bard_is_directory(filename) );
}

void File__exists()
{
  char filename[512];

  bard_file_helper_retrieve_filepath( filename, 512 );
  BARD_PUSH_INT32( bard_file_exists(filename) );
}

void File__directory_listing__ArrayList_of_String()
{
  GET_JAVA_ENV();
  BardObject* list = BARD_POP_REF();
  BardLocalRef gc_guard(list);

  BVM_NULL_CHECK( list, return );

  char filename[PATH_MAX];
  BardObject**    list_ptr;

  bard_file_helper_retrieve_filepath( filename, PATH_MAX );

  jobject filename_obj = java_env->NewStringUTF(filename);
  jobjectArray array = (jobjectArray) java_env->CallObjectMethod( plasmacore_obj, 
        m_jniDirectoryListing, filename_obj );
  java_env->DeleteLocalRef( filename_obj );

  if ( !array )
  {
    bard_throw_file_not_found_error( filename );
    return;
  }

  JavaStringArray filenames( array );

  for (int i=0; i<filenames.count; ++i)
  {
    BARD_PUSH_REF( list );
    BARD_PUSH_REF( (BardObject*) filenames[i] );
    BARD_CALL( list->type, "add(String)" );
    BARD_POP_REF();
  }
}

void File__absolute_filepath()
{
  GET_JAVA_ENV();
  char filepath[PATH_MAX];

  bard_file_helper_retrieve_filepath( filepath, PATH_MAX );

  jobject filepath_obj = java_env->NewStringUTF(filepath);
  jstring jabspath = (jstring) java_env->CallObjectMethod( plasmacore_obj, m_jniAbsoluteFilePath,
    filepath_obj );
  java_env->DeleteLocalRef( filepath_obj );

  const char* jabspath_utf8 = java_env->GetStringUTFChars( jabspath, NULL );
  strcpy( filepath, jabspath_utf8 );
  java_env->ReleaseStringUTFChars( jabspath, jabspath_utf8 );

  BARD_PUSH_REF( BardString::create(filepath) );
}

void File__copy__String()
{
  // File::copy(String)
  GET_JAVA_ENV();
  BardString* new_name_obj = (BardString*) BARD_POP_REF();
  BVM_NULL_CHECK( new_name_obj, return );

  char new_name[PATH_MAX];
  new_name_obj->to_ascii( new_name, 512 );

  char filepath[512];
  if ( !bard_file_helper_retrieve_filepath( filepath, PATH_MAX ) ) return;

  jobject j_filepath_obj = java_env->NewStringUTF(filepath);
  jobject j_new_name_obj = java_env->NewStringUTF(filepath);

  java_env->CallVoidMethod( plasmacore_obj, m_jniFileCopy, j_filepath_obj, j_new_name_obj );

  java_env->DeleteLocalRef( j_new_name_obj );
  java_env->DeleteLocalRef( j_filepath_obj );
}


void File__rename__String()
{
  // File::rename(String)
  GET_JAVA_ENV();
  BardString* new_name_obj = (BardString*) BARD_POP_REF();
  BVM_NULL_CHECK( new_name_obj, return );

  char new_name[PATH_MAX];
  new_name_obj->to_ascii( new_name, 512 );

  char filepath[512];
  if ( !bard_file_helper_retrieve_filepath( filepath, PATH_MAX ) ) return;

  jobject j_filepath_obj = java_env->NewStringUTF(filepath);
  jobject j_new_name_obj = java_env->NewStringUTF(filepath);

  java_env->CallVoidMethod( plasmacore_obj, m_jniFileRename, j_filepath_obj, j_new_name_obj );

  java_env->DeleteLocalRef( j_new_name_obj );
  java_env->DeleteLocalRef( j_filepath_obj );
}

void File__delete()
{
  // File::delete()
  GET_JAVA_ENV();
  char filepath[PATH_MAX];
  if ( !bard_file_helper_retrieve_filepath( filepath, PATH_MAX ) ) return;

  jobject filepath_obj = java_env->NewStringUTF(filepath);
  java_env->CallVoidMethod( plasmacore_obj, m_jniFileDelete, filepath_obj );
  java_env->DeleteLocalRef( filepath_obj );
}

void File__timestamp()
{
  // File::timestamp().Int64
  GET_JAVA_ENV();
  char filepath[PATH_MAX];
  if ( !bard_file_helper_retrieve_filepath( filepath, PATH_MAX ) ) return;

  jobject filepath_obj = java_env->NewStringUTF(filepath);
  jlong timestamp = java_env->CallLongMethod( plasmacore_obj, m_jniFileTimestamp, filepath_obj );
  java_env->DeleteLocalRef( filepath_obj );
  BARD_PUSH_INT64( timestamp );
}

void File__touch()
{
  // File::touch()
  GET_JAVA_ENV();
  char filepath[PATH_MAX];
  if ( !bard_file_helper_retrieve_filepath( filepath, PATH_MAX ) ) return;

  jobject filepath_obj = java_env->NewStringUTF(filepath);
  java_env->CallVoidMethod( plasmacore_obj, m_jniFileTouch, filepath_obj );
  java_env->DeleteLocalRef( filepath_obj );
}

void File__native_mkdir()
{
  GET_JAVA_ENV();
  char filepath[PATH_MAX];
  if ( !bard_file_helper_retrieve_filepath( filepath, PATH_MAX ) ) return;

  jobject filepath_obj = java_env->NewStringUTF(filepath);
  java_env->CallVoidMethod( plasmacore_obj, m_jniFileMkdir, filepath_obj );
  java_env->DeleteLocalRef( filepath_obj );
}

void File__change_dir()
{
  BARD_POP_REF();
  // no action
}

//--------------------------------------------------------------------
//  FileReader
//--------------------------------------------------------------------
#define ANDROID_FILE_BUFFER_SIZE 2048
struct AndroidFileInfo : BardResource
{
  int     fp;
  int     total_size; // in whole file
  int     num_read;   // total
  int     pos;        // within buffer
  bool    is_reader;
  bool    finished;
  unsigned char buffer[ANDROID_FILE_BUFFER_SIZE];

  ~AndroidFileInfo()
  {
    close();
  }

  bool open_infile( const char* filename )
  {
    GET_JAVA_ENV();
    jobject j_filename_obj = java_env->NewStringUTF(filename);
    fp = java_env->CallIntMethod( plasmacore_obj, m_jniFileReaderOpen, j_filename_obj );
    java_env->DeleteLocalRef( j_filename_obj );

    if ( !fp ) return false;

    total_size = java_env->CallIntMethod( plasmacore_obj, m_jniFileReaderAvailable, fp );
    num_read = 0;
    pos = 0;
    is_reader = true;
    finished = false;

    if (total_size == 0) 
    {
      close();
      return true;  // success but zero-length file
    }

    fill_buffer();

    if (finished) return false;
    return true;
  }

  bool open_outfile( const char* filename, jboolean append )
  {
    GET_JAVA_ENV();
    jobject j_filename_obj = java_env->NewStringUTF(filename);
    fp = java_env->CallIntMethod( plasmacore_obj, m_jniFileWriterOpen, j_filename_obj, append );
    java_env->DeleteLocalRef( j_filename_obj );

    if ( !fp ) return false;

    total_size = 0;  // note: inaccurate for append mode but no easy fix
    pos = 0;
    is_reader = false;
    finished = false;

    return true;
  }

  void close()
  {
    GET_JAVA_ENV();
    if (fp)
    {
      if (is_reader)
      {
        java_env->CallVoidMethod( plasmacore_obj, m_jniFileReaderClose, fp );
      }
      else
      {
        flush();
        java_env->CallVoidMethod( plasmacore_obj, m_jniFileWriterClose, fp );
      }
      finished = true;
      fp = 0;
    }
  }

  void fill_buffer()
  {
    GET_JAVA_ENV();
    jbyteArray bytes = (jbyteArray) java_env->CallObjectMethod( plasmacore_obj,
        m_jniFileReaderReadBytes, fp, ANDROID_FILE_BUFFER_SIZE );
    if (bytes == NULL)
    {
      finished = true; // error
      bard_throw_file_error();
      return;
    }

    JavaByteArray byte_data(bytes);
    memcpy( buffer, byte_data.data, ANDROID_FILE_BUFFER_SIZE );
    pos = 0;
  }

  int peek()
  {
    return buffer[pos];
  }

  int read()
  {
    GET_JAVA_ENV();
    int result = buffer[pos++];
    if (++num_read == total_size)
    {
      finished = true;
      java_env->CallVoidMethod( plasmacore_obj, m_jniFileReaderClose, fp );
    }
    else if (pos == ANDROID_FILE_BUFFER_SIZE)
    {
      fill_buffer();
    }
    return result;
  }

  int remaining() { return total_size - num_read; }

  void skip( int count )
  {
    if (count > remaining()) count = remaining();
    if (count <= 0) return;

    while (count > 0)
    {
      int num_buffered = ANDROID_FILE_BUFFER_SIZE - pos;
      if (count >= num_buffered)
      {
        num_read += num_buffered;
        count -= num_buffered;
        fill_buffer();
      }
      else
      {
        num_read += count;
        pos += count;
        count = 0;
      }
    }
  }

  int read_bytes( void* dest_ptr, int count )
  {
    unsigned char* dest = (unsigned char*) dest_ptr;
    if (count > remaining()) count = remaining();

    int num_read_before = num_read;

    while (count > 0)
    {
      int num_buffered = ANDROID_FILE_BUFFER_SIZE - pos;
      if (count >= num_buffered)
      {
        memcpy( dest, buffer+pos, num_buffered );
        dest += num_buffered;
        num_read += num_buffered;
        count -= num_buffered;
        fill_buffer();
      }
      else
      {
        memcpy( dest, buffer+pos, count );
        dest += count;
        num_read += count;
        pos += count;
        count = 0;
      }
    }

    if (num_read == total_size) close();

    return num_read - num_read_before;
  }

  int read_chars( void* dest_ptr, int count )
  {
    BardChar* dest = (BardChar*) dest_ptr;
    if (count > remaining()) count = remaining();

    int num_read_before = num_read;

    while (count > 0)
    {
      int num_buffered = ANDROID_FILE_BUFFER_SIZE - pos;
      if (count >= num_buffered)
      {
        num_read += num_buffered;
        count -= num_buffered;

        ++num_buffered;
        --dest;
        unsigned char* src = (buffer+pos) - 1;
        while (--num_buffered)
        {
          *(++dest) = *(++src);
        }

        fill_buffer();
      }
      else
      {
        num_read += count;

        ++count;
        --dest;
        unsigned char* src = (buffer+pos) - 1;
        while (--count)
        {
          *(++dest) = *(++src);
        }
      }
    }

    if (num_read == total_size) close();

    return num_read - num_read_before;
  }

  void write( int ch )
  {
    ++total_size;
    buffer[pos] = (unsigned char) ch;
    if (++pos == ANDROID_FILE_BUFFER_SIZE)
    {
      flush();
    }
  }

  void write_bytes( void* data_ptr, int count )
  {
    char* data = (char*) data_ptr;

    if (pos + count >= ANDROID_FILE_BUFFER_SIZE)
    {
      flush();
    }

    while (count >= ANDROID_FILE_BUFFER_SIZE)
    {
      memcpy( buffer+pos, data, ANDROID_FILE_BUFFER_SIZE );
      count -= ANDROID_FILE_BUFFER_SIZE;
      data  += ANDROID_FILE_BUFFER_SIZE;
      pos   += ANDROID_FILE_BUFFER_SIZE;
      total_size += ANDROID_FILE_BUFFER_SIZE;
      flush();
    }

    if (count > 0)
    {
      memcpy( buffer+pos, data, count );
      pos        += count;
      total_size += count;
      flush();
    }
  }

  void write_chars( void* data_ptr, int count )
  {
    BardChar* data = (BardChar*) data_ptr;

    ++count;
    --data;
    while (--count) write( *(++data) );
  }

  void flush()
  {
    GET_JAVA_ENV();
    jbyteArray jbytes = (jbyteArray) java_env->CallObjectMethod( plasmacore_obj, m_jniGetIOBuffer, 
        ANDROID_FILE_BUFFER_SIZE );

    JavaByteArray src_wrapper( jbytes );
    memcpy( src_wrapper.data, buffer, pos );
    src_wrapper.release();

    java_env->CallVoidMethod( plasmacore_obj, m_jniFileWriterWriteBytes, fp, jbytes, pos );
    pos = 0;
  }
};



void FileReader__init__String()
{
  // method init( String filename )
  BardString* filename_string = (BardString*) BARD_POP_REF();
  BardObject* reader          = BARD_POP_REF();
  FILE*       fp;
  char filename[512];

  BVM_NULL_CHECK( filename_string && reader, return );

  filename_string->to_ascii( filename, 512 );
  bard_adjust_filename_for_os( filename, 512 );

  if (bard_is_directory(filename))
  {
    bard_throw_file_error( filename );
    return;
  }

  BardLocalRef gc_guard(reader);
  AndroidFileInfo* info = new AndroidFileInfo();
  if ( !info->open_infile(filename) )
  {
    // no luck
    bard_throw_file_not_found_error( filename );
    return;
  }

  if (info->finished) return;  // zero-length file

  BardNativeData* data_obj = BardNativeData::create( info, BardNativeDataDeleteResource );
  BARD_SET_REF(reader,"native_data",data_obj);
}

static AndroidFileInfo* get_reader_file_info( BardObject* reader )
{
  BARD_GET( BardNativeData*, native_data, reader, "native_data" );
  if ( !native_data ) return NULL;

  return (AndroidFileInfo*) native_data->data;
}

static void close_reader( BardObject* reader )
{
  BARD_GET( BardNativeData*, native_data, reader, "native_data" );
  if ( !native_data ) return;
  BARD_SET_REF( reader, "native_data", NULL );

  BARD_PUSH_REF( (BardObject*) native_data );
  NativeData__clean_up();
}


void FileReader__close()
{
  BardObject*   reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  close_reader(reader);
}

void FileReader__has_another()
{
  BardObject*   reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  // Bard files auto-close after the last character so if it's still open that
  // means there's another.
  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info) BARD_PUSH_INT32( 1 );
  else BARD_PUSH_INT32( 0 );
}

void FileReader__peek()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (info->fp)
    {
      BARD_PUSH_INT32( info->peek() );
      if (info->finished) close_reader(reader);
    }
  }
  else
  {
    BARD_PUSH_INT32( -1 );
  }
}

void FileReader__read()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (info->fp)
    {
      BARD_PUSH_INT32( info->read() );
      if (info->finished) close_reader(reader);
    }
  }
  else
  {
    BARD_PUSH_INT32( -1 );
  }
}

void FileReader__read__Array_of_Byte_Int32_Int32()
{
  BardInt32 count    = BARD_POP_INT32();
  BardInt32 i        = BARD_POP_INT32();
  BardArray* array   = (BardArray*) BARD_POP_REF();
  BardObject* reader = BARD_POP_REF();

  BVM_NULL_CHECK( reader && array, return );

#if defined(BARD_VM)
  if (i < 0 || count < 0 || i+count > array->array_count)
  {
    bvm.throw_exception( bvm.type_out_of_bounds_error );
    return;
  }
#endif

  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (count > info->remaining()) count = info->remaining();

    if (info->fp)
    {
      info->read_bytes( (char*) array->data + i, count );
    }

    BARD_PUSH_INT32( count );
  }
  else
  {
    BARD_PUSH_INT32( 0 );  // no data left in file
  }
}

void FileReader__read__Array_of_Char_Int32_Int32()
{
  BardInt32 count    = BARD_POP_INT32();
  BardInt32 i        = BARD_POP_INT32();
  BardArray* array   = (BardArray*) BARD_POP_REF();
  BardObject* reader = BARD_POP_REF();

  BVM_NULL_CHECK( reader && array, return );

#if defined(BARD_VM)
  if (i < 0 || count < 0 || i+count > array->array_count)
  {
    bvm.throw_exception( bvm.type_out_of_bounds_error );
    return;
  }
#endif

  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (count > info->remaining()) count = info->remaining();

    if (info->fp)
    {
      info->read_chars( (char*) array->data + i, count );
    }

    BARD_PUSH_INT32( count );
  }
  else
  {
    BARD_PUSH_INT32( 0 );  // no data left in file
  }
}

void FileReader__remaining()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    BARD_PUSH_INT32( info->remaining() );
  }
  else
  {
    BARD_PUSH_INT32( 0 );
  }
}

void FileReader__skip__Int32()
{
  BardInt32   skip_bytes = BARD_POP_INT32();
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    info->skip(skip_bytes);
  }
}

void FileReader__position()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  AndroidFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    BARD_PUSH_INT32( info->num_read );
  }
  else
  {
    BARD_PUSH_INT32( info->total_size );
  }
}

//--------------------------------------------------------------------
//  FileWriter
//--------------------------------------------------------------------
void FileWriter__init__String_Logical()
{
  // method init( String filename, Logical append )
  int append = BARD_POP_INT32();
  BardString* filename_string = (BardString*) BARD_POP_REF();
  BardObject* writer          = BARD_POP_REF();
  FILE*       fp;
  char filename[512];

  BVM_NULL_CHECK( filename_string && writer, return );

  filename_string->to_ascii( filename, 512 );

  AndroidFileInfo* info = new AndroidFileInfo();
  if ( !info->open_outfile(filename,(bool)append) )
  {
    bard_throw_file_error( filename );
    return;
  }

  BardLocalRef gc_guard( writer );
  BardNativeData* data_obj = BardNativeData::create( info, BardNativeDataDeleteResource );

  BARD_SET_REF(writer,"native_data",data_obj);
}

void FileWriter__close()
{
  // method close()
  BardObject*   writer = BARD_POP_REF();
  BVM_NULL_CHECK( writer, return );

  BARD_GET( BardNativeData*, native_data, writer, "native_data" );
  if ( !native_data ) return;
  BARD_SET_REF( writer, "native_data", NULL );

  BARD_PUSH_REF( (BardObject*) native_data );
  NativeData__clean_up();
}

static AndroidFileInfo* get_writer_file_info( BardObject* writer )
{
  BARD_GET( BardNativeData*, native_data, writer, "native_data" );
  if ( !native_data ) return NULL;

  return (AndroidFileInfo*) native_data->data;
}

void FileWriter__write__Char()
{
  // method write( Char value )
  BardInt32     ch = BARD_POP_INT32();
  BardObject*   writer = BARD_POP_REF();

  BVM_NULL_CHECK( writer, return );

  AndroidFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    info->write(ch);
    return;
  }
  else
  {
    bard_throw_file_error();
  }
}

void FileWriter__position()
{
  // method position.Int32
  BardObject*   writer = BARD_POP_REF();
  BVM_NULL_CHECK( writer, return );

  AndroidFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    BARD_PUSH_INT32( info->total_size );
  }
  else
  {
    bard_throw_file_error();
  }
}

void FileWriter__write__Array_of_Char_Int32_Int32()
{
  BardInt32   count  = BARD_POP_INT32();
  BardInt32   index  = BARD_POP_INT32();
  BardArray*  array  = (BardArray*) BARD_POP_REF();
  BardObject* writer = BARD_POP_REF();

  BVM_NULL_CHECK( writer && array, return );

  AndroidFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    int limit = index + count;
    if (index < 0 || count < 0 || limit > array->array_count)
    {
      bard_throw_invalid_operand_error();
      return;
    }

    info->write_chars( ((BardChar*)array->data)+index, count );
  }
  else
  {
    bard_throw_file_error();
  }
}

void FileWriter__write__Array_of_Byte_Int32_Int32()
{
  BardInt32   count  = BARD_POP_INT32();
  BardInt32   index  = BARD_POP_INT32();
  BardArray*  array  = (BardArray*) BARD_POP_REF();
  BardObject* writer = BARD_POP_REF();

  BVM_NULL_CHECK( writer && array, return );

  AndroidFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    int limit = index + count;
    if (index < 0 || count < 0 || limit > array->array_count)
    {
      bard_throw_invalid_operand_error();
      return;
    }

    info->write_chars( ((char*)array->data)+index, count );
  }
  else
  {
    bard_throw_file_error();
  }
}

void FileWriter__write__String()
{
  BardString* st     = (BardString*) BARD_POP_REF();
  BardObject* writer = BARD_POP_REF();

  BVM_NULL_CHECK( writer && st, return );

  AndroidFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    info->write_chars( st->characters, st->count );
  }
  else
  {
    bard_throw_file_error();
  }
}

//=============================================================================
//  VideoPlayer
//=============================================================================
struct AndroidVideoPlayerInfo : BardResource
{
  int player_id;

  AndroidVideoPlayerInfo( const char* filename )
  {
    GET_JAVA_ENV();
    jobject j_filename_obj = java_env->NewStringUTF(filename);
    player_id = java_env->CallIntMethod( plasmacore_obj, m_jniVideoPlay, j_filename_obj );
    java_env->DeleteLocalRef( j_filename_obj );
  }

  ~AndroidVideoPlayerInfo()
  {
    GET_JAVA_ENV();
    if (player_id)
    {
      java_env->CallVoidMethod( plasmacore_obj, m_jniVideoStop, player_id );
      player_id = 0;
    }
  }

  bool update()
  {
    GET_JAVA_ENV();
    if (player_id == 0) return false;
    return java_env->CallBooleanMethod( plasmacore_obj, m_jniVideoUpdate, player_id );
  }
};

void VideoPlayer__play__String()
{
  BardString* filepath    = (BardString*) BARD_POP_REF();
  BardObject* context = BARD_POP_REF();  // context

  char filename[512];

#if defined(BARD_VM)
  if ( !filepath )
  {
    bvm.throw_exception( bvm.type_null_reference_error );
    return;
  }
#endif

  ((BardString*)filepath)->to_ascii( filename, 512 );
  bard_adjust_filename_for_os( filename, 512 );

  AndroidVideoPlayerInfo* player = new AndroidVideoPlayerInfo( filename );
  BARD_PUSH_REF( BardNativeData::create( player, BardNativeDataDeleteResource ) );
}

void VideoPlayer__update__NativeData()
{
  AndroidVideoPlayerInfo* player = NULL;
  BardNativeData* native_data = (BardNativeData*) BARD_POP_REF();
  if (native_data) player = (AndroidVideoPlayerInfo*) native_data->data;

  BardObject* context = BARD_POP_REF();  // context

  if ( !player || !player->update() )
  {
    BARD_PUSH_LOGICAL( false );
  }
  else
  {
    BARD_PUSH_LOGICAL( true );
  }
}


//=============================================================================
//  WebView
//=============================================================================
static int get_webview_index( BardObject* bard_webview )
{
  GET_JAVA_ENV();
  BARD_GET_INT32( id, bard_webview, "id" );
  id = java_env->CallIntMethod( plasmacore_obj, m_jniWebViewGet, id );
  BARD_SET_INT32( bard_webview, "id", id );
  return id;
}

void WebView__view__URL()
{
  GET_JAVA_ENV();
  BardObject* url_obj = BARD_POP_REF();
  int id = get_webview_index( BARD_POP_REF() );

  if (url_obj == NULL) return;

  BARD_GET_REF( url_string, url_obj, "value" );
  jstring url = to_jstring( url_string );

  java_env->CallVoidMethod( plasmacore_obj, m_jniWebViewURL, id, url );

  java_env->DeleteLocalRef( url );
}

void WebView__view__String()
{
  GET_JAVA_ENV();
  jstring html = to_jstring( BARD_POP_REF() );
  int id = get_webview_index( BARD_POP_REF() );

  java_env->CallVoidMethod( plasmacore_obj, m_jniWebViewHTML, id, html );

  java_env->DeleteLocalRef( html );
}

void WebView__close()
{
  GET_JAVA_ENV();
  BardObject* bard_webview = BARD_POP_REF();
  BARD_GET_INT32( id, bard_webview, "id" );
  if (id)
  {
    java_env->CallVoidMethod( plasmacore_obj, m_jniWebViewClose, id );
    BARD_SET_INT32( bard_webview, "id", 0 );
  }
}

void WebView__bounds__Box()
{
  GET_JAVA_ENV();
  Vector2 position = BARD_POP(Vector2);
  Vector2 size = BARD_POP(Vector2);
  int id = get_webview_index( BARD_POP_REF() );

  //position = plasmacore.point_to_screen(position);
  //size     = plasmacore.size_to_screen(size);

  java_env->CallVoidMethod( plasmacore_obj, m_jniWebViewSetBounds, id, 
      (int)position.x, (int)position.y, (int) size.x, (int) size.y );
}


void WebView__visible__Logical()
{
  GET_JAVA_ENV();
  int setting = BARD_POP_INT32();
  int id = get_webview_index( BARD_POP_REF() );
  jboolean jsetting = (setting==1) ? true : false;
  java_env->CallVoidMethod( plasmacore_obj, m_jniWebViewSetVisible, id, jsetting );
}

void WebView__visible()
{
  GET_JAVA_ENV();
  int id = get_webview_index( BARD_POP_REF() );
  jboolean setting = java_env->CallBooleanMethod( plasmacore_obj, m_jniWebViewGetVisible, id );
  BARD_PUSH_INT32( setting ? 1 : 0 );
}

void WebView__loaded()
{
  GET_JAVA_ENV();
  int id = get_webview_index( BARD_POP_REF() );
  jboolean setting = java_env->CallBooleanMethod( plasmacore_obj, m_jniWebViewGetLoaded, id );
  BARD_PUSH_INT32( setting ? 1 : 0 );
}

void WebView__failed()
{
  GET_JAVA_ENV();
  int id = get_webview_index( BARD_POP_REF() );
  jboolean setting = java_env->CallBooleanMethod( plasmacore_obj, m_jniWebViewGetFailed, id );
  BARD_PUSH_INT32( setting ? 1 : 0 );
}

