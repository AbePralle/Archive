#pragma once

//=============================================================================
//  Boss.h
//
//  2015.08.01 by Abe Pralle
//=============================================================================

#if !defined(BOSS_STACK_SIZE)
#  define BOSS_STACK_SIZE 4096
#endif

#if defined(_WIN32)
#  define BOSS_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
#  define BOS_PLATFORM_MAC 1
#else
#  define BOS_PLATFORM_GENERIC 1
#endif

#if defined(BOSS_PLATFORM_WINDOWS)
#  include <windows.h>
#else
#  include <cstdint>
#endif

#include <cstdio>
using namespace std;

#include <stdlib.h>
#include <string.h>

#ifdef BOSS_PLATFORM_ANDROID
#  include <android/log.h>
#  define BOSS_LOG(...) __android_log_print( ANDROID_LOG_ERROR, "Boss", __VA_ARGS__ )
#else
#  define BOSS_LOG(...) printf( __VA_ARGS__ )
#endif

#include <setjmp.h>

#if defined(BOSS_PLATFORM_MAC)
#  define BOSS_SETJMP  _setjmp
#  define BOSS_LONGJMP _longjmp
#else
#  define BOSS_SETJMP  setjmp
#  define BOSS_LONGJMP longjmp
#endif

#define BOSS_TRY(vm) \
  { \
    JumpBuffer local_jump_buffer; \
    local_jump_buffer.previous_jump_buffer = vm->jump_buffer; \
    vm->jump_buffer = &local_jump_buffer; \
    int local_exception_code; \
    if ( !(local_exception_code = BOSS_SETJMP(local_jump_buffer.info)) ) \
    { \

#define BOSS_CATCH_ANY \
    } else {

#define BOSS_CATCH(err) \
    } else { \
      err = local_exception_code;

#define BOSS_END_TRY(vm) \
    } \
    vm->jump_buffer = local_jump_buffer.previous_jump_buffer; \
  }

#define BOSS_THROW(vm,code) BOSS_LONGJMP( vm->jump_buffer->info, code )

namespace Boss
{

#if defined(BOSS_PLATFORM_WINDOWS)
  typedef double           Real;
  typedef float            Float;
  typedef __int64          Long;
  typedef __int32          Integer;
  typedef unsigned __int16 Character;
  typedef unsigned char    Byte;
  typedef bool             Logical;
#else
  typedef double           Real;
  typedef float            Float;
  typedef int64_t          Long;
  typedef int32_t          Integer;
  typedef uint16_t         Character;
  typedef uint8_t          Byte;
  typedef bool             Logical;
#endif

struct Array;
struct Cmd;
struct CmdAccess;
struct List;
struct Table;
struct TableEntry;

//-----------------------------------------------------------------------------
//  Jump Buffer
//-----------------------------------------------------------------------------
struct JumpBuffer
{
  jmp_buf     info;
  JumpBuffer* previous_jump_buffer;
};


//-----------------------------------------------------------------------------
//  Allocator
//-----------------------------------------------------------------------------
#ifndef BOSSMM_PAGE_SIZE
// 4k; should be a multiple of 256 if redefined
#  define BOSSMM_PAGE_SIZE (4*1024)
#endif

// 0 = large allocations, 1..4 = block sizes 64, 128, 192, 256
#ifndef BOSSMM_SLOT_COUNT
#  define BOSSMM_SLOT_COUNT 5
#endif

// 2^6 = 64
#ifndef BOSSMM_GRANULARITY_BITS
#  define BOSSMM_GRANULARITY_BITS 6
#endif

// Block sizes increase by 64 bytes per slot
#ifndef BOSSMM_GRANULARITY_SIZE
#  define BOSSMM_GRANULARITY_SIZE (1 << BOSSMM_GRANULARITY_BITS)
#endif

// 63
#ifndef BOSSMM_GRANULARITY_MASK
#  define BOSSMM_GRANULARITY_MASK (BOSSMM_GRANULARITY_SIZE - 1)
#endif

// Small allocation limit is 256 bytes - afterwards objects are allocated
// from the system.
#ifndef BOSSMM_SMALL_ALLOCATION_SIZE_LIMIT
#  define BOSSMM_SMALL_ALLOCATION_SIZE_LIMIT  ((BOSSMM_SLOT_COUNT-1) << BOSSMM_GRANULARITY_BITS)
#endif

//-----------------------------------------------------------------------------
//  FORWARD DECLARATIONS
//-----------------------------------------------------------------------------
struct VM;
struct Object;
struct ParseReader;
struct Scope;
struct StringBuilder;
struct String;
struct Tokenizer;


//-----------------------------------------------------------------------------
//  EXCEPTION CODES
//-----------------------------------------------------------------------------
enum
{
  COMPILE_ERROR   = 1,
  RUNTIME_ERROR,
};

//-----------------------------------------------------------------------------
//  EXECUTION CODES
//-----------------------------------------------------------------------------
enum Status
{
  STATUS_NORMAL,
};


}; // namespace Boss

#include "BossAllocator.h"
#include "BossSystemObject.h"
#include "BossSystemList.h"
#include "BossSystemTable.h"
#include "BossValue.h"
#include "BossAttributes.h"
#include "BossType.h"
#include "BossObject.h"
#include "BossString.h"
#include "BossStringBuilder.h"
#include "BossArray.h"
#include "BossList.h"
#include "BossTable.h"
#include "BossTokenType.h"
#include "BossCmdQueue.h"
#include "BossCmd.h"
#include "BossParseReader.h"
#include "BossTokenizer.h"
#include "BossParser.h"
#include "BossUTF8.h"
#include "BossScope.h"
#include "BossMethod.h"
#include "BossVM.h"
#include "BossUtility.h"

