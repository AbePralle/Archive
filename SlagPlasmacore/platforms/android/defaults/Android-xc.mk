# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_LDLIBS := -L${SYSROOT}/usr/lib -lz -lGLESv2

NATIVE_LIBS := ../../../../libraries/native

LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(NATIVE_LIBS)/plasmacore
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(NATIVE_LIBS)/bard
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(NATIVE_LIBS)/zlib-1.2.5/contrib/minizip
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(NATIVE_LIBS)/libpng-1.5.4
LOCAL_CFLAGS += -D BARD_XC -D BARD_USE_LONGJMP
LOCAL_CFLAGS += -D fseeko64=fseeko
LOCAL_CFLAGS += -D ftello64=ftello
LOCAL_CFLAGS += -D fopen64=fopen
LOCAL_CFLAGS += -fno-strict-aliasing
# no strict aliasing is essential for the Bard VM!

LOCAL_MODULE    := $(JNI_PROJECT_ID)

LOCAL_SRC_FILES := \
	game_xc.cpp \
	android_core.cpp \
	custom.cpp \
  $(NATIVE_LIBS)/plasmacore/plasmacore.cpp \
  $(NATIVE_LIBS)/plasmacore/gl_core.cpp \
  $(NATIVE_LIBS)/bard/bard_runtime.cpp \
  $(NATIVE_LIBS)/bard/bard_xc.cpp \
  $(NATIVE_LIBS)/bard/bard_stdlib.cpp \
  $(NATIVE_LIBS)/bard/bard_mm.cpp \
  $(NATIVE_LIBS)/plasmacore/font_system_17.cpp \
  $(NATIVE_LIBS)/zlib-1.2.5/contrib/minizip/unzip.c \
  $(NATIVE_LIBS)/zlib-1.2.5/contrib/minizip/ioapi.c \
  $(NATIVE_LIBS)/libpng-1.5.4/png.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngerror.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngget.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngmem.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngpread.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngread.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngrio.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngrtran.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngrutil.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngset.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngtrans.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngwio.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngwrite.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngwtran.c \
  $(NATIVE_LIBS)/libpng-1.5.4/pngwutil.c

include $(BUILD_SHARED_LIBRARY)

