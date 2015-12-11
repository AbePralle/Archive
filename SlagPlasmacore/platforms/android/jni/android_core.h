#ifndef ANDROID_CORE_H
#define ANDROID_CORE_H
//=============================================================================
// android_core.h
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
#include "plasmacore.h"
#include <jni.h>

extern jclass  class_CustomPlasmacore;
extern jobject plasmacore_obj;
extern JavaVM* jvm;

#define GET_JAVA_ENV() JNIEnv* java_env; jvm->AttachCurrentThread( &java_env, 0 )

void LOG( const char* st );
void LOG( BardString* string_obj );

struct JavaByteArray
{
  jbyteArray array;
  int count;
  char* data;

  JavaByteArray()
  {
    count = 0;
    array = NULL;
    data = NULL;
  }

  JavaByteArray( jbyteArray array )
  {
    data = NULL;
    retain( array );
  }

  ~JavaByteArray() { release(); }

  void retain( jbyteArray array )
  {
    // retain is automatically called for you on creation
    GET_JAVA_ENV();
    this->array = array;
    count = java_env->GetArrayLength(array);
    if ( !data ) data = (char*) java_env->GetByteArrayElements( array, NULL );
  }

  void release()
  {
    if (data)
    {
      GET_JAVA_ENV();
      java_env->ReleaseByteArrayElements( array, (jbyte*)data, 0 );
      data = NULL;
    }
  }
};

struct JavaIntArray
{
  jintArray array;
  int count;
  jint* data;

  JavaIntArray()
  {
    count = 0;
    array = NULL;
    data = NULL;
  }

  JavaIntArray( jintArray array )
  {
    data = NULL;
    retain( array );
  }

  ~JavaIntArray() { release(); }

  void retain( jintArray array )
  {
    // retain is automatically called for you on creation
    GET_JAVA_ENV();
    this->array = array;
    count = java_env->GetArrayLength(array);
    if ( !data ) data = (jint*) java_env->GetIntArrayElements( array, NULL );
  }

  void release()
  {
    if (data)
    {
      GET_JAVA_ENV();
      java_env->ReleaseIntArrayElements( array, (jint*)data, 0 );
      data = NULL;
    }
  }
};

struct JavaStringArray
{
  jobjectArray array;
  int count;

  JavaStringArray( jobjectArray array )
  {
    GET_JAVA_ENV();
    this->array = array;
    count = java_env->GetArrayLength(array);
  }

  BardString* operator[]( int index )
  {
    GET_JAVA_ENV();
    jstring jstr = (jstring) java_env->GetObjectArrayElement( array, index );
    const char* string_value = java_env->GetStringUTFChars( jstr, NULL );
    BardString* result = (BardString*) BardString::create((char*)string_value);
    java_env->ReleaseStringUTFChars( jstr, string_value );
    java_env->DeleteLocalRef( jstr );
    return result;
  }
};

BardString* to_bard_string( jstring jst );
jstring to_jstring( BardObject* string_obj );

#endif // ANDROID_CORE_H
