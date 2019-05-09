#include <stdio.h>
namespace std {}
using namespace std;
#include "Boss.h"

//=============================================================================
//  NativeCPP.cpp
//
//  Rogue runtime routines.
//=============================================================================

#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>
#include <exception>
#include <cstddef>

#if defined(ROGUE_PLATFORM_WINDOWS)
#  include <sys/timeb.h>
#else
#  include <sys/time.h>
#  include <unistd.h>
#  include <signal.h>
#  include <dirent.h>
#  include <sys/socket.h>
#  include <sys/uio.h>
#  include <sys/stat.h>
#  include <netdb.h>
#  include <errno.h>
#endif

#if defined(ANDROID)
  #include <netinet/in.h>
#endif

#if defined(_WIN32)
#  include <direct.h>
#  define chdir _chdir
#endif

#if TARGET_OS_IPHONE
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif

//-----------------------------------------------------------------------------
//  GLOBAL PROPERTIES
//-----------------------------------------------------------------------------
bool               Rogue_gc_logging   = false;
int                Rogue_gc_threshold = ROGUE_GC_THRESHOLD_DEFAULT;
int                Rogue_gc_count     = 0; // Purely informational
bool               Rogue_gc_requested = false;
bool               Rogue_gc_active    = false; // Are we collecting right now?
RogueLogical       Rogue_configured = 0;
int                Rogue_argc;
const char**       Rogue_argv;
RogueCallbackInfo  Rogue_on_gc_begin;
RogueCallbackInfo  Rogue_on_gc_trace_finished;
RogueCallbackInfo  Rogue_on_gc_end;
char               RogueDebugTrace::buffer[512];
ROGUE_THREAD_LOCAL RogueDebugTrace* Rogue_call_stack = 0;

struct RogueWeakReference;
RogueWeakReference* Rogue_weak_references = 0;

//-----------------------------------------------------------------------------
//  Multithreading
//-----------------------------------------------------------------------------
#if ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_PTHREADS

#define ROGUE_MUTEX_LOCK(_M) pthread_mutex_lock(&(_M))
#define ROGUE_MUTEX_UNLOCK(_M) pthread_mutex_unlock(&(_M))
#define ROGUE_MUTEX_DEF(_N) pthread_mutex_t _N = PTHREAD_MUTEX_INITIALIZER

#define ROGUE_COND_STARTWAIT(_V,_M) ROGUE_MUTEX_LOCK(_M);
#define ROGUE_COND_DOWAIT(_V,_M,_C) while (_C) pthread_cond_wait(&(_V), &(_M));
#define ROGUE_COND_ENDWAIT(_V,_M) ROGUE_MUTEX_UNLOCK(_M);
#define ROGUE_COND_WAIT(_V,_M,_C) \
  ROGUE_COND_STARTWAIT(_V,_M); \
  ROGUE_COND_DOWAIT(_V,_M,_C); \
  ROGUE_COND_ENDWAIT(_V,_M);
#define ROGUE_COND_DEF(_N) pthread_cond_t _N = PTHREAD_COND_INITIALIZER
#define ROGUE_COND_NOTIFY_ONE(_V,_M,_C)    \
  ROGUE_MUTEX_LOCK(_M);                    \
  _C ;                                     \
  pthread_cond_signal(&(_V));              \
  ROGUE_MUTEX_UNLOCK(_M);
#define ROGUE_COND_NOTIFY_ALL(_V,_M,_C)    \
  ROGUE_MUTEX_LOCK(_M);                    \
  _C ;                                     \
  pthread_cond_broadcast(&(_V));           \
  ROGUE_MUTEX_UNLOCK(_M);

#define ROGUE_THREAD_DEF(_N) pthread_t _N
#define ROGUE_THREAD_JOIN(_T) pthread_join(_T, NULL)
#define ROGUE_THREAD_START(_T, _F) pthread_create(&(_T), NULL, _F, NULL)

#elif ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_CPP

#include <exception>
#include <condition_variable>

#define ROGUE_MUTEX_LOCK(_M) _M.lock()
#define ROGUE_MUTEX_UNLOCK(_M) _M.unlock()
#define ROGUE_MUTEX_DEF(_N) std::mutex _N

#define ROGUE_COND_STARTWAIT(_V,_M) { std::unique_lock<std::mutex> LK(_M);
#define ROGUE_COND_DOWAIT(_V,_M,_C) while (_C) (_V).wait(LK);
#define ROGUE_COND_ENDWAIT(_V,_M) }
#define ROGUE_COND_WAIT(_V,_M,_C) \
  ROGUE_COND_STARTWAIT(_V,_M); \
  ROGUE_COND_DOWAIT(_V,_M,_C); \
  ROGUE_COND_ENDWAIT(_V,_M);
#define ROGUE_COND_DEF(_N) std::condition_variable _N
#define ROGUE_COND_NOTIFY_ONE(_V,_M,_C) {  \
  std::unique_lock<std::mutex> LK2(_M);    \
  _C ;                                     \
  (_V).notify_one(); }
#define ROGUE_COND_NOTIFY_ALL(_V,_M,_C) {  \
  std::unique_lock<std::mutex> LK2(_M);    \
  _C ;                                     \
  (_V).notify_all(); }

#define ROGUE_THREAD_DEF(_N) std::thread _N
#define ROGUE_THREAD_JOIN(_T) (_T).join()
#define ROGUE_THREAD_START(_T, _F) (_T = std::thread([] () {_F(NULL);}),0)

#endif

#if ROGUE_THREAD_MODE != ROGUE_THREAD_MODE_NONE

// Thread mutex locks around creation and destruction of threads
static ROGUE_MUTEX_DEF(Rogue_mt_thread_mutex);
static int Rogue_mt_tc = 0; // Thread count.  Always set under above lock.
static std::atomic_bool Rogue_mt_terminating(false); // True when terminating.

static void Rogue_thread_register ()
{
  ROGUE_MUTEX_LOCK(Rogue_mt_thread_mutex);
#if ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_PTHREADS
  int n = (int)Rogue_mt_tc;
#endif
  ++Rogue_mt_tc;
  ROGUE_MUTEX_UNLOCK(Rogue_mt_thread_mutex);
#if ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_PTHREADS
  char name[64];
  sprintf(name, "Thread-%i", n); // Nice names are good for valgrind

#if ROGUE_PLATFORM_MACOS
  pthread_setname_np(name);
#elif __linux__
  pthread_setname_np(pthread_self(), name);
#endif
// It should be possible to get thread names working on lots of other
// platforms too.  The functions just vary a bit.
#endif
}

static void Rogue_thread_unregister ()
{
  ROGUE_EXIT;
  ROGUE_MUTEX_LOCK(Rogue_mt_thread_mutex);
  ROGUE_ENTER;
  --Rogue_mt_tc;
  ROGUE_MUTEX_UNLOCK(Rogue_mt_thread_mutex);
}


#define ROGUE_THREADS_WAIT_FOR_ALL Rogue_threads_wait_for_all();

void Rogue_threads_wait_for_all ()
{
  Rogue_mt_terminating = true;
  ROGUE_EXIT;
  int wait = 2; // Initial Xms
  int wait_step = 1;
  while (true)
  {
    ROGUE_MUTEX_LOCK(Rogue_mt_thread_mutex);
    if (Rogue_mt_tc <= 1) // Shouldn't ever really be less than 1
    {
      ROGUE_MUTEX_UNLOCK(Rogue_mt_thread_mutex);
      break;
    }
    ROGUE_MUTEX_UNLOCK(Rogue_mt_thread_mutex);
    usleep(1000 * wait);
    wait_step++;
    if (!(wait_step % 15) && (wait < 500)) wait *= 2; // Max backoff ~500ms
  }
  ROGUE_ENTER;
}

#else

#define ROGUE_THREADS_WAIT_FOR_ALL /* no-op if there's only one thread! */

static void Rogue_thread_register ()
{
}
static void Rogue_thread_unregister ()
{
}

#endif

// Singleton handling
#if ROGUE_THREAD_MODE
#define ROGUE_GET_SINGLETON(_S) (_S)->_singleton.load()
#define ROGUE_SET_SINGLETON(_S,_V) (_S)->_singleton.store(_V);
#if ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_PTHREADS
pthread_mutex_t Rogue_thread_singleton_lock;
#define ROGUE_SINGLETON_LOCK ROGUE_MUTEX_LOCK(Rogue_thread_singleton_lock);
#define ROGUE_SINGLETON_UNLOCK ROGUE_MUTEX_UNLOCK(Rogue_thread_singleton_lock);
#else
std::recursive_mutex Rogue_thread_singleton_lock;
#define ROGUE_SINGLETON_LOCK Rogue_thread_singleton_lock.lock();
#define ROGUE_SINGLETON_UNLOCK Rogue_thread_singleton_lock.unlock();
#endif
#else
#define ROGUE_GET_SINGLETON(_S) (_S)->_singleton
#define ROGUE_SET_SINGLETON(_S,_V) (_S)->_singleton = _V;
#define ROGUE_SINGLETON_LOCK
#define ROGUE_SINGLETON_UNLOCK
#endif

//-----------------------------------------------------------------------------
//  GC
//-----------------------------------------------------------------------------
#if ROGUE_GC_MODE_AUTO_MT
// See the Rogue MT GC diagram for an explanation of some of this.

#define ROGUE_GC_VAR static volatile int
// (Curiously, volatile seems to help performance slightly.)

static thread_local bool Rogue_mtgc_is_gc_thread = false;

#define ROGUE_MTGC_BARRIER asm volatile("" : : : "memory");

// Atomic LL insertion
#define ROGUE_LINKED_LIST_INSERT(__OLD,__NEW,__NEW_NEXT)            \
  for(;;) {                                                         \
    auto tmp = __OLD;                                               \
    __NEW_NEXT = tmp;                                               \
    if (__sync_bool_compare_and_swap(&(__OLD), tmp, __NEW)) break;  \
  }

// We assume malloc is safe, but the SOA needs safety if it's being used.
#if ROGUEMM_SMALL_ALLOCATION_SIZE_LIMIT >= 0
static ROGUE_MUTEX_DEF(Rogue_mtgc_soa_mutex);
#define ROGUE_GC_SOA_LOCK    ROGUE_MUTEX_LOCK(Rogue_mtgc_soa_mutex);
#define ROGUE_GC_SOA_UNLOCK  ROGUE_MUTEX_UNLOCK(Rogue_mtgc_soa_mutex);
#else
#define ROGUE_GC_SOA_LOCK
#define ROGUE_GC_SOA_UNLOCK
#endif

static inline void Rogue_collect_garbage_real ();
void Rogue_collect_garbage_real_noinline ()
{
  Rogue_collect_garbage_real();
}

#if ROGUE_THREAD_MODE
#if ROGUE_THREAD_MODE_PTHREADS
#elif ROGUE_THREAD_MODE_CPP
#else
#error Currently, only --threads=pthreads and --threads=cpp are supported with --gc=auto-mt
#endif
#endif

// This is how unlikely() works in the Linux kernel
#define ROGUE_UNLIKELY(_X) __builtin_expect(!!(_X), 0)

#define ROGUE_GC_CHECK if (ROGUE_UNLIKELY(Rogue_mtgc_w) \
  && !ROGUE_UNLIKELY(Rogue_mtgc_is_gc_thread))          \
  Rogue_mtgc_W2_W3_W4(); // W1

 ROGUE_MUTEX_DEF(Rogue_mtgc_w_mutex);
static ROGUE_MUTEX_DEF(Rogue_mtgc_s_mutex);
static ROGUE_COND_DEF(Rogue_mtgc_w_cond);
static ROGUE_COND_DEF(Rogue_mtgc_s_cond);

ROGUE_GC_VAR Rogue_mtgc_w = 0;
ROGUE_GC_VAR Rogue_mtgc_s = 0;

// Only one worker can be "running" (waiting for) the GC at a time.
// To run, set r = 1, and wait for GC to set it to 0.  If r is already
// 1, just wait.
static ROGUE_MUTEX_DEF(Rogue_mtgc_r_mutex);
static ROGUE_COND_DEF(Rogue_mtgc_r_cond);
ROGUE_GC_VAR Rogue_mtgc_r = 0;

static ROGUE_MUTEX_DEF(Rogue_mtgc_g_mutex);
static ROGUE_COND_DEF(Rogue_mtgc_g_cond);
ROGUE_GC_VAR Rogue_mtgc_g = 0; // Should GC

static int Rogue_mtgc_should_quit = 0; // 0:normal 1:should-quit 2:has-quit

static ROGUE_THREAD_DEF(Rogue_mtgc_thread);

static void Rogue_mtgc_W2_W3_W4 (void);
static inline void Rogue_mtgc_W3_W4 (void);

inline void Rogue_mtgc_B1 ()
{
  ROGUE_COND_NOTIFY_ONE(Rogue_mtgc_s_cond, Rogue_mtgc_s_mutex, ++Rogue_mtgc_s);
}

static inline void Rogue_mtgc_B2_etc ()
{
  Rogue_mtgc_W3_W4();
  // We can probably just do GC_CHECK here rather than this more expensive
  // locking version.
  ROGUE_MUTEX_LOCK(Rogue_mtgc_w_mutex);
  auto w = Rogue_mtgc_w;
  ROGUE_MUTEX_UNLOCK(Rogue_mtgc_w_mutex);
  if (ROGUE_UNLIKELY(w)) Rogue_mtgc_W2_W3_W4(); // W1
}

static inline void Rogue_mtgc_W3_W4 ()
{
  // W3
  ROGUE_COND_WAIT(Rogue_mtgc_w_cond, Rogue_mtgc_w_mutex, Rogue_mtgc_w != 0);

  // W4
  ROGUE_MUTEX_LOCK(Rogue_mtgc_s_mutex);
  --Rogue_mtgc_s;
  ROGUE_MUTEX_UNLOCK(Rogue_mtgc_s_mutex);
}

static void Rogue_mtgc_W2_W3_W4 ()
{
  // W2
  ROGUE_COND_NOTIFY_ONE(Rogue_mtgc_s_cond, Rogue_mtgc_s_mutex, ++Rogue_mtgc_s);
  Rogue_mtgc_W3_W4();
}


static thread_local int Rogue_mtgc_entered = 1;

inline void Rogue_mtgc_enter()
{
  if (ROGUE_UNLIKELY(Rogue_mtgc_is_gc_thread)) return;
  if (ROGUE_UNLIKELY(Rogue_mtgc_entered))
#ifdef ROGUE_MTGC_DEBUG
  {
    ROGUE_LOG_ERROR("ALREADY ENTERED\n");
    exit(1);
  }
#else
  {
    ++Rogue_mtgc_entered;
    return;
  }
#endif

  Rogue_mtgc_entered = 1;
  Rogue_mtgc_B2_etc();
}

inline void Rogue_mtgc_exit()
{
  if (ROGUE_UNLIKELY(Rogue_mtgc_is_gc_thread)) return;
  if (ROGUE_UNLIKELY(Rogue_mtgc_entered <= 0))
  {
    ROGUE_LOG_ERROR("Unabalanced Rogue enter/exit\n");
    exit(1);
  }

  --Rogue_mtgc_entered;
  Rogue_mtgc_B1();
}

static void Rogue_mtgc_M1_M2_GC_M3 (int quit)
{
  // M1
  ROGUE_MUTEX_LOCK(Rogue_mtgc_w_mutex);
  Rogue_mtgc_w = 1;
  ROGUE_MUTEX_UNLOCK(Rogue_mtgc_w_mutex);

  // M2
#if (ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_PTHREADS) && ROGUE_MTGC_DEBUG
  ROGUE_MUTEX_LOCK(Rogue_mtgc_s_mutex);
  while (Rogue_mtgc_s != Rogue_mt_tc)
  {
    if (Rogue_mtgc_s > Rogue_mt_tc || Rogue_mtgc_s < 0)
    {
      ROGUE_LOG_ERROR("INVALID VALUE OF S %i %i\n", Rogue_mtgc_s, Rogue_mt_tc);
      exit(1);
    }

    pthread_cond_wait(&Rogue_mtgc_s_cond, &Rogue_mtgc_s_mutex);
  }
  // We should actually be okay holding the S lock until the
  // very end of the function if we want, and this would prevent
  // threads that were blocking from ever leaving B2.  But
  // We should be okay anyway, though S may temporarily != TC.
  //ROGUE_MUTEX_UNLOCK(Rogue_mtgc_s_mutex);
#else
  ROGUE_COND_STARTWAIT(Rogue_mtgc_s_cond, Rogue_mtgc_s_mutex);
  ROGUE_COND_DOWAIT(Rogue_mtgc_s_cond, Rogue_mtgc_s_mutex, Rogue_mtgc_s != Rogue_mt_tc);
#endif

#if ROGUE_MTGC_DEBUG
  ROGUE_MUTEX_LOCK(Rogue_mtgc_w_mutex);
  Rogue_mtgc_w = 2;
  ROGUE_MUTEX_UNLOCK(Rogue_mtgc_w_mutex);
#endif

  // GC
  // Grab the SOA lock for symmetry.  It should actually never
  // be held by another thread since they're all in GC sleep.
  ROGUE_GC_SOA_LOCK;
  Rogue_collect_garbage_real();

  //NOTE: It's possible (Rogue_mtgc_s != Rogue_mt_tc) here, if we gave up the S
  //      lock, though they should quickly go back to equality.

  if (quit)
  {
    // Run a few more times to finish up
    Rogue_collect_garbage_real_noinline();
    Rogue_collect_garbage_real_noinline();

    // Free from the SOA
    RogueAllocator_free_all();
  }
  ROGUE_GC_SOA_UNLOCK;

  // M3
  ROGUE_COND_NOTIFY_ALL(Rogue_mtgc_w_cond, Rogue_mtgc_w_mutex, Rogue_mtgc_w = 0);

  // Could have done this much earlier
  ROGUE_COND_ENDWAIT(Rogue_mtgc_s_cond, Rogue_mtgc_s_mutex);
}

static void * Rogue_mtgc_threadproc (void *)
{
  Rogue_mtgc_is_gc_thread = true;
  int quit = 0;
  while (quit == 0)
  {
    ROGUE_COND_STARTWAIT(Rogue_mtgc_g_cond, Rogue_mtgc_g_mutex);
    ROGUE_COND_DOWAIT(Rogue_mtgc_g_cond, Rogue_mtgc_g_mutex, !Rogue_mtgc_g && !Rogue_mtgc_should_quit);
    Rogue_mtgc_g = 0;
    quit = Rogue_mtgc_should_quit;
    ROGUE_COND_ENDWAIT(Rogue_mtgc_g_cond, Rogue_mtgc_g_mutex);

    ROGUE_MUTEX_LOCK(Rogue_mt_thread_mutex);

    Rogue_mtgc_M1_M2_GC_M3(quit);

    ROGUE_MUTEX_UNLOCK(Rogue_mt_thread_mutex);

    ROGUE_COND_NOTIFY_ALL(Rogue_mtgc_r_cond, Rogue_mtgc_r_mutex, Rogue_mtgc_r = 0);
  }

  ROGUE_MUTEX_LOCK(Rogue_mtgc_g_mutex);
  Rogue_mtgc_should_quit = 2;
  Rogue_mtgc_g = 0;
  ROGUE_MUTEX_UNLOCK(Rogue_mtgc_g_mutex);
  return NULL;
}

// Cause GC to run and wait for a GC to complete.
void Rogue_mtgc_run_gc_and_wait ()
{
  bool again;
  do
  {
    again = false;
    ROGUE_COND_STARTWAIT(Rogue_mtgc_r_cond, Rogue_mtgc_r_mutex);
    if (Rogue_mtgc_r == 0)
    {
      Rogue_mtgc_r = 1;

      // Signal GC to run
      ROGUE_COND_NOTIFY_ONE(Rogue_mtgc_g_cond, Rogue_mtgc_g_mutex, Rogue_mtgc_g = 1);
    }
    else
    {
      // If one or more simultaneous requests to run the GC came in, run it
      // again.
      again = (Rogue_mtgc_r == 1);
      ++Rogue_mtgc_r;
    }
    ROGUE_EXIT;
    ROGUE_COND_DOWAIT(Rogue_mtgc_r_cond, Rogue_mtgc_r_mutex, Rogue_mtgc_r != 0);
    ROGUE_COND_ENDWAIT(Rogue_mtgc_r_cond, Rogue_mtgc_r_mutex);
    ROGUE_ENTER;
  }
  while (again);
}

static void Rogue_mtgc_quit_gc_thread ()
{
  //NOTE: This could probably be simplified (and the quit behavior removed
  //      from Rogue_mtgc_M1_M2_GC_M3) since we now wait for all threads
  //      to stop before calling this.
  // This doesn't quite use the normal condition variable pattern, sadly.
  ROGUE_EXIT;
  timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 1000000 * 10; // 10ms
  while (true)
  {
    bool done = true;
    ROGUE_COND_STARTWAIT(Rogue_mtgc_g_cond, Rogue_mtgc_g_mutex);
    if (Rogue_mtgc_should_quit != 2)
    {
      done = false;
      Rogue_mtgc_g = 1;
      Rogue_mtgc_should_quit = 1;
#if ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_PTHREADS
      pthread_cond_signal(&Rogue_mtgc_g_cond);
#elif ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_CPP
      Rogue_mtgc_g_cond.notify_one();
#endif
    }
    ROGUE_COND_ENDWAIT(Rogue_mtgc_g_cond, Rogue_mtgc_g_mutex);
    if (done) break;
    nanosleep(&ts, NULL);
  }
  ROGUE_THREAD_JOIN(Rogue_mtgc_thread);
  ROGUE_ENTER;
}

void Rogue_configure_gc()
{
  int c = ROGUE_THREAD_START(Rogue_mtgc_thread, Rogue_mtgc_threadproc);
  if (c != 0)
  {
    exit(77); //TODO: Do something better in this (hopefully) rare case.
  }
}

// Used as part of the ROGUE_BLOCKING_CALL macro.
template<typename RT> RT Rogue_mtgc_reenter (RT expr)
{
  ROGUE_ENTER;
  return expr;
}

#include <atomic>

// We do all relaxed operations on this.  It's possible this will lead to
// something "bad", but I don't think *too* bad.  Like an extra GC.
// And I think that'll be rare, since the reset happens when all the
// threads are synced.  But I could be wrong.  Should probably think
// about this harder.
std::atomic_int Rogue_allocation_bytes_until_gc(Rogue_gc_threshold);
#define ROGUE_GC_COUNT_BYTES(__x) Rogue_allocation_bytes_until_gc.fetch_sub(__x, std::memory_order_relaxed);
#define ROGUE_GC_AT_THRESHOLD (Rogue_allocation_bytes_until_gc.load(std::memory_order_relaxed) <= 0)
#define ROGUE_GC_RESET_COUNT Rogue_allocation_bytes_until_gc.store(Rogue_gc_threshold, std::memory_order_relaxed);

#else // Anything besides auto-mt

#define ROGUE_GC_CHECK /* Does nothing in non-auto-mt modes */

#define ROGUE_GC_SOA_LOCK
#define ROGUE_GC_SOA_UNLOCK

int Rogue_allocation_bytes_until_gc = Rogue_gc_threshold;
#define ROGUE_GC_COUNT_BYTES(__x) Rogue_allocation_bytes_until_gc -= (__x);
#define ROGUE_GC_AT_THRESHOLD (Rogue_allocation_bytes_until_gc <= 0)
#define ROGUE_GC_RESET_COUNT Rogue_allocation_bytes_until_gc = Rogue_gc_threshold;


#define ROGUE_MTGC_BARRIER
#define ROGUE_LINKED_LIST_INSERT(__OLD,__NEW,__NEW_NEXT) do {__NEW_NEXT = __OLD; __OLD = __NEW;} while(false)

#endif

//-----------------------------------------------------------------------------
//  Misc Utility
//-----------------------------------------------------------------------------
void Rogue_define_literal_string( int index, const char* st, int count=-1 );

void Rogue_define_literal_string( int index, const char* st, int count )
{
  Rogue_literal_strings[index] = (RogueString*) RogueObject_retain( RogueString_create_from_utf8( st, count ) );
}

//-----------------------------------------------------------------------------
//  RogueDebugTrace
//-----------------------------------------------------------------------------
RogueDebugTrace::RogueDebugTrace( const char* method_signature, const char* filename, int line )
  : method_signature(method_signature), filename(filename), line(line), previous_trace(0)
{
  previous_trace = Rogue_call_stack;
  Rogue_call_stack = this;
}

RogueDebugTrace::~RogueDebugTrace()
{
  Rogue_call_stack = previous_trace;
}

int RogueDebugTrace::count()
{
  int n = 1;
  RogueDebugTrace* current = previous_trace;
  while (current)
  {
    ++n;
    current = current->previous_trace;
  }
  return n;
}

char* RogueDebugTrace::to_c_string()
{
  snprintf( buffer, 512, "[%s %s:%d]", method_signature, filename, line );
  return buffer;
}

//-----------------------------------------------------------------------------
//  RogueType
//-----------------------------------------------------------------------------
RogueArray* RogueType_create_array( int count, int element_size, bool is_reference_array, int element_type_index )
{
  if (count < 0) count = 0;
  int data_size  = count * element_size;
  int total_size = sizeof(RogueArray) + data_size;

  RogueArray* array = (RogueArray*) RogueAllocator_allocate_object( RogueTypeArray->allocator, RogueTypeArray, total_size, element_type_index);

  array->count = count;
  array->element_size = element_size;
  array->is_reference_array = is_reference_array;

  return array;
}

RogueObject* RogueType_create_object( RogueType* THIS, RogueInt32 size )
{
  ROGUE_DEF_LOCAL_REF_NULL(RogueObject*, obj);
  RogueInitFn  fn;
#if ROGUE_GC_MODE_BOEHM_TYPED
  ROGUE_DEBUG_STATEMENT(assert(size == 0 || size == THIS->object_size));
#endif

  obj = RogueAllocator_allocate_object( THIS->allocator, THIS, size ? size : THIS->object_size );

  if ((fn = THIS->init_object_fn)) return fn( obj );
  else                             return obj;
}

RogueLogical RogueType_instance_of( RogueType* THIS, RogueType* ancestor_type )
{
  if (THIS == ancestor_type)
  {
    return true;
  }

  int count = THIS->base_type_count;
  RogueType** base_type_ptr = THIS->base_types - 1;
  while (--count >= 0)
  {
    if (ancestor_type == *(++base_type_ptr))
    {
      return true;
    }
  }

  return false;
}

RogueString* RogueType_name( RogueType* THIS )
{
  return Rogue_literal_strings[ THIS->name_index ];
}

bool RogueType_name_equals( RogueType* THIS, const char* name )
{
  // For debugging purposes
  RogueString* st = Rogue_literal_strings[ THIS->name_index ];
  if ( !st ) return false;

  return (0 == strcmp((char*)st->utf8,name));
}

void RogueType_print_name( RogueType* THIS )
{
  RogueString* st = Rogue_literal_strings[ THIS->name_index ];
  if (st)
  {
    ROGUE_LOG( "%s", st->utf8 );
  }
}

RogueType* RogueType_retire( RogueType* THIS )
{
  if (THIS->base_types)
  {
#if !ROGUE_GC_MODE_BOEHM
    delete [] THIS->base_types;
#endif
    THIS->base_types = 0;
    THIS->base_type_count = 0;
  }

  return THIS;
}

RogueObject* RogueType_singleton( RogueType* THIS )
{
  RogueInitFn fn;
  RogueObject * r = ROGUE_GET_SINGLETON(THIS);
  if (r) return r;

  ROGUE_SINGLETON_LOCK;

#if ROGUE_THREAD_MODE // Very minor optimization: don't check twice if unthreaded
  // We probably need to initialize the singleton, but now that we have the
  // lock, we double check.
  r = ROGUE_GET_SINGLETON(THIS);
  if (r)
  {
    // Ah, someone else just initialized it.  We'll use that.
    ROGUE_SINGLETON_UNLOCK;
  }
  else
#endif
  {
    // Yes, we'll be the one doing the initializing.

    // NOTE: _singleton must be assigned before calling init_object()
    // so we can't just call RogueType_create_object().
    r = RogueAllocator_allocate_object( THIS->allocator, THIS, THIS->object_size );

    if ((fn = THIS->init_object_fn)) r = fn( r );

    ROGUE_SET_SINGLETON(THIS, r);

    ROGUE_SINGLETON_UNLOCK;

    if ((fn = THIS->init_fn)) r = fn( THIS->_singleton );
  }

  return r;
}

//-----------------------------------------------------------------------------
//  RogueObject
//-----------------------------------------------------------------------------
RogueObject* RogueObject_as( RogueObject* THIS, RogueType* specialized_type )
{
  if (RogueObject_instance_of(THIS,specialized_type)) return THIS;
  return 0;
}

RogueLogical RogueObject_instance_of( RogueObject* THIS, RogueType* ancestor_type )
{
  if ( !THIS )
  {
    return false;
  }

  return RogueType_instance_of( THIS->type, ancestor_type );
}

void* RogueObject_retain( RogueObject* THIS )
{
  ROGUE_INCREF(THIS);
  return THIS;
}

void* RogueObject_release( RogueObject* THIS )
{
  ROGUE_DECREF(THIS);
  return THIS;
}

RogueString* RogueObject_to_string( RogueObject* THIS )
{
  RogueToStringFn fn = THIS->type->to_string_fn;
  if (fn) return fn( THIS );

  return Rogue_literal_strings[ THIS->type->name_index ];
}

void RogueObject_trace( void* obj )
{
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
}

void RogueString_trace( void* obj )
{
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
}

void RogueArray_trace( void* obj )
{
  int count;
  RogueObject** src;
  RogueArray* array = (RogueArray*) obj;

  if ( !array || array->object_size < 0 ) return;
  array->object_size = ~array->object_size;

  if ( !array->is_reference_array ) return;

  count = array->count;
  src = array->as_objects + count;
  while (--count >= 0)
  {
    RogueObject* cur = *(--src);
    if (cur && cur->object_size >= 0)
    {
      cur->type->trace_fn( cur );
    }
  }
}

//-----------------------------------------------------------------------------
//  RogueString
//-----------------------------------------------------------------------------
RogueString* RogueString_create_with_byte_count( int byte_count )
{
  if (byte_count < 0) byte_count = 0;

#if ROGUE_GC_MODE_BOEHM_TYPED
  RogueString* st = (RogueString*) RogueAllocator_allocate_object( RogueTypeString->allocator, RogueTypeString, RogueTypeString->object_size );
  char * data = (char *)GC_malloc_atomic_ignore_off_page( byte_count + 1 );
  data[0] = 0;
  data[byte_count] = 0;
  st->utf8 = (RogueByte*)data;
#else
  int total_size = sizeof(RogueString) + (byte_count+1);

  RogueString* st = (RogueString*) RogueAllocator_allocate_object( RogueTypeString->allocator, RogueTypeString, total_size );
#endif
  st->byte_count = byte_count;

  return st;
}

RogueString* RogueString_create_from_utf8( const char* utf8, int count )
{
  if (count == -1) count = (int) strlen( utf8 );

  RogueString* st = RogueString_create_with_byte_count( count );
  memcpy( st->utf8, utf8, count );
  return RogueString_validate( st );
}

RogueString* RogueString_create_from_characters( RogueCharacter_List* characters )
{
  if ( !characters ) return RogueString_create_with_byte_count(0);

  RogueCharacter* data = characters->data->as_characters;
  int count = characters->count;
  int utf8_count = 0;
  for (int i=count; --i>=0; )
  {
    RogueCharacter ch = data[i];
    if (ch <= 0x7F)         ++utf8_count;
    else if (ch <= 0x7FF)   utf8_count += 2;
    else if (ch <= 0xFFFF)  utf8_count += 3;
    else                    utf8_count += 4;
  }

  RogueString* result = RogueString_create_with_byte_count( utf8_count );
  char*   dest = result->utf8;
  for (int i=0; i<count; ++i)
  {
    RogueCharacter ch = data[i];
    if (ch < 0)
    {
      *(dest++) = 0;
    }
    else if (ch <= 0x7F)
    {
      *(dest++) = (RogueByte) ch;
    }
    else if (ch <= 0x7FF)
    {
      dest[0] = (RogueByte) (0xC0 | ((ch >> 6) & 0x1F));
      dest[1] = (RogueByte) (0x80 | (ch & 0x3F));
      dest += 2;
    }
    else if (ch <= 0xFFFF)
    {
      dest[0] = (RogueByte) (0xE0 | ((ch >> 12) & 0xF));
      dest[1] = (RogueByte) (0x80 | ((ch >> 6) & 0x3F));
      dest[2] = (RogueByte) (0x80 | (ch & 0x3F));
      dest += 3;
    }
    else
    {
      dest[0] = (RogueByte) (0xF0 | ((ch >> 18) & 0x7));
      dest[1] = (RogueByte) (0x80 | ((ch >> 12) & 0x3F));
      dest[2] = (RogueByte) (0x80 | ((ch >> 6) & 0x3F));
      dest[3] = (RogueByte) (0x80 | (ch & 0x3F));
      dest += 4;
    }
  }

  result->character_count = count;

  return RogueString_validate( result );
}

void RogueString_print_string( RogueString* st )
{
  if (st)
  {
    RogueString_print_utf8( st->utf8, st->byte_count );
  }
  else
  {
    ROGUE_LOG( "null" );
  }
}

void RogueString_print_characters( RogueCharacter* characters, int count )
{
  if (characters)
  {
    RogueCharacter* src = characters - 1;
    while (--count >= 0)
    {
      int ch = *(++src);

      if (ch < 0)
      {
        putchar( 0 );
      }
      else if (ch < 0x80)
      {
        // %0xxxxxxx
        putchar( ch );
      }
      else if (ch < 0x800)
      {
        // %110xxxxx 10xxxxxx
        putchar( ((ch >> 6) & 0x1f) | 0xc0 );
        putchar( (ch & 0x3f) | 0x80 );
      }
      else if (ch <= 0xFFFF)
      {
        // %1110xxxx 10xxxxxx 10xxxxxx
        putchar( ((ch >> 12) & 15) | 0xe0 );
        putchar( ((ch >> 6) & 0x3f) | 0x80 );
        putchar( (ch & 0x3f) | 0x80 );
      }
      else
      {
        // Assume 21-bit
        // %11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        putchar( 0xf0 | ((ch>>18) & 7) );
        putchar( 0x80 | ((ch>>12) & 0x3f) );
        putchar( 0x80 | ((ch>>6)  & 0x3f) );
        putchar( (ch & 0x3f) | 0x80 );
      }
    }
  }
  else
  {
    ROGUE_LOG( "null" );
  }
}

void RogueString_print_utf8( char* utf8, int count )
{
  --utf8;
  while (--count >= 0)
  {
    putchar( *(++utf8) );
  }
}

RogueCharacter RogueString_character_at( RogueString* THIS, int index )
{
  if (THIS->is_ascii) return (RogueCharacter) THIS->utf8[ index ];

  RogueInt32 offset = RogueString_set_cursor( THIS, index );
  char* utf8 = THIS->utf8;

  RogueCharacter ch = utf8[ offset ];
  if (ch & 0x80)
  {
    if (ch & 0x20)
    {
      if (ch & 0x10)
      {
        return ((ch&7)<<18)
            | ((utf8[offset+1] & 0x3F) << 12)
            | ((utf8[offset+2] & 0x3F) << 6)
            | (utf8[offset+3] & 0x3F);
      }
      else
      {
        return ((ch&15)<<12)
            | ((utf8[offset+1] & 0x3F) << 6)
            | (utf8[offset+2] & 0x3F);
      }
    }
    else
    {
      return ((ch&31)<<6)
          | (utf8[offset+1] & 0x3F);
    }
  }
  else
  {
    return ch;
  }
}

RogueInt32 RogueString_set_cursor( RogueString* THIS, int index )
{
  // Sets this string's cursor_offset and cursor_index and returns cursor_offset.
  if (THIS->is_ascii)
  {
    return THIS->cursor_offset = THIS->cursor_index = index;
  }

  char* utf8 = THIS->utf8;

  RogueInt32 c_offset;
  RogueInt32 c_index;
  if (index == 0)
  {
    THIS->cursor_index = 0;
    return THIS->cursor_offset = 0;
  }
  else if (index >= THIS->character_count - 1)
  {
    c_offset = THIS->byte_count;
    c_index = THIS->character_count;
  }
  else
  {
    c_offset  = THIS->cursor_offset;
    c_index = THIS->cursor_index;
  }

  while (c_index < index)
  {
    while ((utf8[++c_offset] & 0xC0) == 0x80) {}
    ++c_index;
  }

  while (c_index > index)
  {
    while ((utf8[--c_offset] & 0xC0) == 0x80) {}
    --c_index;
  }

  THIS->cursor_index = c_index;
  return THIS->cursor_offset = c_offset;
}

RogueString* RogueString_validate( RogueString* THIS )
{
  // Trims any invalid UTF-8, counts the number of characters, and sets the hash code
  THIS->is_ascii = 1;  // assumption

  int character_count = 0;
  int byte_count = THIS->byte_count;
  int i;
  char* utf8 = THIS->utf8;
  for (i=0; i<byte_count; ++character_count)
  {
    int b = utf8[ i ];
    if (b & 0x80)
    {
      THIS->is_ascii = 0;
      if ( !(b & 0x40) ) { break;}  // invalid UTF-8

      if (b & 0x20)
      {
        if (b & 0x10)
        {
          // %11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
          if (b & 0x08) { break;}
          if (i + 4 > byte_count || ((utf8[i+1] & 0xC0) != 0x80) || ((utf8[i+2] & 0xC0) != 0x80)
              || ((utf8[i+3] & 0xC0) != 0x80)) { break;}
          i += 4;
        }
        else
        {
          // %1110xxxx 10xxxxxx 10xxxxxx
          if (i + 3 > byte_count || ((utf8[i+1] & 0xC0) != 0x80) || ((utf8[i+2] & 0xC0) != 0x80)) { break;}
          i += 3;
        }
      }
      else
      {
        // %110x xxxx 10xx xxxx
        if (i + 2 > byte_count || ((utf8[i+1] & 0xC0) != 0x80)) { break; }
        i += 2;
      }
    }
    else
    {
      ++i;
    }
  }

  //if (i != byte_count)
  //{
  //  ROGUE_LOG_ERROR( "*** RogueString validation error - invalid UTF8 (%d/%d):\n", i, byte_count );
  //  ROGUE_LOG_ERROR( "%02x\n", utf8[0] );
  //  ROGUE_LOG_ERROR( "%s\n", utf8 );
  //  utf8[ i ] = 0;
  //  Rogue_print_stack_trace();
  //}

  THIS->byte_count = i;
  THIS->character_count = character_count;

  int code = 0;
  int len = THIS->byte_count;
  char* src = THIS->utf8 - 1;
  while (--len >= 0)
  {
    code = ((code<<3) - code) + *(++src);
  }
  THIS->hash_code = code;
  return THIS;
}

//-----------------------------------------------------------------------------
//  RogueArray
//-----------------------------------------------------------------------------
RogueArray* RogueArray_set( RogueArray* THIS, RogueInt32 dest_i1, RogueArray* src_array, RogueInt32 src_i1, RogueInt32 copy_count )
{
  int element_size;
  int dest_i2, src_i2;

  if ( !src_array || dest_i1 >= THIS->count ) return THIS;
  if (THIS->is_reference_array ^ src_array->is_reference_array) return THIS;

  if (copy_count == -1) src_i2 = src_array->count - 1;
  else                  src_i2 = (src_i1 + copy_count) - 1;

  if (dest_i1 < 0)
  {
    src_i1 -= dest_i1;
    dest_i1 = 0;
  }

  if (src_i1 < 0) src_i1 = 0;
  if (src_i2 >= src_array->count) src_i2 = src_array->count - 1;
  if (src_i1 > src_i2) return THIS;

  copy_count = (src_i2 - src_i1) + 1;
  dest_i2 = dest_i1 + (copy_count - 1);
  if (dest_i2 >= THIS->count)
  {
    dest_i2 = (THIS->count - 1);
    copy_count = (dest_i2 - dest_i1) + 1;
  }
  if ( !copy_count ) return THIS;


#if defined(ROGUE_ARC)
  if (THIS != src_array || dest_i1 >= src_i1 + copy_count || (src_i1 + copy_count) <= dest_i1 || dest_i1 < src_i1)
  {
    // no overlap
    RogueObject** src  = src_array->as_objects + src_i1 - 1;
    RogueObject** dest = THIS->as_objects + dest_i1 - 1;
    while (--copy_count >= 0)
    {
      RogueObject* src_obj, dest_obj;
      if ((src_obj = *(++src))) ROGUE_INCREF(src_obj);
      if ((dest_obj = *(++dest)) && !(ROGUE_DECREF(dest_obj)))
      {
        // TODO: delete dest_obj
        *dest = src_obj;
      }
    }
  }
  else
  {
    // Copying earlier data to later data; copy in reverse order to
    // avoid accidental overwriting
    if (dest_i1 > src_i1)  // if they're equal then we don't need to copy anything!
    {
      RogueObject** src  = src_array->as_objects + src_i2 + 1;
      RogueObject** dest = THIS->as_objects + dest_i2 + 1;
      while (--copy_count >= 0)
      {
        RogueObject* src_obj, dest_obj;
        if ((src_obj = *(--src))) ROGUE_INCREF(src_obj);
        if ((dest_obj = *(--dest)) && !(ROGUE_DECREF(dest_obj)))
        {
          // TODO: delete dest_obj
          *dest = src_obj;
        }
      }
    }
  }
  return THIS;
#endif

  element_size = THIS->element_size;
  RogueByte* src = src_array->as_bytes + src_i1 * element_size;
  RogueByte* dest = THIS->as_bytes + (dest_i1 * element_size);
  int copy_bytes = copy_count * element_size;

  if (src == dest) return THIS;

  if (src >= dest + copy_bytes || (src + copy_bytes) <= dest)
  {
    // Copy region does not overlap
    memcpy( dest, src, copy_count * element_size );
  }
  else
  {
    // Copy region overlaps
    memmove( dest, src, copy_count * element_size );
  }

  return THIS;
}

//-----------------------------------------------------------------------------
//  RogueAllocationPage
//-----------------------------------------------------------------------------
RogueAllocationPage* RogueAllocationPage_create( RogueAllocationPage* next_page )
{
  RogueAllocationPage* result = (RogueAllocationPage*) ROGUE_NEW_BYTES(sizeof(RogueAllocationPage));
  result->next_page = next_page;
  result->cursor = result->data;
  result->remaining = ROGUEMM_PAGE_SIZE;
  return result;
}

#if 0 // This is currently done statically.  Code likely to be removed.
RogueAllocationPage* RogueAllocationPage_delete( RogueAllocationPage* THIS )
{
  if (THIS) ROGUE_DEL_BYTES( THIS );
  return 0;
};
#endif

void* RogueAllocationPage_allocate( RogueAllocationPage* THIS, int size )
{
  // Round size up to multiple of 8.
  if (size > 0) size = (size + 7) & ~7;
  else          size = 8;

  if (size > THIS->remaining) return 0;

  //ROGUE_LOG( "Allocating %d bytes from page.\n", size );
  void* result = THIS->cursor;
  THIS->cursor += size;
  THIS->remaining -= size;
  ((RogueObject*)result)->reference_count = 0;

  //ROGUE_LOG( "%d / %d\n", ROGUEMM_PAGE_SIZE - remaining, ROGUEMM_PAGE_SIZE );
  return result;
}


//-----------------------------------------------------------------------------
//  RogueAllocator
//-----------------------------------------------------------------------------
#if 0 // This is currently done statically.  Code likely to be removed.
RogueAllocator* RogueAllocator_create()
{
  RogueAllocator* result = (RogueAllocator*) ROGUE_NEW_BYTES( sizeof(RogueAllocator) );

  memset( result, 0, sizeof(RogueAllocator) );

  return result;
}

RogueAllocator* RogueAllocator_delete( RogueAllocator* THIS )
{
  while (THIS->pages)
  {
    RogueAllocationPage* next_page = THIS->pages->next_page;
    RogueAllocationPage_delete( THIS->pages );
    THIS->pages = next_page;
  }
  return 0;
}
#endif

void* RogueAllocator_allocate( RogueAllocator* THIS, int size )
{
#if ROGUE_GC_MODE_AUTO_MT
#if ROGUE_MTGC_DEBUG
    ROGUE_MUTEX_LOCK(Rogue_mtgc_w_mutex);
    if (Rogue_mtgc_w == 2)
    {
      ROGUE_LOG_ERROR("ALLOC DURING GC!\n");
      exit(1);
    }
    ROGUE_MUTEX_UNLOCK(Rogue_mtgc_w_mutex);
#endif
#endif

#if ROGUE_GC_MODE_AUTO_ANY
  Rogue_collect_garbage();
#endif
  if (size > ROGUEMM_SMALL_ALLOCATION_SIZE_LIMIT)
  {
    ROGUE_GC_COUNT_BYTES(size);
    void * mem = ROGUE_NEW_BYTES(size);
#if ROGUE_GC_MODE_AUTO_ANY
    if (!mem)
    {
      // Try hard!
      Rogue_collect_garbage(true);
      mem = ROGUE_NEW_BYTES(size);
    }
#endif
    return mem;
  }

  ROGUE_GC_SOA_LOCK;

  size = (size > 0) ? (size + ROGUEMM_GRANULARITY_MASK) & ~ROGUEMM_GRANULARITY_MASK : ROGUEMM_GRANULARITY_SIZE;

  ROGUE_GC_COUNT_BYTES(size);

  int slot;
  ROGUE_DEF_LOCAL_REF(RogueObject*, obj, THIS->available_objects[(slot=(size>>ROGUEMM_GRANULARITY_BITS))]);

  if (obj)
  {
    //ROGUE_LOG( "found free object\n");
    THIS->available_objects[slot] = obj->next_object;
    ROGUE_GC_SOA_UNLOCK;
    return obj;
  }

  // No free objects for requested size.

  // Try allocating a new object from the current page.
  if (THIS->pages )
  {
    obj = (RogueObject*) RogueAllocationPage_allocate( THIS->pages, size );
    if (obj)
    {
      ROGUE_GC_SOA_UNLOCK;
      return obj;
    }

    // Not enough room on allocation page.  Allocate any smaller blocks
    // we're able to and then move on to a new page.
    int s = slot - 1;
    while (s >= 1)
    {
      obj = (RogueObject*) RogueAllocationPage_allocate( THIS->pages, s << ROGUEMM_GRANULARITY_BITS );
      if (obj)
      {
        //ROGUE_LOG( "free obj size %d\n", (s << ROGUEMM_GRANULARITY_BITS) );
        obj->next_object = THIS->available_objects[s];
        THIS->available_objects[s] = obj;
      }
      else
      {
        --s;
      }
    }
  }

  // New page; this will work for sure.
  THIS->pages = RogueAllocationPage_create( THIS->pages );
  void * r = RogueAllocationPage_allocate( THIS->pages, size );
  ROGUE_GC_SOA_UNLOCK;
  return r;
}

#if ROGUE_GC_MODE_BOEHM
void Rogue_Boehm_Finalizer( void* obj, void* data )
{
  RogueObject* o = (RogueObject*)obj;
  o->type->on_cleanup_fn(o);
}

RogueObject* RogueAllocator_allocate_object( RogueAllocator* THIS, RogueType* of_type, int size, int element_type_index )
{
  // We use the "off page" allocations here, which require that somewhere there's a pointer
  // to something within the first 256 bytes.  Since someone should always be holding a
  // reference to the absolute start of the allocation (a reference!), this should always
  // be true.
#if ROGUE_GC_MODE_BOEHM_TYPED
  RogueObject * obj;
  if (of_type->gc_alloc_type == ROGUE_GC_ALLOC_TYPE_TYPED)
  {
    obj = (RogueObject*)GC_malloc_explicitly_typed_ignore_off_page(of_type->object_size, of_type->gc_type_descr);
  }
  else if (of_type->gc_alloc_type == ROGUE_GC_ALLOC_TYPE_ATOMIC)
  {
    obj = (RogueObject*)GC_malloc_atomic_ignore_off_page( of_type->object_size );
    if (obj) memset( obj, 0, of_type->object_size );
  }
  else
  {
    obj = (RogueObject*)GC_malloc_ignore_off_page( of_type->object_size );
  }
  if (!obj)
  {
    Rogue_collect_garbage( true );
    obj = (RogueObject*)GC_MALLOC( of_type->object_size );
  }
  obj->object_size = of_type->object_size;
#else
  RogueObject * obj = (RogueObject*)GC_malloc_ignore_off_page( size );
  if (!obj)
  {
    Rogue_collect_garbage( true );
    obj = (RogueObject*)GC_MALLOC( size );
  }
  obj->object_size = size;
#endif

  ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( "Allocating " ) );
  ROGUE_GCDEBUG_STATEMENT( RogueType_print_name(of_type) );
  ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( " %p\n", (RogueObject*)obj ) );
  //ROGUE_GCDEBUG_STATEMENT( Rogue_print_stack_trace() );

#if ROGUE_GC_MODE_BOEHM_TYPED
  // In typed mode, we allocate the array object and the actual data independently so that
  // they can have different GC types.
  if (element_type_index != -1)
  {
    RogueType* el_type = &Rogue_types[element_type_index];
    int data_size = size - of_type->object_size;
    int elements = data_size / el_type->object_size;
    void * data;
    if (el_type->gc_alloc_type == ROGUE_GC_ALLOC_TYPE_TYPED)
    {
      data = GC_calloc_explicitly_typed(elements, el_type->object_size, el_type->gc_type_descr);
    }
    else if (of_type->gc_alloc_type == ROGUE_GC_ALLOC_TYPE_ATOMIC)
    {
      data = GC_malloc_atomic_ignore_off_page( data_size );
      if (data) memset( obj, 0, data_size );
    }
    else
    {
      data = GC_malloc_ignore_off_page( data_size );
    }
    ((RogueArray*)obj)->as_bytes = (RogueByte*)data;
    ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( "Allocating " ) );
    ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( "  Elements " ) );
    ROGUE_GCDEBUG_STATEMENT( RogueType_print_name(el_type) );
    ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( " %p\n", (RogueObject*)data ) );
  }
#endif

  if (of_type->on_cleanup_fn)
  {
    GC_REGISTER_FINALIZER_IGNORE_SELF( obj, Rogue_Boehm_Finalizer, 0, 0, 0 );
  }

  obj->type = of_type;

  return obj;
}
#else
RogueObject* RogueAllocator_allocate_object( RogueAllocator* THIS, RogueType* of_type, int size, int element_type_index )
{
  void * mem = RogueAllocator_allocate( THIS, size );
  memset( mem, 0, size );

  ROGUE_DEF_LOCAL_REF(RogueObject*, obj, (RogueObject*)mem);

  ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( "Allocating " ) );
  ROGUE_GCDEBUG_STATEMENT( RogueType_print_name(of_type) );
  ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( " %p\n", (RogueObject*)obj ) );
  //ROGUE_GCDEBUG_STATEMENT( Rogue_print_stack_trace() );

  obj->type = of_type;
  obj->object_size = size;

  ROGUE_MTGC_BARRIER; // Probably not necessary

  if (of_type->on_cleanup_fn)
  {
    ROGUE_LINKED_LIST_INSERT(THIS->objects_requiring_cleanup, obj, obj->next_object);
  }
  else
  {
    ROGUE_LINKED_LIST_INSERT(THIS->objects, obj, obj->next_object);
  }

  return obj;
}
#endif

void* RogueAllocator_free( RogueAllocator* THIS, void* data, int size )
{
  if (data)
  {
    ROGUE_GCDEBUG_STATEMENT(memset(data,0,size));
    if (size > ROGUEMM_SMALL_ALLOCATION_SIZE_LIMIT)
    {
      // When debugging GC, it can be very useful to log the object types of
      // freed objects.  When valgrind points out access to freed memory, you
      // can then see what it was.
      #if 0
      RogueObject* obj = (RogueObject*) data;
      ROGUE_LOG("DEL %i %p ", (int)pthread_self(), data);
      RogueType_print_name( obj-> type );
      ROGUE_LOG("\n");
      #endif
      ROGUE_DEL_BYTES( data );
    }
    else
    {
      // Return object to small allocation pool
      RogueObject* obj = (RogueObject*) data;
      int slot = (size + ROGUEMM_GRANULARITY_MASK) >> ROGUEMM_GRANULARITY_BITS;
      if (slot <= 0) slot = 1;
      obj->next_object = THIS->available_objects[slot];
      THIS->available_objects[slot] = obj;
    }
  }

  // Always returns null, allowing a pointer to be freed and assigned null in
  // a single step.
  return 0;
}


void RogueAllocator_free_objects( RogueAllocator* THIS )
{
  RogueObject* objects = THIS->objects;
  while (objects)
  {
    RogueObject* next_object = objects->next_object;
    RogueAllocator_free( THIS, objects, objects->object_size );
    objects = next_object;
  }

  THIS->objects = 0;
}

void RogueAllocator_free_all( )
{
  for (int i=0; i<Rogue_allocator_count; ++i)
  {
    RogueAllocator_free_objects( &Rogue_allocators[i] );
  }
}

void RogueAllocator_collect_garbage( RogueAllocator* THIS )
{
  // Global program objects have already been traced through.

  // Trace through all as-yet unreferenced objects that are manually retained.
  RogueObject* cur = THIS->objects;
  while (cur)
  {
    if (cur->object_size >= 0 && cur->reference_count > 0)
    {
      cur->type->trace_fn( cur );
    }
    cur = cur->next_object;
  }

  cur = THIS->objects_requiring_cleanup;
  while (cur)
  {
    if (cur->object_size >= 0 && cur->reference_count > 0)
    {
      cur->type->trace_fn( cur );
    }
    cur = cur->next_object;
  }

  // For any unreferenced objects requiring clean-up, we'll:
  //   1.  Reference them and move them to a separate short-term list.
  //   2.  Finish the regular GC.
  //   3.  Call on_cleanup() on each of them, which may create new
  //       objects (which is why we have to wait until after the GC).
  //   4.  Move them to the list of regular objects.
  cur = THIS->objects_requiring_cleanup;
  RogueObject* unreferenced_on_cleanup_objects = 0;
  RogueObject* survivors = 0;  // local var for speed
  while (cur)
  {
    RogueObject* next_object = cur->next_object;
    if (cur->object_size < 0)
    {
      // Referenced.
      cur->object_size = ~cur->object_size;
      cur->next_object = survivors;
      survivors = cur;
    }
    else
    {
      // Unreferenced - go ahead and trace it since we'll call on_cleanup
      // on it.
      cur->type->trace_fn( cur );
      cur->next_object = unreferenced_on_cleanup_objects;
      unreferenced_on_cleanup_objects = cur;
    }
    cur = next_object;
  }
  THIS->objects_requiring_cleanup = survivors;

  // All objects are in a state where a non-negative size means that the object is
  // due to be deleted.
  Rogue_on_gc_trace_finished.call();

  // Reset or delete each general object
  cur = THIS->objects;
  THIS->objects = 0;
  survivors = 0;  // local var for speed

  while (cur)
  {
    RogueObject* next_object = cur->next_object;
    if (cur->object_size < 0)
    {
      cur->object_size = ~cur->object_size;
      cur->next_object = survivors;
      survivors = cur;
    }
    else
    {
      ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( "Freeing " ) );
      ROGUE_GCDEBUG_STATEMENT( RogueType_print_name(cur->type) );
      ROGUE_GCDEBUG_STATEMENT( ROGUE_LOG( " %p\n", cur ) );
      RogueAllocator_free( THIS, cur, cur->object_size );
    }
    cur = next_object;
  }

  THIS->objects = survivors;


  // Call on_cleanup() on unreferenced objects requiring cleanup
  // and move them to the general objects list so they'll be deleted
  // the next time they're unreferenced.  Calling on_cleanup() may
  // create additional objects so THIS->objects may change during a
  // on_cleanup() call.
  cur = unreferenced_on_cleanup_objects;
  while (cur)
  {
    RogueObject* next_object = cur->next_object;

    cur->type->on_cleanup_fn( cur );

    cur->object_size = ~cur->object_size;
    cur->next_object = THIS->objects;
    THIS->objects = cur;

    cur = next_object;
  }

  if (Rogue_gc_logging)
  {
    int byte_count = 0;
    int object_count = 0;

    for (int i=0; i<Rogue_allocator_count; ++i)
    {
      RogueAllocator* allocator = &Rogue_allocators[i];

      RogueObject* cur = allocator->objects;
      while (cur)
      {
        ++object_count;
        byte_count += cur->object_size;
        cur = cur->next_object;
      }

      cur = allocator->objects_requiring_cleanup;
      while (cur)
      {
        ++object_count;
        byte_count += cur->object_size;
        cur = cur->next_object;
      }
    }

    ROGUE_LOG( "Post-GC: %d objects, %d bytes used.\n", object_count, byte_count );
  }
}

void Rogue_print_stack_trace ( bool leading_newline )
{
  RogueDebugTrace* current = Rogue_call_stack;
  if (current && leading_newline) ROGUE_LOG( "\n" );
  while (current)
  {
    ROGUE_LOG( "%s\n", current->to_c_string() );
    current = current->previous_trace;
  }
  ROGUE_LOG("\n");
}

#if defined(ROGUE_PLATFORM_WINDOWS)
void Rogue_segfault_handler( int signal )
{
  ROGUE_LOG_ERROR( "Access violation\n" );
#else
void Rogue_segfault_handler( int signal, siginfo_t *si, void *arg )
{
  if (si->si_addr < (void*)4096)
  {
    // Probably a null pointer dereference.
    ROGUE_LOG_ERROR( "Null reference error (accessing memory at %p)\n",
            si->si_addr );
  }
  else
  {
    if (si->si_code == SEGV_MAPERR)
      ROGUE_LOG_ERROR( "Access to unmapped memory at " );
    else if (si->si_code == SEGV_ACCERR)
      ROGUE_LOG_ERROR( "Access to forbidden memory at " );
    else
      ROGUE_LOG_ERROR( "Unknown segfault accessing " );
    ROGUE_LOG_ERROR("%p\n", si->si_addr);
  }
#endif

  Rogue_print_stack_trace( true );

  exit(1);
}

void Rogue_update_weak_references_during_gc()
{
  RogueWeakReference* cur = Rogue_weak_references;
  while (cur)
  {
    if (cur->value && cur->value->object_size >= 0)
    {
      // The value held by this weak reference is about to be deleted by the
      // GC system; null out the value.
      cur->value = 0;
    }
    cur = cur->next_weak_reference;
  }
}


void Rogue_configure_types()
{
#if ROGUE_THREAD_MODE == ROGUE_THREAD_MODE_PTHREADS
_rogue_init_mutex(&Rogue_thread_singleton_lock);
#endif

  int i;
  const int* next_type_info = Rogue_type_info_table;

#if defined(ROGUE_PLATFORM_WINDOWS)
  // Use plain old signal() instead of sigaction()
  signal( SIGSEGV, Rogue_segfault_handler );
#else
  // Install seg fault handler
  struct sigaction sa;

  memset( &sa, 0, sizeof(sa) );
  sigemptyset( &sa.sa_mask );
  sa.sa_sigaction = Rogue_segfault_handler;
  sa.sa_flags     = SA_SIGINFO;

  sigaction( SIGSEGV, &sa, NULL );
#endif

  // Initialize allocators
  memset( Rogue_allocators, 0, sizeof(RogueAllocator)*Rogue_allocator_count );

#ifdef ROGUE_INTROSPECTION
  int global_property_pointer_cursor = 0;
  int property_offset_cursor = 0;
#endif

  // Initialize types
  for (i=0; i<Rogue_type_count; ++i)
  {
    int j;
    RogueType* type = &Rogue_types[i];
    const int* type_info = next_type_info;
    next_type_info += *(type_info++) + 1;

    memset( type, 0, sizeof(RogueType) );

    type->index = i;
    type->name_index = Rogue_type_name_index_table[i];
    type->object_size = Rogue_object_size_table[i];
#ifdef ROGUE_INTROSPECTION
    type->attributes = Rogue_attributes_table[i];
#endif
    type->allocator = &Rogue_allocators[ *(type_info++) ];
    type->methods = Rogue_dynamic_method_table + *(type_info++);
    type->base_type_count = *(type_info++);
    if (type->base_type_count)
    {
#if ROGUE_GC_MODE_BOEHM
      type->base_types = new (NoGC) RogueType*[ type->base_type_count ];
#else
      type->base_types = new RogueType*[ type->base_type_count ];
#endif
      for (j=0; j<type->base_type_count; ++j)
      {
        type->base_types[j] = &Rogue_types[ *(type_info++) ];
      }
    }

    type->global_property_count = *(type_info++);
    type->global_property_name_indices = type_info;
    type_info += type->global_property_count;
    type->global_property_type_indices = type_info;
    type_info += type->global_property_count;

    type->property_count = *(type_info++);
    type->property_name_indices = type_info;
    type_info += type->property_count;
    type->property_type_indices = type_info;
    type_info += type->property_count;

#if ROGUE_GC_MODE_BOEHM_TYPED
    type->gc_alloc_type = *(type_info++);
#endif

#ifdef ROGUE_INTROSPECTION
    if (((type->attributes & ROGUE_ATTRIBUTE_TYPE_MASK) == ROGUE_ATTRIBUTE_IS_CLASS)
      || ((type->attributes & ROGUE_ATTRIBUTE_TYPE_MASK) == ROGUE_ATTRIBUTE_IS_COMPOUND))
    {
      type->global_property_pointers = Rogue_global_property_pointers + global_property_pointer_cursor;
      global_property_pointer_cursor += type->global_property_count;
      type->property_offsets = Rogue_property_offsets + property_offset_cursor;
      property_offset_cursor += type->property_count;
    }
#endif
    type->method_count = *(type_info++);
    type->global_method_count = *(type_info++);

    type->trace_fn = Rogue_trace_fn_table[i];
    type->init_object_fn = Rogue_init_object_fn_table[i];
    type->init_fn        = Rogue_init_fn_table[i];
    type->on_cleanup_fn  = Rogue_on_cleanup_fn_table[i];
    type->to_string_fn   = Rogue_to_string_fn_table[i];

    ROGUE_DEBUG_STATEMENT(assert(type_info <= next_type_info));
  }

  Rogue_on_gc_trace_finished.add( Rogue_update_weak_references_during_gc );

#if ROGUE_GC_MODE_BOEHM_TYPED
  Rogue_init_boehm_type_info();
#endif
}

#if ROGUE_GC_MODE_BOEHM
static GC_ToggleRefStatus Rogue_Boehm_ToggleRefStatus( void * o )
{
  RogueObject* obj = (RogueObject*)o;
  if (obj->reference_count > 0) return GC_TOGGLE_REF_STRONG;
  return GC_TOGGLE_REF_DROP;
}

static void Rogue_Boehm_on_collection_event( GC_EventType event )
{
  if (event == GC_EVENT_START)
  {
    Rogue_on_gc_begin.call();
  }
  else if (event == GC_EVENT_END)
  {
    Rogue_on_gc_end.call();
  }
}

void Rogue_configure_gc()
{
  // Initialize Boehm collector
  //GC_set_finalize_on_demand(0);
  GC_set_toggleref_func(Rogue_Boehm_ToggleRefStatus);
  GC_set_on_collection_event(Rogue_Boehm_on_collection_event);
  //GC_set_all_interior_pointers(0);
  GC_INIT();
}
#elif ROGUE_GC_MODE_AUTO_MT
// Rogue_configure_gc already defined above.
#else
void Rogue_configure_gc()
{
}
#endif

#if ROGUE_GC_MODE_BOEHM
bool Rogue_collect_garbage( bool forced )
{
  if (forced)
  {
    GC_gcollect();
    return true;
  }

  return GC_collect_a_little();
}
#else // Auto or manual

static inline void Rogue_collect_garbage_real(void);

bool Rogue_collect_garbage( bool forced )
{
  if (!forced && !Rogue_gc_requested & !ROGUE_GC_AT_THRESHOLD) return false;

#if ROGUE_GC_MODE_AUTO_MT
  Rogue_mtgc_run_gc_and_wait();
#else
  Rogue_collect_garbage_real();
#endif

  return true;
}

static inline void Rogue_collect_garbage_real()
{
  Rogue_gc_requested = false;
  if (Rogue_gc_active) return;
  Rogue_gc_active = true;
  ++ Rogue_gc_count;

//ROGUE_LOG( "GC %d\n", Rogue_allocation_bytes_until_gc );
  ROGUE_GC_RESET_COUNT;

  Rogue_on_gc_begin.call();

  Rogue_trace();

  for (int i=0; i<Rogue_allocator_count; ++i)
  {
    RogueAllocator_collect_garbage( &Rogue_allocators[i] );
  }

  Rogue_on_gc_end.call();
  Rogue_gc_active = false;
}

#endif

void Rogue_quit()
{
  int i;

  if ( !Rogue_configured ) return;
  Rogue_configured = 0;

  RogueGlobal__call_exit_functions( (RogueClassGlobal*) ROGUE_SINGLETON(Global) );

  ROGUE_THREADS_WAIT_FOR_ALL;

#if ROGUE_GC_MODE_AUTO_MT
  Rogue_mtgc_quit_gc_thread();
#else
  // Give a few GC's to allow objects requiring clean-up to do so.
  Rogue_collect_garbage( true );
  Rogue_collect_garbage( true );
  Rogue_collect_garbage( true );

  RogueAllocator_free_all();
#endif

  for (i=0; i<Rogue_type_count; ++i)
  {
    RogueType_retire( &Rogue_types[i] );
  }

  Rogue_thread_unregister();

  Rogue_gc_logging = false;
  Rogue_gc_count = 0;
  Rogue_gc_requested = 0;
  Rogue_gc_active = 0;
  Rogue_call_stack = 0;
  Rogue_weak_references = 0;
}

#if ROGUE_GC_MODE_BOEHM

void Rogue_Boehm_IncRef (RogueObject* o)
{
  ++o->reference_count;
  if (o->reference_count == 1)
  {
    GC_toggleref_add(o, 1);
  }
}
void Rogue_Boehm_DecRef (RogueObject* o)
{
  --o->reference_count;
  if (o->reference_count < 0)
  {
    o->reference_count = 0;
  }
}
#endif


//-----------------------------------------------------------------------------
//  Exception handling
//-----------------------------------------------------------------------------
void Rogue_terminate_handler()
{
  ROGUE_LOG_ERROR( "Uncaught exception.\n" );
  exit(1);
}
//=============================================================================
typedef void*(*ROGUEM0)(void*);
typedef void(*ROGUEM1)(void*);
typedef void*(*ROGUEM2)(void*,void*);

// Object.to_String()
extern void* Rogue_call_ROGUEM0( int i, void* THIS )
{
  return ((ROGUEM0)(((RogueObject*)THIS)->type->methods[i]))( THIS );
}

// (Function()).call()
extern void Rogue_call_ROGUEM1( int i, void* THIS )
{
  ((ROGUEM1)(((RogueObject*)THIS)->type->methods[i]))( THIS );
}


// GLOBAL PROPERTIES
RogueString_List* RogueSystem_command_line_arguments = 0;
RogueString* RogueSystem_executable_filepath = 0;
RogueClassSystemEnvironment RogueSystem_environment = RogueClassSystemEnvironment();
RogueString_List* RogueStateFlags_names = 0;
RogueInt32_List* RogueStateFlags_values = 0;
RogueString_List* RogueTransitionActions_names = 0;
RogueInt32_List* RogueTransitionActions_values = 0;
RogueString_List* RogueTokenType_names = 0;
RogueInt32_List* RogueTokenType_values = 0;
ROGUE_THREAD_LOCAL RogueByte_List* RogueStringBuilder_work_bytes = 0;
ROGUE_THREAD_LOCAL RogueClassStringBuilderPool* RogueStringBuilder_pool = 0;

void RogueGlobal_trace( void* obj );
void RogueStringBuilder_trace( void* obj );
void RogueByte_List_trace( void* obj );
void RogueStringBuilderPool_trace( void* obj );
void RogueStringBuilder_List_trace( void* obj );
void Rogue_Function____List_trace( void* obj );
void RogueStackTrace_trace( void* obj );
void RogueString_List_trace( void* obj );
void RogueCharacter_List_trace( void* obj );
void RogueConsole_trace( void* obj );
void RogueConsoleErrorPrinter_trace( void* obj );
void RogueBossScanTableBuilder_trace( void* obj );
void RogueScanTableBuilder_trace( void* obj );
void RogueScanState_trace( void* obj );
void RogueInt32_List_trace( void* obj );
void RogueTransition_List_trace( void* obj );
void RogueTransition_trace( void* obj );
void RogueWeakReference_trace( void* obj );
void RogueFunction_1670_trace( void* obj );
void RogueException_trace( void* obj );
void RogueError_trace( void* obj );
void RogueOutOfBoundsError_trace( void* obj );

void RogueGlobal_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassGlobal*)obj)->console)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueClassGlobal*)obj)->global_output_buffer)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueClassGlobal*)obj)->exit_functions)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueStringBuilder_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueStringBuilder*)obj)->utf8)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueByte_List_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueByte_List*)obj)->data)) RogueArray_trace( link );
}

void RogueStringBuilderPool_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassStringBuilderPool*)obj)->available)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueStringBuilder_List_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueStringBuilder_List*)obj)->data)) RogueArray_trace( link );
}

void Rogue_Function____List_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((Rogue_Function____List*)obj)->data)) RogueArray_trace( link );
}

void RogueStackTrace_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassStackTrace*)obj)->entries)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueString_List_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueString_List*)obj)->data)) RogueArray_trace( link );
}

void RogueCharacter_List_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueCharacter_List*)obj)->data)) RogueArray_trace( link );
}

void RogueConsole_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassConsole*)obj)->error)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueClassConsole*)obj)->output_buffer)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueClassConsole*)obj)->input_buffer)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueClassConsole*)obj)->input_bytes)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueConsoleErrorPrinter_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassConsoleErrorPrinter*)obj)->output_buffer)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueBossScanTableBuilder_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassBossScanTableBuilder*)obj)->start)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueScanTableBuilder_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassScanTableBuilder*)obj)->start)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueScanState_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassScanState*)obj)->transitions)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueInt32_List_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueInt32_List*)obj)->data)) RogueArray_trace( link );
}

void RogueTransition_List_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueTransition_List*)obj)->data)) RogueArray_trace( link );
}

void RogueTransition_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassTransition*)obj)->target_state)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueWeakReference_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueWeakReference*)obj)->next_weak_reference)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueFunction_1670_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassFunction_1670*)obj)->console)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueException_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueException*)obj)->message)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueException*)obj)->stack_trace)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueError_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassError*)obj)->message)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueClassError*)obj)->stack_trace)) ((RogueObject*)link)->type->trace_fn( link );
}

void RogueOutOfBoundsError_trace( void* obj )
{
  void* link;
  if ( !obj || ((RogueObject*)obj)->object_size < 0 ) return;
  ((RogueObject*)obj)->object_size = ~((RogueObject*)obj)->object_size;
  if ((link=((RogueClassOutOfBoundsError*)obj)->message)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=((RogueClassOutOfBoundsError*)obj)->stack_trace)) ((RogueObject*)link)->type->trace_fn( link );
}


const int Rogue_type_name_index_table[] =
{
  27,63,64,29,44,30,65,52,31,66,35,47,54,67,48,36,
  53,28,32,46,51,68,69,70,45,55,71,33,72,41,34,58,
  61,37,42,49,56,50,43,57,38,39,40,59,7,2,60,62,
  73,74,75,76,77
};
RogueInitFn Rogue_init_object_fn_table[] =
{
  (RogueInitFn) RogueGlobal__init_object,
  0,
  0,
  (RogueInitFn) RogueStringBuilder__init_object,
  (RogueInitFn) RogueByte_List__init_object,
  (RogueInitFn) RogueGenericList__init_object,
  0,
  0,
  0,
  0,
  (RogueInitFn) RogueStringBuilderPool__init_object,
  (RogueInitFn) RogueStringBuilder_List__init_object,
  0,
  0,
  (RogueInitFn) Rogue_Function____List__init_object,
  (RogueInitFn) Rogue_Function_____init_object,
  0,
  0,
  (RogueInitFn) RogueStackTrace__init_object,
  (RogueInitFn) RogueString_List__init_object,
  0,
  0,
  0,
  0,
  (RogueInitFn) RogueCharacter_List__init_object,
  0,
  0,
  (RogueInitFn) RogueConsole__init_object,
  0,
  (RogueInitFn) RogueConsoleErrorPrinter__init_object,
  (RogueInitFn) RogueMath__init_object,
  (RogueInitFn) RogueFunction_540__init_object,
  (RogueInitFn) RogueBossScanTableBuilder__init_object,
  (RogueInitFn) RogueScanTableBuilder__init_object,
  (RogueInitFn) RogueScanState__init_object,
  (RogueInitFn) RogueInt32_List__init_object,
  0,
  (RogueInitFn) RogueTransition_List__init_object,
  (RogueInitFn) RogueTransition__init_object,
  0,
  (RogueInitFn) RogueRuntime__init_object,
  (RogueInitFn) RogueSystem__init_object,
  (RogueInitFn) RogueWeakReference__init_object,
  (RogueInitFn) RogueFunction_1670__init_object,
  0,
  (RogueInitFn) RogueException__init_object,
  (RogueInitFn) RogueError__init_object,
  (RogueInitFn) RogueOutOfBoundsError__init_object,
  0,
  0,
  0,
  0,
  0
};

RogueInitFn Rogue_init_fn_table[] =
{
  (RogueInitFn) RogueGlobal__init,
  0,
  0,
  (RogueInitFn) RogueStringBuilder__init,
  (RogueInitFn) RogueByte_List__init,
  (RogueInitFn) RogueObject__init,
  0,
  0,
  0,
  0,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueStringBuilder_List__init,
  0,
  0,
  (RogueInitFn) Rogue_Function____List__init,
  (RogueInitFn) RogueObject__init,
  0,
  0,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueString_List__init,
  0,
  0,
  0,
  0,
  (RogueInitFn) RogueCharacter_List__init,
  0,
  0,
  (RogueInitFn) RogueConsole__init,
  0,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueBossScanTableBuilder__init,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueInt32_List__init,
  0,
  (RogueInitFn) RogueTransition_List__init,
  (RogueInitFn) RogueTransition__init,
  0,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueObject__init,
  (RogueInitFn) RogueObject__init,
  0,
  (RogueInitFn) RogueException__init,
  (RogueInitFn) RogueException__init,
  (RogueInitFn) RogueException__init,
  0,
  0,
  0,
  0,
  0
};

RogueCleanUpFn Rogue_on_cleanup_fn_table[] =
{
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  (RogueCleanUpFn) RogueWeakReference__on_cleanup,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

RogueToStringFn Rogue_to_string_fn_table[] =
{
  (RogueToStringFn) RogueObject__to_String,
  0,
  0,
  (RogueToStringFn) RogueStringBuilder__to_String,
  (RogueToStringFn) RogueByte_List__to_String,
  (RogueToStringFn) RogueObject__to_String,
  0,
  0,
  0,
  0,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueStringBuilder_List__to_String,
  0,
  0,
  (RogueToStringFn) Rogue_Function____List__to_String,
  (RogueToStringFn) RogueObject__to_String,
  0,
  0,
  (RogueToStringFn) RogueStackTrace__to_String,
  (RogueToStringFn) RogueString_List__to_String,
  0,
  0,
  0,
  0,
  (RogueToStringFn) RogueCharacter_List__to_String,
  0,
  0,
  (RogueToStringFn) RogueConsole__to_String,
  0,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueInt32_List__to_String,
  0,
  (RogueToStringFn) RogueTransition_List__to_String,
  (RogueToStringFn) RogueObject__to_String,
  0,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  (RogueToStringFn) RogueObject__to_String,
  0,
  (RogueToStringFn) RogueException__to_String,
  (RogueToStringFn) RogueException__to_String,
  (RogueToStringFn) RogueException__to_String,
  0,
  0,
  0,
  0,
  0
};

RogueTraceFn Rogue_trace_fn_table[] =
{
  RogueGlobal_trace,
  0,
  0,
  RogueStringBuilder_trace,
  RogueByte_List_trace,
  RogueObject_trace,
  0,
  RogueObject_trace,
  RogueObject_trace,
  0,
  RogueStringBuilderPool_trace,
  RogueStringBuilder_List_trace,
  RogueArray_trace,
  0,
  Rogue_Function____List_trace,
  RogueObject_trace,
  RogueObject_trace,
  RogueObject_trace,
  RogueStackTrace_trace,
  RogueString_List_trace,
  RogueObject_trace,
  0,
  0,
  0,
  RogueCharacter_List_trace,
  RogueObject_trace,
  0,
  RogueConsole_trace,
  0,
  RogueConsoleErrorPrinter_trace,
  RogueObject_trace,
  RogueObject_trace,
  RogueBossScanTableBuilder_trace,
  RogueScanTableBuilder_trace,
  RogueScanState_trace,
  RogueInt32_List_trace,
  RogueObject_trace,
  RogueTransition_List_trace,
  RogueTransition_trace,
  RogueArray_trace,
  RogueObject_trace,
  RogueObject_trace,
  RogueWeakReference_trace,
  RogueFunction_1670_trace,
  RogueObject_trace,
  RogueException_trace,
  RogueError_trace,
  RogueOutOfBoundsError_trace,
  0,
  0,
  0,
  0,
  0
};

void Rogue_trace()
{
  void* link;
  int i;

  // Trace GLOBAL PROPERTIES
  if ((link=RogueStringBuilder_work_bytes)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueStringBuilder_pool)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueSystem_command_line_arguments)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueSystem_executable_filepath)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueStateFlags_names)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueStateFlags_values)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueTransitionActions_names)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueTransitionActions_values)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueTokenType_names)) ((RogueObject*)link)->type->trace_fn( link );
  if ((link=RogueTokenType_values)) ((RogueObject*)link)->type->trace_fn( link );

  // Trace Class objects and singletons
  for (i=Rogue_type_count; --i>=0; )
  {
    RogueType* type = &Rogue_types[i];
    {
      auto singleton = ROGUE_GET_SINGLETON(type);
      if (singleton) type->trace_fn( singleton );
    }
  }
}

const void* Rogue_dynamic_method_table[] =
{
  (void*) (ROGUEM0) RogueGlobal__init_object, // Global
  (void*) (ROGUEM0) RogueGlobal__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueGlobal__type_name,
  0, // PrintWriter<<global_output_buffer>>.close() // PrintWriter<<global_output_buffer>>
  0, // PrintWriter.close() // PrintWriter
  (void*) (ROGUEM0) RogueStringBuilder__init_object, // StringBuilder
  (void*) (ROGUEM0) RogueStringBuilder__init,
  0, // StringBuilder.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueStringBuilder__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueStringBuilder__type_name,
  (void*) (ROGUEM0) RogueByte_List__init_object, // Byte[]
  (void*) (ROGUEM0) RogueByte_List__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueByte_List__to_String,
  0, // Byte[].to_Value()
  0, // Byte[].to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Byte[].unpack(Value)
  (void*) (ROGUEM0) RogueByte_List__type_name,
  (void*) (ROGUEM0) RogueGenericList__init_object, // GenericList
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueGenericList__type_name,
  0, // Byte.to_String() // Byte
  (void*) (ROGUEM1) RogueObject__init_object, // Array<<Byte>>
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray_Byte___type_name,
  (void*) (ROGUEM1) RogueObject__init_object, // Array
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray__type_name,
  0, // Int32.or_smaller(Int32) // Int32
  (void*) (ROGUEM0) RogueStringBuilderPool__init_object, // StringBuilderPool
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueStringBuilderPool__type_name,
  (void*) (ROGUEM0) RogueStringBuilder_List__init_object, // StringBuilder[]
  (void*) (ROGUEM0) RogueStringBuilder_List__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueStringBuilder_List__to_String,
  0, // StringBuilder[].to_Value()
  0, // StringBuilder[].to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // StringBuilder[].unpack(Value)
  (void*) (ROGUEM0) RogueStringBuilder_List__type_name,
  (void*) (ROGUEM1) RogueObject__init_object, // Array<<StringBuilder>>
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray_StringBuilder___type_name,
  (void*) (ROGUEM0) Rogue_Function____List__init_object, // (Function())[]
  (void*) (ROGUEM0) Rogue_Function____List__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) Rogue_Function____List__to_String,
  0, // (Function())[].to_Value()
  0, // (Function())[].to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // (Function())[].unpack(Value)
  (void*) (ROGUEM0) Rogue_Function____List__type_name,
  (void*) (ROGUEM0) Rogue_Function_____init_object, // (Function())
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) Rogue_Function_____type_name,
  (void*) (ROGUEM1) Rogue_Function_____call,
  (void*) (ROGUEM1) RogueObject__init_object, // Array<<(Function())>>
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray__Function______type_name,
  (void*) (ROGUEM1) RogueObject__init_object, // String
  (void*) (ROGUEM0) RogueObject__init,
  0, // String.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueString__to_String,
  0, // String.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueString__type_name,
  (void*) (ROGUEM0) RogueStackTrace__init_object, // StackTrace
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueStackTrace__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueStackTrace__type_name,
  (void*) (ROGUEM0) RogueString_List__init_object, // String[]
  (void*) (ROGUEM0) RogueString_List__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueString_List__to_String,
  0, // String[].to_Value()
  0, // String[].to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // String[].unpack(Value)
  (void*) (ROGUEM0) RogueString_List__type_name,
  (void*) (ROGUEM1) RogueObject__init_object, // Array<<String>>
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray_String___type_name,
  0, // Int64.print_in_power2_base(Int32,Int32,StringBuilder) // Int64
  0, // Character.is_letter() // Character
  (void*) (ROGUEM0) RogueCharacter_List__init_object, // Character[]
  (void*) (ROGUEM0) RogueCharacter_List__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueCharacter_List__to_String,
  0, // Character[].to_Value()
  0, // Character[].to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Character[].unpack(Value)
  (void*) (ROGUEM0) RogueCharacter_List__type_name,
  (void*) (ROGUEM1) RogueObject__init_object, // Array<<Character>>
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray_Character___type_name,
  0, // Reader<<Character>>.close() // Reader<<Character>>
  (void*) (ROGUEM0) RogueConsole__init_object, // Console
  (void*) (ROGUEM0) RogueConsole__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueConsole__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueConsole__type_name,
  0, // PrintWriter<<output_buffer>>.close() // PrintWriter<<output_buffer>>
  (void*) (ROGUEM0) RogueConsoleErrorPrinter__init_object, // ConsoleErrorPrinter
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueConsoleErrorPrinter__type_name,
  (void*) (ROGUEM0) RogueMath__init_object, // Math
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueMath__type_name,
  (void*) (ROGUEM0) RogueFunction_540__init_object, // Function_540
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueFunction_540__type_name,
  (void*) (ROGUEM1) RogueFunction_540__call,
  (void*) (ROGUEM0) RogueBossScanTableBuilder__init_object, // BossScanTableBuilder
  (void*) (ROGUEM0) RogueBossScanTableBuilder__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueBossScanTableBuilder__type_name,
  (void*) (ROGUEM0) RogueScanTableBuilder__init_object, // ScanTableBuilder
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueScanTableBuilder__type_name,
  (void*) (ROGUEM0) RogueScanState__init_object, // ScanState
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueScanState__type_name,
  (void*) (ROGUEM0) RogueInt32_List__init_object, // Int32[]
  (void*) (ROGUEM0) RogueInt32_List__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueInt32_List__to_String,
  0, // Int32[].to_Value()
  0, // Int32[].to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Int32[].unpack(Value)
  (void*) (ROGUEM0) RogueInt32_List__type_name,
  (void*) (ROGUEM1) RogueObject__init_object, // Array<<Int32>>
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray_Int32___type_name,
  (void*) (ROGUEM0) RogueTransition_List__init_object, // Transition[]
  (void*) (ROGUEM0) RogueTransition_List__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueTransition_List__to_String,
  0, // Transition[].to_Value()
  0, // Transition[].to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Transition[].unpack(Value)
  (void*) (ROGUEM0) RogueTransition_List__type_name,
  (void*) (ROGUEM0) RogueTransition__init_object, // Transition
  (void*) (ROGUEM0) RogueTransition__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueTransition__type_name,
  (void*) (ROGUEM1) RogueObject__init_object, // Array<<Transition>>
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueArray_Transition___type_name,
  (void*) (ROGUEM0) RogueRuntime__init_object, // Runtime
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueRuntime__type_name,
  (void*) (ROGUEM0) RogueSystem__init_object, // System
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueSystem__type_name,
  (void*) (ROGUEM0) RogueWeakReference__init_object, // WeakReference
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueWeakReference__type_name,
  (void*) (ROGUEM0) RogueFunction_1670__init_object, // Function_1670
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueFunction_1670__type_name,
  (void*) (ROGUEM1) RogueFunction_1670__call,
  (void*) (ROGUEM1) RogueObject__init_object, // Object
  (void*) (ROGUEM0) RogueObject__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueObject__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueObject__type_name,
  (void*) (ROGUEM0) RogueException__init_object, // Exception
  (void*) (ROGUEM0) RogueException__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueException__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueException__type_name,
  (void*) (ROGUEM2) RogueException__init__String,
  (void*) (ROGUEM0) RogueError__init_object, // Error
  (void*) (ROGUEM0) RogueException__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueException__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueError__type_name,
  (void*) (ROGUEM2) RogueException__init__String,
  (void*) (ROGUEM0) RogueOutOfBoundsError__init_object, // OutOfBoundsError
  (void*) (ROGUEM0) RogueException__init,
  0, // Object.hash_code()
  0, // Object.introspector()
  0, // Object.object_id()
  0, // Object.operator==(Object)
  (void*) (ROGUEM0) RogueException__to_String,
  0, // Object.to_Value()
  0, // Object.to_ValueList()
  0, // Object.to_ValueTable()
  0, // Object.type_info()
  0, // Object.unpack(Value)
  (void*) (ROGUEM0) RogueOutOfBoundsError__type_name,
  (void*) (ROGUEM2) RogueOutOfBoundsError__init__String,
  0, // Exception.display()
  0, // Exception.format()
  (void*) (ROGUEM0) RogueOutOfBoundsError___throw,
  0, // StateFlags.operator|(StateFlags) // StateFlags
  0, // SystemEnvironment.get(String) // SystemEnvironment
  0, // TokenType.to_Int32() // TokenType

};

const int Rogue_type_info_table[] ={
  // <FOR EACH TYPE>
  // info_count (total int count including this int)
  // allocator_index
  // dynamic_method_table_index
  // base_type_count
  // base_type_index[ base_type_count ]
  // global_property_count
  // global_property_name_indices[ global_property_count ]
  // global_property_type_indices[ global_property_count ]
  // property_count
  // property_name_indices[ property_count ]
  // property_type_indices[ property_count ]
  // dynamic_method_count
  // global_method_count
  16,0,0,3,44,1,2,0,3,78,79,80,2,3,14,13,0,8,0,13,1,2,0,0,1,0,7,0,
  14,0,0,0,1,0,24,0,15,1,44,2,81,82,4,10,6,83,84,85,86,87,88,4,9,9,
  9,9,13,13,0,13,0,28,2,5,44,0,2,89,84,7,9,13,0,8,0,41,1,44,0,0,13,
  0,7,0,0,0,0,0,1,0,9,0,55,2,8,44,0,0,13,0,8,0,68,1,44,0,0,13,0,
  7,0,0,0,0,0,1,0,10,0,82,1,44,0,1,90,11,13,0,13,0,95,2,5,44,0,2,89,
  84,12,9,13,0,9,0,108,2,8,44,0,0,13,0,7,0,0,0,0,0,0,0,13,0,121,2,5,
  44,0,2,89,84,16,9,13,0,8,0,134,1,44,0,0,14,0,9,0,148,2,8,44,0,0,13,
  0,8,0,161,1,44,0,0,13,0,14,0,174,1,44,0,3,91,84,92,19,9,13,13,0,13,0,187,
  2,5,44,0,2,89,84,20,9,13,0,9,0,200,2,8,44,0,0,13,0,7,0,0,0,0,0,
  0,0,7,0,0,0,0,0,1,0,7,0,0,0,0,0,1,0,13,0,215,2,5,44,0,2,89,84,
  25,9,13,0,9,0,228,2,8,44,0,0,13,0,9,0,241,0,0,1,93,9,1,0,31,0,242,4,
  44,26,28,2,0,10,93,94,95,96,97,98,99,100,101,102,9,29,13,13,13,3,3,48,4,
  22,13,0,8,0,255,1,2,0,0,1,0,12,0,256,3,44,28,2,0,1,98,3,13,0,8,0,269,
  1,44,0,0,13,0,9,0,282,2,15,44,0,0,14,0,11,0,296,2,33,44,0,1,103,34,13,
  0,10,0,309,1,44,0,1,103,34,13,0,18,0,322,1,44,0,5,104,105,106,107,108,9,9,49,
  9,37,13,0,13,0,335,2,5,44,0,2,89,84,36,9,13,0,9,0,348,2,8,44,0,0,13,
  0,13,0,361,2,5,44,0,2,89,84,39,9,13,0,16,0,374,1,44,0,4,109,110,111,112,23,
  50,9,34,13,0,9,0,387,2,8,44,0,0,13,0,8,0,400,1,44,0,0,13,0,14,0,413,1,
  44,3,113,114,115,19,17,51,0,13,0,12,0,426,1,44,0,2,116,117,42,22,13,0,11,0,439,
  2,15,44,0,1,78,27,14,0,7,0,453,0,0,0,13,0,12,0,466,1,44,0,2,118,119,17,
  18,14,0,13,0,480,2,45,44,0,2,118,119,17,18,14,0,14,0,494,3,46,45,44,0,2,118,
  119,17,18,17,0,11,0,511,0,0,2,111,120,9,13,0,0,13,0,511,0,2,121,122,19,35,1,
  111,9,1,0,13,0,512,0,2,121,122,19,35,1,111,9,0,0,7,0,512,0,0,0,1,0,13,0,
  513,0,2,121,122,19,35,1,111,9,1,0,
};

const int Rogue_object_size_table[53] =
{
  (int) sizeof(RogueClassGlobal),
  (int) sizeof(RogueClassPrintWriter_global_output_buffer_),
  (int) sizeof(RogueClassPrintWriter),
  (int) sizeof(RogueStringBuilder),
  (int) sizeof(RogueByte_List),
  (int) sizeof(RogueClassGenericList),
  (int) sizeof(RogueByte),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueInt32),
  (int) sizeof(RogueClassStringBuilderPool),
  (int) sizeof(RogueStringBuilder_List),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueLogical),
  (int) sizeof(Rogue_Function____List),
  (int) sizeof(RogueClass_Function___),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueString),
  (int) sizeof(RogueClassStackTrace),
  (int) sizeof(RogueString_List),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueReal64),
  (int) sizeof(RogueInt64),
  (int) sizeof(RogueCharacter),
  (int) sizeof(RogueCharacter_List),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueClassReader_Character_),
  (int) sizeof(RogueClassConsole),
  (int) sizeof(RogueClassPrintWriter_output_buffer_),
  (int) sizeof(RogueClassConsoleErrorPrinter),
  (int) sizeof(RogueClassMath),
  (int) sizeof(RogueClassFunction_540),
  (int) sizeof(RogueClassBossScanTableBuilder),
  (int) sizeof(RogueClassScanTableBuilder),
  (int) sizeof(RogueClassScanState),
  (int) sizeof(RogueInt32_List),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueTransition_List),
  (int) sizeof(RogueClassTransition),
  (int) sizeof(RogueArray),
  (int) sizeof(RogueClassRuntime),
  (int) sizeof(RogueClassSystem),
  (int) sizeof(RogueWeakReference),
  (int) sizeof(RogueClassFunction_1670),
  (int) sizeof(RogueObject),
  (int) sizeof(RogueException),
  (int) sizeof(RogueClassError),
  (int) sizeof(RogueClassOutOfBoundsError),
  (int) sizeof(RogueOptionalInt32),
  (int) sizeof(RogueClassStateFlags),
  (int) sizeof(RogueClassTransitionActions),
  (int) sizeof(RogueClassSystemEnvironment),
  (int) sizeof(RogueClassTokenType)
};

int Rogue_allocator_count = 1;
RogueAllocator Rogue_allocators[1];

int Rogue_type_count = 53;
RogueType Rogue_types[53];

RogueType* RogueTypeGlobal;
RogueType* RogueTypePrintWriter_global_output_buffer_;
RogueType* RogueTypePrintWriter;
RogueType* RogueTypeStringBuilder;
RogueType* RogueTypeByte_List;
RogueType* RogueTypeGenericList;
RogueType* RogueTypeByte;
RogueType* RogueTypeArray;
RogueType* RogueTypeInt32;
RogueType* RogueTypeStringBuilderPool;
RogueType* RogueTypeStringBuilder_List;
RogueType* RogueTypeLogical;
RogueType* RogueType_Function____List;
RogueType* RogueType_Function___;
RogueType* RogueTypeString;
RogueType* RogueTypeStackTrace;
RogueType* RogueTypeString_List;
RogueType* RogueTypeReal64;
RogueType* RogueTypeInt64;
RogueType* RogueTypeCharacter;
RogueType* RogueTypeCharacter_List;
RogueType* RogueTypeReader_Character_;
RogueType* RogueTypeConsole;
RogueType* RogueTypePrintWriter_output_buffer_;
RogueType* RogueTypeConsoleErrorPrinter;
RogueType* RogueTypeMath;
RogueType* RogueTypeFunction_540;
RogueType* RogueTypeBossScanTableBuilder;
RogueType* RogueTypeScanTableBuilder;
RogueType* RogueTypeScanState;
RogueType* RogueTypeInt32_List;
RogueType* RogueTypeTransition_List;
RogueType* RogueTypeTransition;
RogueType* RogueTypeRuntime;
RogueType* RogueTypeSystem;
RogueType* RogueTypeWeakReference;
RogueType* RogueTypeFunction_1670;
RogueType* RogueTypeObject;
RogueType* RogueTypeException;
RogueType* RogueTypeError;
RogueType* RogueTypeOutOfBoundsError;
RogueType* RogueTypeOptionalInt32;
RogueType* RogueTypeStateFlags;
RogueType* RogueTypeTransitionActions;
RogueType* RogueTypeSystemEnvironment;
RogueType* RogueTypeTokenType;

int Rogue_literal_string_count = 123;
RogueString* Rogue_literal_strings[123];

const char* Rogue_literal_c_strings[123] =
{
  "",
  "null",
  "Exception",
  "=",
  ",",
  "\n",
  "(",
  "Object",
  " 0x",
  ")",
  "No stack trace",
  "Unknown",
  "ROGUE_GC_THRESHOLD",
  "MB",
  "KB",
  "-9223372036854775808",
  "Index ",
  " is out of bounds (data has zero elements).",
  " is out of bounds (data has one element at index 0).",
  " is out of bounds (data has ",
  "# element",
  "#",
  "es",
  "s",
  " indices ",
  "..",
  ").",
  "Global",
  "String",
  "StringBuilder",
  "GenericList",
  "Array",
  "StackTrace",
  "Console",
  "Math",
  "StringBuilderPool",
  "(Function())",
  "ScanTableBuilder",
  "Runtime",
  "System",
  "WeakReference",
  "ConsoleErrorPrinter",
  "ScanState",
  "Transition",
  "Byte[]",
  "Character[]",
  "String[]",
  "StringBuilder[]",
  "(Function())[]",
  "Int32[]",
  "Transition[]",
  "Array<<String>>",
  "Array<<Byte>>",
  "Array<<(Function())>>",
  "Array<<StringBuilder>>",
  "Array<<Character>>",
  "Array<<Int32>>",
  "Array<<Transition>>",
  "Function_540",
  "Function_1670",
  "Error",
  "BossScanTableBuilder",
  "OutOfBoundsError",
  "PrintWriter<<global_output_buffer>>",
  "PrintWriter",
  "Byte",
  "Int32",
  "Logical",
  "Real64",
  "Int64",
  "Character",
  "Reader<<Character>>",
  "PrintWriter<<output_buffer>>",
  "Int32?",
  "StateFlags",
  "TransitionActions",
  "SystemEnvironment",
  "TokenType",
  "console",
  "global_output_buffer",
  "exit_functions",
  "work_bytes",
  "pool",
  "utf8",
  "count",
  "indent",
  "cursor_offset",
  "cursor_index",
  "at_newline",
  "data",
  "available",
  "entries",
  "is_formatted",
  "position",
  "error",
  "immediate_mode",
  "is_blocking",
  "decode_bytes",
  "output_buffer",
  "input_buffer",
  "next_input_character",
  "input_bytes",
  "#if (ROGUE_CONSOLE_FULL)\n  termios original_terminal_settings;\n  int     original_stdin_flags;\n#endif",
  "start",
  "type",
  "option",
  "flags",
  "index",
  "transitions",
  "character",
  "actions",
  "value",
  "target_state",
  "command_line_arguments",
  "executable_filepath",
  "environment",
  "next_weak_reference",
  "RogueObject* value;",
  "message",
  "stack_trace",
  "exists",
  "names",
  "values"
};

const int Rogue_accessible_type_count = 46;

RogueType** Rogue_accessible_type_pointers[46] =
{
  &RogueTypeGlobal,
  &RogueTypePrintWriter_global_output_buffer_,
  &RogueTypePrintWriter,
  &RogueTypeStringBuilder,
  &RogueTypeByte_List,
  &RogueTypeGenericList,
  &RogueTypeByte,
  &RogueTypeArray,
  &RogueTypeInt32,
  &RogueTypeStringBuilderPool,
  &RogueTypeStringBuilder_List,
  &RogueTypeLogical,
  &RogueType_Function____List,
  &RogueType_Function___,
  &RogueTypeString,
  &RogueTypeStackTrace,
  &RogueTypeString_List,
  &RogueTypeReal64,
  &RogueTypeInt64,
  &RogueTypeCharacter,
  &RogueTypeCharacter_List,
  &RogueTypeReader_Character_,
  &RogueTypeConsole,
  &RogueTypePrintWriter_output_buffer_,
  &RogueTypeConsoleErrorPrinter,
  &RogueTypeMath,
  &RogueTypeFunction_540,
  &RogueTypeBossScanTableBuilder,
  &RogueTypeScanTableBuilder,
  &RogueTypeScanState,
  &RogueTypeInt32_List,
  &RogueTypeTransition_List,
  &RogueTypeTransition,
  &RogueTypeRuntime,
  &RogueTypeSystem,
  &RogueTypeWeakReference,
  &RogueTypeFunction_1670,
  &RogueTypeObject,
  &RogueTypeException,
  &RogueTypeError,
  &RogueTypeOutOfBoundsError,
  &RogueTypeOptionalInt32,
  &RogueTypeStateFlags,
  &RogueTypeTransitionActions,
  &RogueTypeSystemEnvironment,
  &RogueTypeTokenType
};

const int Rogue_accessible_type_indices[46] =
{
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  8,
  9,
  10,
  11,
  13,
  14,
  15,
  17,
  18,
  19,
  21,
  22,
  23,
  24,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  33,
  34,
  35,
  37,
  38,
  40,
  41,
  42,
  43,
  44,
  45,
  46,
  47,
  48,
  49,
  50,
  51,
  52
};

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/StringBuilder.rogue"
void RogueStringBuilder__init_class_thread_local()
{
  ROGUE_GC_CHECK;
  #line 3
  RogueStringBuilder_work_bytes = ((RogueByte_List*)(RogueByte_List__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueByte_List*,ROGUE_CREATE_OBJECT(Byte_List))) )));
  #line 5
  RogueStringBuilder_pool = ((RogueClassStringBuilderPool*)(((RogueObject*)Rogue_call_ROGUEM0( 1, ROGUE_ARG(((RogueObject*)ROGUE_CREATE_REF(RogueClassStringBuilderPool*,ROGUE_CREATE_OBJECT(StringBuilderPool)))) ))));
}

#line 3 "../../Rogue/Programs/RogueC/Libraries/Standard/String.rogue"
RogueString* RogueString__create__Byte_List( RogueByte_List* utf8_0 )
{
  ROGUE_GC_CHECK;
  #line 4
  return (RogueString*)(((RogueString*)(RogueString_create_from_utf8( (char*)utf8_0->data->as_bytes, utf8_0->count ))));
}

#line 61
RogueString* RogueString__operatorPLUS__String_String( RogueString* st_0, RogueString* value_1 )
{
  ROGUE_GC_CHECK;
  #line 62
  ROGUE_DEF_LOCAL_REF(RogueString*,_auto_135_2,(st_0));
  return (RogueString*)(((RogueString*)(RogueString__operatorPLUS__String( ROGUE_ARG(((((_auto_135_2))) ? (ROGUE_ARG(_auto_135_2)) : ROGUE_ARG(Rogue_literal_strings[1]))), value_1 ))));
}

#line 64
RogueString* RogueString__operatorTIMES__String_Int32( RogueString* st_0, RogueInt32 value_1 )
{
  ROGUE_GC_CHECK;
  #line 65
  if (ROGUE_COND(((void*)st_0) == ((void*)NULL)))
  {
    return (RogueString*)(((RogueString*)(NULL)));
  }
  #line 66
  return (RogueString*)(((RogueString*)(RogueString__operatorTIMES__Int32( st_0, value_1 ))));
}

#line 742 "../../Rogue/Programs/RogueC/Libraries/Standard/Primitives.rogue"
RogueCharacter RogueCharacter__create__Int32( RogueInt32 value_0 )
{
  ROGUE_GC_CHECK;
  #line 743
  return (RogueCharacter)(((RogueCharacter)(value_0)));
}

#line 745
RogueCharacter RogueCharacter__create__Character( RogueCharacter value_0 )
{
  ROGUE_GC_CHECK;
  #line 746
  return (RogueCharacter)(value_0);
}

#line 61 "../../Rogue/Programs/RogueC/Libraries/Standard/Math.rogue"
RogueInt32 RogueMath__max__Int32_Int32( RogueInt32 a_0, RogueInt32 b_1 )
{
  ROGUE_GC_CHECK;
  #line 62
  if (ROGUE_COND(((a_0) >= (b_1))))
  {
    return (RogueInt32)(a_0);
  }
  else
  {
    #line 63
    return (RogueInt32)(b_1);
  }
}

#line 96
RogueInt64 RogueMath__mod__Int64_Int64( RogueInt64 a_0, RogueInt64 b_1 )
{
  ROGUE_GC_CHECK;
  #line 97
  if (ROGUE_COND(((((!(!!(a_0))) && (!(!!(b_1))))) || (((b_1) == (1LL))))))
  {
    return (RogueInt64)(0LL);
  }
  #line 99
  RogueInt64 r_2 = (((RogueInt64)(a_0 % b_1
  )));
  #line 100
  if (ROGUE_COND(((((a_0) ^ (b_1))) < (0LL))))
  {
    #line 101
    if (ROGUE_COND(!!(r_2)))
    {
      return (RogueInt64)(((r_2) + (b_1)));
    }
    else
    {
      #line 102
      return (RogueInt64)(0LL);
    }
  }
  else
  {
    #line 104
    return (RogueInt64)(r_2);
  }
}

#line 118
RogueInt64 RogueMath__shift_right__Int64_Int64( RogueInt64 value_0, RogueInt64 bits_1 )
{
  ROGUE_GC_CHECK;
  #line 119
  if (ROGUE_COND(((bits_1) <= (0LL))))
  {
    return (RogueInt64)(value_0);
  }
  #line 121
  --bits_1;
  #line 122
  if (ROGUE_COND(!!(bits_1)))
  {
    return (RogueInt64)(((((((value_0) >> (1LL))) & (9223372036854775807LL))) >> (bits_1)));
  }
  else
  {
    #line 123
    return (RogueInt64)(((((value_0) >> (1LL))) & (9223372036854775807LL)));
  }
}

#line 3 "../../Rogue/Programs/RogueC/Libraries/Standard/Runtime.rogue"
void RogueRuntime__init_class()
{
  ROGUE_GC_CHECK;
  #line 4
  ROGUE_DEF_LOCAL_REF(RogueString*,value_0,(((RogueString*)(RogueSystemEnvironment__get__String( RogueSystem_environment, Rogue_literal_strings[12] )))));
  #line 5
  if (ROGUE_COND(((void*)value_0) != ((void*)NULL)))
  {
    #line 6
    RogueReal64 n_1 = (((RogueString__to_Real64( value_0 ))));
    #line 7
    if (ROGUE_COND(((((RogueString__ends_with__Character_Logical( value_0, (RogueCharacter)'M', false )))) || (((RogueString__ends_with__String_Logical( value_0, Rogue_literal_strings[13], false )))))))
    {
      n_1 *= 1048576;
    }
    else if (ROGUE_COND(((((RogueString__ends_with__Character_Logical( value_0, (RogueCharacter)'K', false )))) || (((RogueString__ends_with__String_Logical( value_0, Rogue_literal_strings[14], false )))))))
    {
      #line 8
      n_1 *= 1024;
    }
    #line 9
    RogueRuntime__set_gc_threshold__Int32( ROGUE_ARG(((RogueInt32)(n_1))) );
  }
}

#line 91
void RogueRuntime__set_gc_threshold__Int32( RogueInt32 value_0 )
{
  ROGUE_GC_CHECK;
  #line 92
  if (ROGUE_COND(((value_0) <= (0))))
  {
    value_0 = ((RogueInt32)(2147483647));
  }
  #line 96
  Rogue_gc_threshold = value_0;

}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/System.rogue"
void RogueSystem__init_class()
{
  #line 10
  RogueSystem_command_line_arguments = ((RogueString_List*)(RogueString_List__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueString_List*,ROGUE_CREATE_OBJECT(String_List))) )));
}

#line 3 "../../Rogue/Programs/RogueC/Libraries/Standard/Optional.rogue"
RogueOptionalInt32 RogueOptionalInt32__create()
{
  ROGUE_GC_CHECK;
  #line 4
  RogueInt32 default_value_0 = 0;
  #line 5
  return (RogueOptionalInt32)(RogueOptionalInt32( default_value_0, false ));
}

#line 3 "../ScanTableBuilder.rogue"
void RogueStateFlags__init_class()
{
  ROGUE_GC_CHECK;
  ROGUE_DEF_LOCAL_REF(RogueInt32_List*,_auto_1707_0,(((RogueInt32_List*)(RogueInt32_List__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueInt32_List*,ROGUE_CREATE_OBJECT(Int32_List))), 4 )))));
  {
    RogueInt32_List__add__Int32( _auto_1707_0, 0 );
    RogueInt32_List__add__Int32( _auto_1707_0, 1 );
    RogueInt32_List__add__Int32( _auto_1707_0, 3 );
    RogueInt32_List__add__Int32( _auto_1707_0, 4 );
  }
  RogueStateFlags_values = _auto_1707_0;
}

#line 10
void RogueTransitionActions__init_class()
{
  ROGUE_GC_CHECK;
  ROGUE_DEF_LOCAL_REF(RogueInt32_List*,_auto_1860_0,(((RogueInt32_List*)(RogueInt32_List__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueInt32_List*,ROGUE_CREATE_OBJECT(Int32_List))), 6 )))));
  {
    RogueInt32_List__add__Int32( _auto_1860_0, 0 );
    RogueInt32_List__add__Int32( _auto_1860_0, 8 );
    RogueInt32_List__add__Int32( _auto_1860_0, 16 );
    RogueInt32_List__add__Int32( _auto_1860_0, 32 );
    RogueInt32_List__add__Int32( _auto_1860_0, 64 );
    RogueInt32_List__add__Int32( _auto_1860_0, 128 );
  }
  RogueTransitionActions_values = _auto_1860_0;
}

#line 3 "../Boss.rogue"
void RogueTokenType__init_class()
{
  ROGUE_GC_CHECK;
  ROGUE_DEF_LOCAL_REF(RogueInt32_List*,_auto_2493_0,(((RogueInt32_List*)(RogueInt32_List__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueInt32_List*,ROGUE_CREATE_OBJECT(Int32_List))), 3 )))));
  {
    RogueInt32_List__add__Int32( _auto_2493_0, 0 );
    RogueInt32_List__add__Int32( _auto_2493_0, 1 );
    RogueInt32_List__add__Int32( _auto_2493_0, 2 );
  }
  RogueTokenType_values = _auto_2493_0;
}


#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/Global.rogue"
RogueClassGlobal* RogueGlobal__init_object( RogueClassGlobal* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 3
  THIS->console = ((RogueClassPrintWriter*)(((RogueClassConsole*)ROGUE_SINGLETON(Console))));
  #line 4
  THIS->global_output_buffer = ((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )));
  #line 1
  return (RogueClassGlobal*)(THIS);
}

#line 8
RogueClassGlobal* RogueGlobal__init( RogueClassGlobal* THIS )
{
  ROGUE_GC_CHECK;
  #line 9
  RogueGlobal__on_exit___Function___( ROGUE_ARG(THIS), ROGUE_ARG(((RogueClass_Function___*)(((RogueClassFunction_540*)ROGUE_SINGLETON(Function_540))))) );
  #line 11
  return (RogueClassGlobal*)(THIS);
}

#line 1
RogueString* RogueGlobal__type_name( RogueClassGlobal* THIS )
{
  return (RogueString*)(Rogue_literal_strings[27]);
}

#line 47 "../../Rogue/Programs/RogueC/Libraries/Standard/PrintWriter.rogue"
RogueClassGlobal* RogueGlobal__flush( RogueClassGlobal* THIS )
{
  ROGUE_GC_CHECK;
  #line 48
  RogueGlobal__write__StringBuilder( ROGUE_ARG(THIS), ROGUE_ARG(THIS->global_output_buffer) );
  #line 49
  RogueStringBuilder__clear( ROGUE_ARG(THIS->global_output_buffer) );
  #line 50
  return (RogueClassGlobal*)(THIS);
}

#line 77
RogueClassGlobal* RogueGlobal__print__Object( RogueClassGlobal* THIS, RogueObject* value_0 )
{
  ROGUE_GC_CHECK;
  #line 78
  RogueStringBuilder__print__Object( ROGUE_ARG(THIS->global_output_buffer), value_0 );
  #line 79
  return (RogueClassGlobal*)(THIS);
}

#line 94
RogueClassGlobal* RogueGlobal__println( RogueClassGlobal* THIS )
{
  ROGUE_GC_CHECK;
  #line 95
  RogueStringBuilder__print__Character_Logical( ROGUE_ARG(THIS->global_output_buffer), (RogueCharacter)10, true );
  #line 96
  return (RogueClassGlobal*)(((RogueClassGlobal*)(RogueGlobal__flush( ROGUE_ARG(THIS) ))));
}

#line 122
RogueClassGlobal* RogueGlobal__println__Object( RogueClassGlobal* THIS, RogueObject* value_0 )
{
  ROGUE_GC_CHECK;
  #line 123
  return (RogueClassGlobal*)(((RogueClassGlobal*)(RogueGlobal__println( ROGUE_ARG(((RogueClassGlobal*)(RogueGlobal__print__Object( ROGUE_ARG(THIS), value_0 )))) ))));
}

#line 27 "../../Rogue/Programs/RogueC/Libraries/Standard/Global.rogue"
RogueClassGlobal* RogueGlobal__write__StringBuilder( RogueClassGlobal* THIS, RogueStringBuilder* buffer_0 )
{
  ROGUE_GC_CHECK;
  #line 28
  RoguePrintWriter__flush( ROGUE_ARG(((((RogueObject*)(RoguePrintWriter__write__StringBuilder( ROGUE_ARG(((((RogueObject*)THIS->console)))), buffer_0 )))))) );
  #line 29
  return (RogueClassGlobal*)(THIS);
}

#line 9 "../Boss.rogue"
void RogueGlobal__on_launch( RogueClassGlobal* THIS )
{
  ROGUE_GC_CHECK;
  ROGUE_DEF_LOCAL_REF(RogueClassBossScanTableBuilder*,builder_0,(((RogueClassBossScanTableBuilder*)(RogueBossScanTableBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassBossScanTableBuilder*,ROGUE_CREATE_OBJECT(BossScanTableBuilder))) )))));
  #line 64 "../../Rogue/Programs/RogueC/Libraries/Standard/Standard.rogue"
  #line 66
}

#line 0 "[Built-in Type]"
void RogueGlobal__run_tests( RogueClassGlobal* THIS )
{
}

#line 11 "../../Rogue/Programs/RogueC/Libraries/Standard/Global.rogue"
void RogueGlobal__call_exit_functions( RogueClassGlobal* THIS )
{
  ROGUE_GC_CHECK;
  #line 16
  ROGUE_DEF_LOCAL_REF(Rogue_Function____List*,functions_0,(THIS->exit_functions));
  #line 17
  THIS->exit_functions = ((Rogue_Function____List*)(NULL));
  #line 19
  if (ROGUE_COND(!!(functions_0)))
  {
    #line 20
    {
      ROGUE_DEF_LOCAL_REF(Rogue_Function____List*,_auto_616_0,(functions_0));
      RogueInt32 _auto_617_0 = (0);
      RogueInt32 _auto_618_0 = (((_auto_616_0->count) - (1)));
      for (;ROGUE_COND(((_auto_617_0) <= (_auto_618_0)));++_auto_617_0)
      {
        ROGUE_GC_CHECK;
        ROGUE_DEF_LOCAL_REF(RogueClass_Function___*,fn_0,(((RogueClass_Function___*)(_auto_616_0->data->as_objects[_auto_617_0]))));
        Rogue_call_ROGUEM1( 13, fn_0 );
      }
    }
  }
}

#line 23
void RogueGlobal__on_exit___Function___( RogueClassGlobal* THIS, RogueClass_Function___* fn_0 )
{
  ROGUE_GC_CHECK;
  #line 24
  if (ROGUE_COND(!(!!(THIS->exit_functions))))
  {
    THIS->exit_functions = ((Rogue_Function____List*)(Rogue_Function____List__init( ROGUE_ARG(ROGUE_CREATE_REF(Rogue_Function____List*,ROGUE_CREATE_OBJECT(_Function____List))) )));
  }
  #line 25
  Rogue_Function____List__add___Function___( ROGUE_ARG(THIS->exit_functions), fn_0 );
}

#line 47 "../../Rogue/Programs/RogueC/Libraries/Standard/PrintWriter.rogue"
RogueClassPrintWriter_global_output_buffer_* RoguePrintWriter_global_output_buffer___flush( RogueObject* THIS )
{
  switch (THIS->type->index)
  {
    case 0:
      return (RogueClassPrintWriter_global_output_buffer_*)RogueGlobal__flush( (RogueClassGlobal*)THIS );
    default:
      return 0;
  }
}

#line 38
RogueClassPrintWriter_global_output_buffer_* RoguePrintWriter_global_output_buffer___write__StringBuilder( RogueObject* THIS, RogueStringBuilder* buffer_0 )
{
  switch (THIS->type->index)
  {
    case 0:
      return (RogueClassPrintWriter_global_output_buffer_*)RogueGlobal__write__StringBuilder( (RogueClassGlobal*)THIS, buffer_0 );
    default:
      return 0;
  }
}

#line 8
RogueClassPrintWriter* RoguePrintWriter__flush( RogueObject* THIS )
{
  switch (THIS->type->index)
  {
    case 0:
      return (RogueClassPrintWriter*)RogueGlobal__flush( (RogueClassGlobal*)THIS );
    case 27:
      return (RogueClassPrintWriter*)RogueConsole__flush( (RogueClassConsole*)THIS );
    case 29:
      return (RogueClassPrintWriter*)RogueConsoleErrorPrinter__flush( (RogueClassConsoleErrorPrinter*)THIS );
    default:
      return 0;
  }
}

#line 38
RogueClassPrintWriter* RoguePrintWriter__write__StringBuilder( RogueObject* THIS, RogueStringBuilder* buffer_0 )
{
  switch (THIS->type->index)
  {
    case 0:
      return (RogueClassPrintWriter*)RogueGlobal__write__StringBuilder( (RogueClassGlobal*)THIS, buffer_0 );
    case 27:
      return (RogueClassPrintWriter*)RogueConsole__write__StringBuilder( (RogueClassConsole*)THIS, buffer_0 );
    case 29:
      return (RogueClassPrintWriter*)RogueConsoleErrorPrinter__write__StringBuilder( (RogueClassConsoleErrorPrinter*)THIS, buffer_0 );
    default:
      return 0;
  }
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/StringBuilder.rogue"
RogueStringBuilder* RogueStringBuilder__init_object( RogueStringBuilder* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 20
  THIS->at_newline = true;
  #line 1
  return (RogueStringBuilder*)(THIS);
}

#line 23
RogueStringBuilder* RogueStringBuilder__init( RogueStringBuilder* THIS )
{
  ROGUE_GC_CHECK;
  #line 24
  RogueStringBuilder__init__Int32( ROGUE_ARG(THIS), 40 );
  #line 26
  return (RogueStringBuilder*)(THIS);
}

#line 450
RogueString* RogueStringBuilder__to_String( RogueStringBuilder* THIS )
{
  ROGUE_GC_CHECK;
  #line 451
  return (RogueString*)((RogueString__create__Byte_List( ROGUE_ARG(THIS->utf8) )));
}

#line 1
RogueString* RogueStringBuilder__type_name( RogueStringBuilder* THIS )
{
  return (RogueString*)(Rogue_literal_strings[29]);
}

#line 26
RogueStringBuilder* RogueStringBuilder__init__Int32( RogueStringBuilder* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 27
  THIS->utf8 = ((RogueByte_List*)(RogueByte_List__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueByte_List*,ROGUE_CREATE_OBJECT(Byte_List))), initial_capacity_0 )));
  #line 29
  return (RogueStringBuilder*)(THIS);
}

#line 39
RogueStringBuilder* RogueStringBuilder__clear( RogueStringBuilder* THIS )
{
  ROGUE_GC_CHECK;
  #line 40
  RogueByte_List__clear( ROGUE_ARG(THIS->utf8) );
  #line 41
  THIS->count = 0;
  #line 42
  THIS->at_newline = true;
  #line 43
  THIS->cursor_index = 0;
  #line 44
  THIS->cursor_offset = 0;
  #line 45
  return (RogueStringBuilder*)(THIS);
}

#line 131
RogueLogical RogueStringBuilder__needs_indent( RogueStringBuilder* THIS )
{
  ROGUE_GC_CHECK;
  #line 132
  return (RogueLogical)(((THIS->at_newline) && (((THIS->indent) > (0)))));
}

#line 153
RogueStringBuilder* RogueStringBuilder__print__Character_Logical( RogueStringBuilder* THIS, RogueCharacter value_0, RogueLogical formatted_1 )
{
  ROGUE_GC_CHECK;
  #line 154
  if (ROGUE_COND(formatted_1))
  {
    #line 155
    if (ROGUE_COND(((value_0) == ((RogueCharacter)10))))
    {
      THIS->at_newline = true;
    }
    else if (ROGUE_COND(((RogueStringBuilder__needs_indent( ROGUE_ARG(THIS) )))))
    {
      #line 156
      RogueStringBuilder__print_indent( ROGUE_ARG(THIS) );
    }
  }
  #line 159
  ++THIS->count;
  #line 160
  if (ROGUE_COND(((((RogueInt32)(value_0))) < (0))))
  {
    #line 161
    RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), 0 );
  }
  else if (ROGUE_COND(((value_0) <= ((RogueCharacter__create__Int32( 127 ))))))
  {
    #line 163
    RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), ROGUE_ARG(((RogueByte)(value_0))) );
  }
  else if (ROGUE_COND(((value_0) <= ((RogueCharacter__create__Int32( 2047 ))))))
  {
    #line 165
    RogueByte_List__add__Byte( ROGUE_ARG(((RogueByte_List*)(RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), ROGUE_ARG(((RogueByte)(((192) | (((((RogueInt32)(value_0))) >> (6))))))) )))), ROGUE_ARG(((RogueByte)(((128) | (((((RogueInt32)(value_0))) & (63))))))) );
  }
  else if (ROGUE_COND(((value_0) <= ((RogueCharacter__create__Int32( 65535 ))))))
  {
    #line 167
    RogueByte_List__add__Byte( ROGUE_ARG(((RogueByte_List*)(RogueByte_List__add__Byte( ROGUE_ARG(((RogueByte_List*)(RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), ROGUE_ARG(((RogueByte)(((224) | (((((RogueInt32)(value_0))) >> (12))))))) )))), ROGUE_ARG(((RogueByte)(((128) | (((((((RogueInt32)(value_0))) >> (6))) & (63))))))) )))), ROGUE_ARG(((RogueByte)(((128) | (((((RogueInt32)(value_0))) & (63))))))) );
  }
  else if (ROGUE_COND(((value_0) <= ((RogueCharacter__create__Int32( 1114111 ))))))
  {
    #line 169
    RogueByte_List__add__Byte( ROGUE_ARG(((RogueByte_List*)(RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), ROGUE_ARG(((RogueByte)(((240) | (((((RogueInt32)(value_0))) >> (18))))))) )))), ROGUE_ARG(((RogueByte)(((128) | (((((((RogueInt32)(value_0))) >> (12))) & (63))))))) );
    #line 170
    RogueByte_List__add__Byte( ROGUE_ARG(((RogueByte_List*)(RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), ROGUE_ARG(((RogueByte)(((128) | (((((((RogueInt32)(value_0))) >> (6))) & (63))))))) )))), ROGUE_ARG(((RogueByte)(((128) | (((((RogueInt32)(value_0))) & (63))))))) );
  }
  else
  {
    #line 172
    RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), 63 );
  }
  #line 174
  return (RogueStringBuilder*)(THIS);
}

#line 179
RogueStringBuilder* RogueStringBuilder__print__Int32( RogueStringBuilder* THIS, RogueInt32 value_0 )
{
  ROGUE_GC_CHECK;
  #line 180
  return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__Int64( ROGUE_ARG(THIS), ROGUE_ARG(((RogueInt64)(value_0))) ))));
}

#line 186
RogueStringBuilder* RogueStringBuilder__print__Int64( RogueStringBuilder* THIS, RogueInt64 value_0 )
{
  ROGUE_GC_CHECK;
  #line 187
  if (ROGUE_COND(((value_0) == (((1LL) << (63LL))))))
  {
    #line 188
    return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(THIS), Rogue_literal_strings[15] ))));
  }
  else if (ROGUE_COND(((value_0) < (0LL))))
  {
    #line 190
    RogueStringBuilder__print__Character_Logical( ROGUE_ARG(THIS), (RogueCharacter)'-', true );
    #line 191
    return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__Int64( ROGUE_ARG(THIS), ROGUE_ARG((-(value_0))) ))));
  }
  else if (ROGUE_COND(((value_0) >= (10LL))))
  {
    #line 193
    RogueStringBuilder__print__Int64( ROGUE_ARG(THIS), ROGUE_ARG(((value_0) / (10LL))) );
    #line 194
    return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__Character_Logical( ROGUE_ARG(THIS), ROGUE_ARG(((RogueCharacter)(((48LL) + ((RogueMath__mod__Int64_Int64( value_0, 10LL ))))))), true ))));
  }
  else
  {
    #line 196
    return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__Character_Logical( ROGUE_ARG(THIS), ROGUE_ARG(((RogueCharacter)(((48LL) + (value_0))))), true ))));
  }
}

#line 200
RogueStringBuilder* RogueStringBuilder__print__Object( RogueStringBuilder* THIS, RogueObject* value_0 )
{
  ROGUE_GC_CHECK;
  #line 201
  if (ROGUE_COND(!!(value_0)))
  {
    return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(THIS), ROGUE_ARG(((RogueString*)Rogue_call_ROGUEM0( 6, value_0 ))) ))));
  }
  #line 202
  return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(THIS), Rogue_literal_strings[1] ))));
}

#line 273
RogueStringBuilder* RogueStringBuilder__print__String( RogueStringBuilder* THIS, RogueString* value_0 )
{
  ROGUE_GC_CHECK;
  #line 274
  if (ROGUE_COND(!!(value_0)))
  {
    #line 275
    if (ROGUE_COND(!!(THIS->indent)))
    {
      #line 276
      {
        ROGUE_DEF_LOCAL_REF(RogueString*,_auto_505_0,(value_0));
        RogueInt32 _auto_506_0 = (0);
        RogueInt32 _auto_507_0 = (((((RogueString__count( _auto_505_0 )))) - (1)));
        for (;ROGUE_COND(((_auto_506_0) <= (_auto_507_0)));++_auto_506_0)
        {
          ROGUE_GC_CHECK;
          RogueCharacter ch_0 = (((RogueString__get__Int32( _auto_505_0, _auto_506_0 ))));
          RogueStringBuilder__print__Character_Logical( ROGUE_ARG(THIS), ch_0, true );
        }
      }
    }
    else
    {
      #line 278
      {
        RogueInt32 i_1 = (0);
        RogueInt32 _auto_1_2 = (((RogueString__byte_count( value_0 ))));
        for (;ROGUE_COND(((i_1) < (_auto_1_2)));++i_1)
        {
          ROGUE_GC_CHECK;
          RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), ROGUE_ARG(((RogueString__byte__Int32( value_0, i_1 )))) );
        }
      }
      #line 279
      THIS->count += ((RogueString__count( value_0 )));
      #line 280
      if (ROGUE_COND(((!!(((RogueString__count( value_0 ))))) && (((((RogueString__last( value_0 )))) == ((RogueCharacter)10))))))
      {
        THIS->at_newline = true;
      }
    }
    #line 282
    return (RogueStringBuilder*)(THIS);
  }
  else
  {
    #line 284
    return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(THIS), Rogue_literal_strings[1] ))));
  }
}

#line 287
void RogueStringBuilder__print_indent( RogueStringBuilder* THIS )
{
  ROGUE_GC_CHECK;
  #line 288
  if (ROGUE_COND(((!(((RogueStringBuilder__needs_indent( ROGUE_ARG(THIS) ))))) || (((THIS->indent) == (0))))))
  {
    return;
  }
  #line 289
  {
    RogueInt32 i_0 = (1);
    RogueInt32 _auto_2_1 = (THIS->indent);
    for (;ROGUE_COND(((i_0) <= (_auto_2_1)));++i_0)
    {
      ROGUE_GC_CHECK;
      RogueByte_List__add__Byte( ROGUE_ARG(THIS->utf8), 32 );
    }
  }
  #line 290
  THIS->count += THIS->indent;
  #line 291
  THIS->at_newline = false;
}

#line 327
RogueStringBuilder* RogueStringBuilder__println( RogueStringBuilder* THIS )
{
  ROGUE_GC_CHECK;
  #line 328
  return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__Character_Logical( ROGUE_ARG(THIS), (RogueCharacter)10, true ))));
}

#line 360
RogueStringBuilder* RogueStringBuilder__println__String( RogueStringBuilder* THIS, RogueString* value_0 )
{
  ROGUE_GC_CHECK;
  #line 361
  return (RogueStringBuilder*)(((RogueStringBuilder*)(RogueStringBuilder__print__Character_Logical( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(THIS), value_0 )))), (RogueCharacter)10, true ))));
}

#line 380
RogueStringBuilder* RogueStringBuilder__reserve__Int32( RogueStringBuilder* THIS, RogueInt32 additional_bytes_0 )
{
  ROGUE_GC_CHECK;
  #line 381
  RogueByte_List__reserve__Int32( ROGUE_ARG(THIS->utf8), additional_bytes_0 );
  #line 382
  return (RogueStringBuilder*)(THIS);
}

#line 4 "../../Rogue/Programs/RogueC/Libraries/Standard/List.rogue"
RogueByte_List* RogueByte_List__init_object( RogueByte_List* THIS )
{
  ROGUE_GC_CHECK;
  RogueGenericList__init_object( ROGUE_ARG(((RogueClassGenericList*)THIS)) );
  return (RogueByte_List*)(THIS);
}

#line 10
RogueByte_List* RogueByte_List__init( RogueByte_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 11
  RogueByte_List__init__Int32( ROGUE_ARG(THIS), 0 );
  #line 13
  return (RogueByte_List*)(THIS);
}

#line 633
RogueString* RogueByte_List__to_String( RogueByte_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 634
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 635
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)'[', true );
  #line 636
  RogueLogical first_1 = (true);
  #line 637
  {
    ROGUE_DEF_LOCAL_REF(RogueByte_List*,_auto_644_0,(THIS));
    RogueInt32 _auto_645_0 = (0);
    RogueInt32 _auto_646_0 = (((_auto_644_0->count) - (1)));
    for (;ROGUE_COND(((_auto_645_0) <= (_auto_646_0)));++_auto_645_0)
    {
      ROGUE_GC_CHECK;
      RogueByte value_0 = (_auto_644_0->data->as_bytes[_auto_645_0]);
      #line 638
      if (ROGUE_COND(first_1))
      {
        first_1 = ((RogueLogical)(false));
      }
      else
      {
        #line 639
        RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)',', true );
      }
      #line 640
      if (ROGUE_COND((false)))
      {
      }
      else
      {
        #line 641
        RogueStringBuilder__print__String( buffer_0, ROGUE_ARG(((RogueString*)(RogueByte__to_String( value_0 )))) );
      }
    }
  }
  #line 643
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)']', true );
  #line 644
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_0 ))));
}

#line 4
RogueString* RogueByte_List__type_name( RogueByte_List* THIS )
{
  return (RogueString*)(Rogue_literal_strings[44]);
}

#line 13
RogueByte_List* RogueByte_List__init__Int32( RogueByte_List* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 14
  if (ROGUE_COND(((initial_capacity_0) != (((RogueByte_List__capacity( ROGUE_ARG(THIS) )))))))
  {
    THIS->data = RogueType_create_array( initial_capacity_0, sizeof(RogueByte), false, 6 );
  }
  #line 15
  RogueByte_List__clear( ROGUE_ARG(THIS) );
  #line 17
  return (RogueByte_List*)(THIS);
}

#line 58
RogueByte_List* RogueByte_List__add__Byte( RogueByte_List* THIS, RogueByte value_0 )
{
  ROGUE_GC_CHECK;
  #line 59
  RogueByte_List__set__Int32_Byte( ROGUE_ARG(((RogueByte_List*)(RogueByte_List__reserve__Int32( ROGUE_ARG(THIS), 1 )))), ROGUE_ARG(THIS->count), value_0 );
  #line 60
  THIS->count = ((THIS->count) + (1));
  #line 61
  return (RogueByte_List*)(THIS);
}

#line 68
RogueInt32 RogueByte_List__capacity( RogueByte_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 69
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    return (RogueInt32)(0);
  }
  #line 70
  return (RogueInt32)(THIS->data->count);
}

#line 72
RogueByte_List* RogueByte_List__clear( RogueByte_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 73
  RogueByte_List__discard_from__Int32( ROGUE_ARG(THIS), 0 );
  #line 74
  return (RogueByte_List*)(THIS);
}

#line 115
RogueByte RogueByte_List__first( RogueByte_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 116
  return (RogueByte)(THIS->data->as_bytes[0]);
}

#line 174
RogueByte_List* RogueByte_List__discard_from__Int32( RogueByte_List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 175
  if (ROGUE_COND(!!(THIS->data)))
  {
    #line 176
    RogueInt32 n_1 = (((THIS->count) - (index_0)));
    #line 177
    if (ROGUE_COND(((n_1) > (0))))
    {
      #line 178
      RogueArray__zero__Int32_Int32( ROGUE_ARG(((RogueArray*)THIS->data)), index_0, n_1 );
      #line 179
      THIS->count = index_0;
    }
  }
  #line 182
  return (RogueByte_List*)(THIS);
}

#line 467
RogueByte_List* RogueByte_List__reserve__Int32( RogueByte_List* THIS, RogueInt32 additional_elements_0 )
{
  ROGUE_GC_CHECK;
  #line 468
  RogueInt32 required_capacity_1 = (((THIS->count) + (additional_elements_0)));
  #line 469
  if (ROGUE_COND(((required_capacity_1) == (0))))
  {
    return (RogueByte_List*)(THIS);
  }
  #line 471
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    #line 472
    if (ROGUE_COND(((required_capacity_1) == (1))))
    {
      required_capacity_1 = ((RogueInt32)(10));
    }
    #line 473
    THIS->data = RogueType_create_array( required_capacity_1, sizeof(RogueByte), false, 6 );
  }
  else if (ROGUE_COND(((required_capacity_1) > (THIS->data->count))))
  {
    #line 475
    RogueInt32 cap_2 = (((RogueByte_List__capacity( ROGUE_ARG(THIS) ))));
    #line 476
    if (ROGUE_COND(((required_capacity_1) < (((cap_2) + (cap_2))))))
    {
      required_capacity_1 = ((RogueInt32)(((cap_2) + (cap_2))));
    }
    #line 477
    ROGUE_DEF_LOCAL_REF(RogueArray*,new_data_3,(RogueType_create_array( required_capacity_1, sizeof(RogueByte), false, 6 )));
    #line 478
    RogueArray_set(new_data_3,0,((RogueArray*)(THIS->data)),0,-1);
    #line 479
    THIS->data = new_data_3;
  }
  #line 482
  return (RogueByte_List*)(THIS);
}

#line 504
RogueByte RogueByte_List__remove_at__Int32( RogueByte_List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 505
  if (ROGUE_COND(((RogueLogical)((((unsigned int)index_0) >= (unsigned int)THIS->count)))))
  {
    throw ((RogueClassOutOfBoundsError*)(RogueOutOfBoundsError___throw( ROGUE_ARG(((RogueClassOutOfBoundsError*)(RogueOutOfBoundsError__init__Int32_Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassOutOfBoundsError*,ROGUE_CREATE_OBJECT(OutOfBoundsError))), index_0, ROGUE_ARG(THIS->count) )))) )));
  }
  #line 507
  RogueByte result_1 = (THIS->data->as_bytes[index_0]);
  #line 508
  RogueArray_set(THIS->data,index_0,((RogueArray*)(THIS->data)),((index_0) + (1)),-1);
  #line 509
  RogueByte zero_value_2 = 0;
  #line 510
  THIS->count = ((THIS->count) + (-1));
  #line 511
  THIS->data->as_bytes[THIS->count] = (zero_value_2);
  #line 512
  return (RogueByte)(result_1);
}

#line 514
RogueByte RogueByte_List__remove_first( RogueByte_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 515
  return (RogueByte)(((RogueByte_List__remove_at__Int32( ROGUE_ARG(THIS), 0 ))));
}

#line 562
void RogueByte_List__set__Int32_Byte( RogueByte_List* THIS, RogueInt32 index_0, RogueByte new_value_1 )
{
  ROGUE_GC_CHECK;
  #line 563
  THIS->data->as_bytes[index_0] = (new_value_1);
}

#line 1
RogueClassGenericList* RogueGenericList__init_object( RogueClassGenericList* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueClassGenericList*)(THIS);
}

RogueString* RogueGenericList__type_name( RogueClassGenericList* THIS )
{
  return (RogueString*)(Rogue_literal_strings[30]);
}

#line 986 "../../Rogue/Programs/RogueC/Libraries/Standard/Primitives.rogue"
RogueString* RogueByte__to_String( RogueByte THIS )
{
  ROGUE_GC_CHECK;
  #line 987
  return (RogueString*)(((RogueString*)(RogueString__operatorPLUS__Byte( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), ROGUE_ARG(THIS) ))));
}

#line 18 "../../Rogue/Programs/RogueC/Libraries/Standard/Array.rogue"
RogueString* RogueArray_Byte___type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[52]);
}

#line 1
RogueString* RogueArray__type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[31]);
}

#line 3
RogueInt32 RogueArray__count( RogueArray* THIS )
{
  #line 4
  return (RogueInt32)(((RogueInt32)(THIS->count)));
}

#line 6
RogueInt32 RogueArray__element_size( RogueArray* THIS )
{
  #line 7
  return (RogueInt32)(((RogueInt32)(THIS->element_size)));
}

#line 9
RogueArray* RogueArray__set__Int32_Array_Int32_Int32( RogueArray* THIS, RogueInt32 i1_0, RogueArray* other_1, RogueInt32 other_i1_2, RogueInt32 copy_count_3 )
{
  #line 10
  return (RogueArray*)(((RogueArray*)(RogueArray_set(THIS,i1_0,other_1,other_i1_2,copy_count_3))));
}

#line 12
void RogueArray__zero__Int32_Int32( RogueArray* THIS, RogueInt32 i1_0, RogueInt32 n_1 )
{
  ROGUE_GC_CHECK;
  #line 13
  RogueInt32 size_2 = (THIS->element_size);
  #line 14
  memset( THIS->as_bytes + i1_0*size_2, 0, n_1*size_2 );

}

#line 648 "../../Rogue/Programs/RogueC/Libraries/Standard/Primitives.rogue"
RogueInt32 RogueInt32__or_smaller__Int32( RogueInt32 THIS, RogueInt32 other_0 )
{
  ROGUE_GC_CHECK;
  #line 649
  return (RogueInt32)(((((((THIS) <= (other_0))))) ? (THIS) : other_0));
}

#line 677
RogueString* RogueInt32__to_String( RogueInt32 THIS )
{
  ROGUE_GC_CHECK;
  #line 678
  return (RogueString*)(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), ROGUE_ARG(THIS) ))));
}

#line 689
RogueCharacter RogueInt32__to_digit__Logical( RogueInt32 THIS, RogueLogical base64_0 )
{
  ROGUE_GC_CHECK;
  #line 690
  if (ROGUE_COND(base64_0))
  {
    #line 691
    if (ROGUE_COND(((((THIS) >= (0))) && (((THIS) <= (25))))))
    {
      return (RogueCharacter)(((RogueCharacter)(((THIS) + (65)))));
    }
    #line 692
    if (ROGUE_COND(((((THIS) >= (26))) && (((THIS) <= (51))))))
    {
      return (RogueCharacter)(((RogueCharacter)(((((THIS) - (26))) + (97)))));
    }
    #line 693
    if (ROGUE_COND(((((THIS) >= (52))) && (((THIS) <= (61))))))
    {
      return (RogueCharacter)(((RogueCharacter)(((((THIS) - (52))) + (48)))));
    }
    #line 694
    if (ROGUE_COND(((THIS) == (62))))
    {
      return (RogueCharacter)((RogueCharacter)'+');
    }
    #line 695
    if (ROGUE_COND(((THIS) == (63))))
    {
      return (RogueCharacter)((RogueCharacter)'/');
    }
    #line 696
    return (RogueCharacter)((RogueCharacter)'=');
  }
  else
  {
    #line 698
    if (ROGUE_COND(((((THIS) >= (0))) && (((THIS) <= (9))))))
    {
      return (RogueCharacter)(((RogueCharacter)(((THIS) + (48)))));
    }
    #line 699
    if (ROGUE_COND(((((THIS) >= (10))) && (((THIS) <= (35))))))
    {
      return (RogueCharacter)(((RogueCharacter)(((((THIS) - (10))) + (65)))));
    }
    #line 700
    return (RogueCharacter)((RogueCharacter)'0');
  }
}

#line 461 "../../Rogue/Programs/RogueC/Libraries/Standard/StringBuilder.rogue"
RogueClassStringBuilderPool* RogueStringBuilderPool__init_object( RogueClassStringBuilderPool* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 463
  THIS->available = ((RogueStringBuilder_List*)(RogueStringBuilder_List__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder_List*,ROGUE_CREATE_OBJECT(StringBuilder_List))) )));
  #line 461
  return (RogueClassStringBuilderPool*)(THIS);
}

RogueString* RogueStringBuilderPool__type_name( RogueClassStringBuilderPool* THIS )
{
  return (RogueString*)(Rogue_literal_strings[35]);
}

#line 4 "../../Rogue/Programs/RogueC/Libraries/Standard/List.rogue"
RogueStringBuilder_List* RogueStringBuilder_List__init_object( RogueStringBuilder_List* THIS )
{
  ROGUE_GC_CHECK;
  RogueGenericList__init_object( ROGUE_ARG(((RogueClassGenericList*)THIS)) );
  return (RogueStringBuilder_List*)(THIS);
}

#line 10
RogueStringBuilder_List* RogueStringBuilder_List__init( RogueStringBuilder_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 11
  RogueStringBuilder_List__init__Int32( ROGUE_ARG(THIS), 0 );
  #line 13
  return (RogueStringBuilder_List*)(THIS);
}

#line 633
RogueString* RogueStringBuilder_List__to_String( RogueStringBuilder_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 634
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 635
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)'[', true );
  #line 636
  RogueLogical first_1 = (true);
  #line 637
  {
    ROGUE_DEF_LOCAL_REF(RogueStringBuilder_List*,_auto_762_0,(THIS));
    RogueInt32 _auto_763_0 = (0);
    RogueInt32 _auto_764_0 = (((_auto_762_0->count) - (1)));
    for (;ROGUE_COND(((_auto_763_0) <= (_auto_764_0)));++_auto_763_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,value_0,(((RogueStringBuilder*)(_auto_762_0->data->as_objects[_auto_763_0]))));
      #line 638
      if (ROGUE_COND(first_1))
      {
        first_1 = ((RogueLogical)(false));
      }
      else
      {
        #line 639
        RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)',', true );
      }
      #line 640
      if (ROGUE_COND(((void*)value_0) == ((void*)NULL)))
      {
        RogueStringBuilder__print__String( buffer_0, Rogue_literal_strings[1] );
      }
      else
      {
        #line 641
        RogueStringBuilder__print__String( buffer_0, ROGUE_ARG(((RogueString*)(RogueStringBuilder__to_String( value_0 )))) );
      }
    }
  }
  #line 643
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)']', true );
  #line 644
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_0 ))));
}

#line 4
RogueString* RogueStringBuilder_List__type_name( RogueStringBuilder_List* THIS )
{
  return (RogueString*)(Rogue_literal_strings[47]);
}

#line 13
RogueStringBuilder_List* RogueStringBuilder_List__init__Int32( RogueStringBuilder_List* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 14
  if (ROGUE_COND(((initial_capacity_0) != (((RogueStringBuilder_List__capacity( ROGUE_ARG(THIS) )))))))
  {
    THIS->data = RogueType_create_array( initial_capacity_0, sizeof(RogueStringBuilder*), true, 3 );
  }
  #line 15
  RogueStringBuilder_List__clear( ROGUE_ARG(THIS) );
  #line 17
  return (RogueStringBuilder_List*)(THIS);
}

#line 68
RogueInt32 RogueStringBuilder_List__capacity( RogueStringBuilder_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 69
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    return (RogueInt32)(0);
  }
  #line 70
  return (RogueInt32)(THIS->data->count);
}

#line 72
RogueStringBuilder_List* RogueStringBuilder_List__clear( RogueStringBuilder_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 73
  RogueStringBuilder_List__discard_from__Int32( ROGUE_ARG(THIS), 0 );
  #line 74
  return (RogueStringBuilder_List*)(THIS);
}

#line 174
RogueStringBuilder_List* RogueStringBuilder_List__discard_from__Int32( RogueStringBuilder_List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 175
  if (ROGUE_COND(!!(THIS->data)))
  {
    #line 176
    RogueInt32 n_1 = (((THIS->count) - (index_0)));
    #line 177
    if (ROGUE_COND(((n_1) > (0))))
    {
      #line 178
      RogueArray__zero__Int32_Int32( ROGUE_ARG(((RogueArray*)THIS->data)), index_0, n_1 );
      #line 179
      THIS->count = index_0;
    }
  }
  #line 182
  return (RogueStringBuilder_List*)(THIS);
}

#line 18 "../../Rogue/Programs/RogueC/Libraries/Standard/Array.rogue"
RogueString* RogueArray_StringBuilder___type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[54]);
}

#line 4 "../../Rogue/Programs/RogueC/Libraries/Standard/List.rogue"
Rogue_Function____List* Rogue_Function____List__init_object( Rogue_Function____List* THIS )
{
  ROGUE_GC_CHECK;
  RogueGenericList__init_object( ROGUE_ARG(((RogueClassGenericList*)THIS)) );
  return (Rogue_Function____List*)(THIS);
}

#line 10
Rogue_Function____List* Rogue_Function____List__init( Rogue_Function____List* THIS )
{
  ROGUE_GC_CHECK;
  #line 11
  Rogue_Function____List__init__Int32( ROGUE_ARG(THIS), 0 );
  #line 13
  return (Rogue_Function____List*)(THIS);
}

#line 633
RogueString* Rogue_Function____List__to_String( Rogue_Function____List* THIS )
{
  ROGUE_GC_CHECK;
  #line 634
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 635
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)'[', true );
  #line 636
  RogueLogical first_1 = (true);
  #line 637
  {
    ROGUE_DEF_LOCAL_REF(Rogue_Function____List*,_auto_840_0,(THIS));
    RogueInt32 _auto_841_0 = (0);
    RogueInt32 _auto_842_0 = (((_auto_840_0->count) - (1)));
    for (;ROGUE_COND(((_auto_841_0) <= (_auto_842_0)));++_auto_841_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueClass_Function___*,value_0,(((RogueClass_Function___*)(_auto_840_0->data->as_objects[_auto_841_0]))));
      #line 638
      if (ROGUE_COND(first_1))
      {
        first_1 = ((RogueLogical)(false));
      }
      else
      {
        #line 639
        RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)',', true );
      }
      #line 640
      if (ROGUE_COND(((void*)value_0) == ((void*)NULL)))
      {
        RogueStringBuilder__print__String( buffer_0, Rogue_literal_strings[1] );
      }
      else
      {
        #line 641
        RogueStringBuilder__print__String( buffer_0, ROGUE_ARG(((RogueString*)Rogue_call_ROGUEM0( 6, ((RogueObject*)value_0) ))) );
      }
    }
  }
  #line 643
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)']', true );
  #line 644
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_0 ))));
}

#line 4
RogueString* Rogue_Function____List__type_name( Rogue_Function____List* THIS )
{
  return (RogueString*)(Rogue_literal_strings[48]);
}

#line 13
Rogue_Function____List* Rogue_Function____List__init__Int32( Rogue_Function____List* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 14
  if (ROGUE_COND(((initial_capacity_0) != (((Rogue_Function____List__capacity( ROGUE_ARG(THIS) )))))))
  {
    THIS->data = RogueType_create_array( initial_capacity_0, sizeof(RogueClass_Function___*), true, 15 );
  }
  #line 15
  Rogue_Function____List__clear( ROGUE_ARG(THIS) );
  #line 17
  return (Rogue_Function____List*)(THIS);
}

#line 58
Rogue_Function____List* Rogue_Function____List__add___Function___( Rogue_Function____List* THIS, RogueClass_Function___* value_0 )
{
  ROGUE_GC_CHECK;
  #line 59
  Rogue_Function____List__set__Int32__Function___( ROGUE_ARG(((Rogue_Function____List*)(Rogue_Function____List__reserve__Int32( ROGUE_ARG(THIS), 1 )))), ROGUE_ARG(THIS->count), value_0 );
  #line 60
  THIS->count = ((THIS->count) + (1));
  #line 61
  return (Rogue_Function____List*)(THIS);
}

#line 68
RogueInt32 Rogue_Function____List__capacity( Rogue_Function____List* THIS )
{
  ROGUE_GC_CHECK;
  #line 69
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    return (RogueInt32)(0);
  }
  #line 70
  return (RogueInt32)(THIS->data->count);
}

#line 72
Rogue_Function____List* Rogue_Function____List__clear( Rogue_Function____List* THIS )
{
  ROGUE_GC_CHECK;
  #line 73
  Rogue_Function____List__discard_from__Int32( ROGUE_ARG(THIS), 0 );
  #line 74
  return (Rogue_Function____List*)(THIS);
}

#line 174
Rogue_Function____List* Rogue_Function____List__discard_from__Int32( Rogue_Function____List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 175
  if (ROGUE_COND(!!(THIS->data)))
  {
    #line 176
    RogueInt32 n_1 = (((THIS->count) - (index_0)));
    #line 177
    if (ROGUE_COND(((n_1) > (0))))
    {
      #line 178
      RogueArray__zero__Int32_Int32( ROGUE_ARG(((RogueArray*)THIS->data)), index_0, n_1 );
      #line 179
      THIS->count = index_0;
    }
  }
  #line 182
  return (Rogue_Function____List*)(THIS);
}

#line 467
Rogue_Function____List* Rogue_Function____List__reserve__Int32( Rogue_Function____List* THIS, RogueInt32 additional_elements_0 )
{
  ROGUE_GC_CHECK;
  #line 468
  RogueInt32 required_capacity_1 = (((THIS->count) + (additional_elements_0)));
  #line 469
  if (ROGUE_COND(((required_capacity_1) == (0))))
  {
    return (Rogue_Function____List*)(THIS);
  }
  #line 471
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    #line 472
    if (ROGUE_COND(((required_capacity_1) == (1))))
    {
      required_capacity_1 = ((RogueInt32)(10));
    }
    #line 473
    THIS->data = RogueType_create_array( required_capacity_1, sizeof(RogueClass_Function___*), true, 15 );
  }
  else if (ROGUE_COND(((required_capacity_1) > (THIS->data->count))))
  {
    #line 475
    RogueInt32 cap_2 = (((Rogue_Function____List__capacity( ROGUE_ARG(THIS) ))));
    #line 476
    if (ROGUE_COND(((required_capacity_1) < (((cap_2) + (cap_2))))))
    {
      required_capacity_1 = ((RogueInt32)(((cap_2) + (cap_2))));
    }
    #line 477
    ROGUE_DEF_LOCAL_REF(RogueArray*,new_data_3,(RogueType_create_array( required_capacity_1, sizeof(RogueClass_Function___*), true, 15 )));
    #line 478
    RogueArray_set(new_data_3,0,((RogueArray*)(THIS->data)),0,-1);
    #line 479
    THIS->data = new_data_3;
  }
  #line 482
  return (Rogue_Function____List*)(THIS);
}

#line 562
void Rogue_Function____List__set__Int32__Function___( Rogue_Function____List* THIS, RogueInt32 index_0, RogueClass_Function___* new_value_1 )
{
  ROGUE_GC_CHECK;
  #line 563
  THIS->data->as_objects[index_0] = new_value_1;
}

#line 33 "../../Rogue/Programs/RogueC/Libraries/Standard/Task.rogue"
RogueClass_Function___* Rogue_Function_____init_object( RogueClass_Function___* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueClass_Function___*)(THIS);
}

RogueString* Rogue_Function_____type_name( RogueClass_Function___* THIS )
{
  return (RogueString*)(Rogue_literal_strings[36]);
}

void Rogue_Function_____call( RogueClass_Function___* THIS )
{
}

#line 18 "../../Rogue/Programs/RogueC/Libraries/Standard/Array.rogue"
RogueString* RogueArray__Function______type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[53]);
}

#line 963 "../../Rogue/Programs/RogueC/Libraries/Standard/String.rogue"
RogueString* RogueString__to_String( RogueString* THIS )
{
  ROGUE_GC_CHECK;
  #line 964
  return (RogueString*)(THIS);
}

#line 1
RogueString* RogueString__type_name( RogueString* THIS )
{
  return (RogueString*)(Rogue_literal_strings[28]);
}

#line 74
RogueString* RogueString__after__Int32( RogueString* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 79
  return (RogueString*)(((RogueString*)(RogueString__from__Int32( ROGUE_ARG(THIS), ROGUE_ARG(((index_0) + (1))) ))));
}

#line 101
RogueString* RogueString__after_last__Character_Logical( RogueString* THIS, RogueCharacter ch_0, RogueLogical ignore_case_1 )
{
  ROGUE_GC_CHECK;
  #line 102
  RogueOptionalInt32 i_2 = (((RogueString__locate_last__Character_OptionalInt32_Logical( ROGUE_ARG(THIS), ch_0, (RogueOptionalInt32__create()), ignore_case_1 ))));
  #line 103
  if (ROGUE_COND(i_2.exists))
  {
    return (RogueString*)(((RogueString*)(RogueString__from__Int32( ROGUE_ARG(THIS), ROGUE_ARG(((i_2.value) + (1))) ))));
  }
  else
  {
    #line 104
    return (RogueString*)(Rogue_literal_strings[0]);
  }
}

#line 119
RogueString* RogueString__before__Int32( RogueString* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 124
  return (RogueString*)(((RogueString*)(RogueString__from__Int32_Int32( ROGUE_ARG(THIS), 0, ROGUE_ARG(((index_0) - (1))) ))));
}

#line 126
RogueString* RogueString__before_first__Character_Logical( RogueString* THIS, RogueCharacter ch_0, RogueLogical ignore_case_1 )
{
  ROGUE_GC_CHECK;
  #line 127
  RogueOptionalInt32 i_2 = (((RogueString__locate__Character_OptionalInt32_Logical( ROGUE_ARG(THIS), ch_0, (RogueOptionalInt32__create()), ignore_case_1 ))));
  #line 128
  if (ROGUE_COND(i_2.exists))
  {
    return (RogueString*)(((RogueString*)(RogueString__from__Int32_Int32( ROGUE_ARG(THIS), 0, ROGUE_ARG(((i_2.value) - (1))) ))));
  }
  else
  {
    #line 129
    return (RogueString*)(THIS);
  }
}

#line 164
RogueByte RogueString__byte__Int32( RogueString* THIS, RogueInt32 byte_index_0 )
{
  ROGUE_GC_CHECK;
  #line 165
  return (RogueByte)(((RogueByte)(THIS->utf8[ byte_index_0 ])));
}

#line 167
RogueInt32 RogueString__byte_count( RogueString* THIS )
{
  ROGUE_GC_CHECK;
  #line 168
  return (RogueInt32)(((RogueInt32)(THIS->byte_count)));
}

#line 190
RogueLogical RogueString__contains__Character_Logical( RogueString* THIS, RogueCharacter ch_0, RogueLogical ignore_case_1 )
{
  ROGUE_GC_CHECK;
  #line 191
  return (RogueLogical)(((RogueString__locate__Character_OptionalInt32_Logical( ROGUE_ARG(THIS), ch_0, (RogueOptionalInt32__create()), ignore_case_1 ))).exists);
}

#line 196
RogueLogical RogueString__contains_at__String_Int32_Logical( RogueString* THIS, RogueString* substring_0, RogueInt32 at_index_1, RogueLogical ignore_case_2 )
{
  ROGUE_GC_CHECK;
  #line 197
  if (ROGUE_COND(((at_index_1) < (0))))
  {
    return (RogueLogical)(false);
  }
  #line 199
  if (ROGUE_COND(ignore_case_2))
  {
    #line 200
    if (ROGUE_COND(((((at_index_1) + (((RogueString__count( substring_0 )))))) > (((RogueString__count( ROGUE_ARG(THIS) )))))))
    {
      return (RogueLogical)(false);
    }
    #line 201
    {
      ROGUE_DEF_LOCAL_REF(RogueString*,_auto_409_0,(substring_0));
      RogueInt32 _auto_410_0 = (0);
      RogueInt32 _auto_411_0 = (((((RogueString__count( _auto_409_0 )))) - (1)));
      for (;ROGUE_COND(((_auto_410_0) <= (_auto_411_0)));++_auto_410_0)
      {
        ROGUE_GC_CHECK;
        RogueCharacter other_ch_0 = (((RogueString__get__Int32( _auto_409_0, _auto_410_0 ))));
        #line 202
        if (ROGUE_COND(((((RogueCharacter__to_lowercase( other_ch_0 )))) != (((RogueCharacter__to_lowercase( ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), at_index_1 )))) )))))))
        {
          return (RogueLogical)(false);
        }
        #line 203
        ++at_index_1;
      }
    }
    #line 205
    return (RogueLogical)(true);
  }
  else
  {
    #line 208
    RogueInt32 offset = RogueString_set_cursor( THIS, at_index_1 );
    RogueInt32 other_count = substring_0->byte_count;
    if (offset + other_count > THIS->byte_count) return false;

    return (0 == memcmp(THIS->utf8 + offset, substring_0->utf8, other_count));

  }
}

#line 217
RogueInt32 RogueString__count( RogueString* THIS )
{
  ROGUE_GC_CHECK;
  #line 218
  return (RogueInt32)(((RogueInt32)(THIS->character_count)));
}

#line 242
RogueLogical RogueString__ends_with__Character_Logical( RogueString* THIS, RogueCharacter ch_0, RogueLogical ignore_case_1 )
{
  ROGUE_GC_CHECK;
  #line 243
  if (ROGUE_COND(ignore_case_1))
  {
    #line 244
    return (RogueLogical)(((((((RogueString__count( ROGUE_ARG(THIS) )))) > (0))) && (((((RogueCharacter__to_lowercase( ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), ROGUE_ARG(((((RogueString__count( ROGUE_ARG(THIS) )))) - (1))) )))) )))) == (((RogueCharacter__to_lowercase( ch_0 ))))))));
  }
  else
  {
    #line 246
    return (RogueLogical)(((((((RogueString__count( ROGUE_ARG(THIS) )))) > (0))) && (((((RogueString__get__Int32( ROGUE_ARG(THIS), ROGUE_ARG(((((RogueString__count( ROGUE_ARG(THIS) )))) - (1))) )))) == (ch_0)))));
  }
}

#line 249
RogueLogical RogueString__ends_with__String_Logical( RogueString* THIS, RogueString* other_0, RogueLogical ignore_case_1 )
{
  ROGUE_GC_CHECK;
  #line 250
  RogueInt32 other_count_2 = (((RogueString__count( other_0 ))));
  #line 251
  return (RogueLogical)(((((((((RogueString__count( ROGUE_ARG(THIS) )))) >= (other_count_2))) && (((other_count_2) > (0))))) && (((RogueString__contains_at__String_Int32_Logical( ROGUE_ARG(THIS), other_0, ROGUE_ARG(((((RogueString__count( ROGUE_ARG(THIS) )))) - (other_count_2))), ignore_case_1 ))))));
}

#line 409
RogueString* RogueString__from__Int32( RogueString* THIS, RogueInt32 i1_0 )
{
  ROGUE_GC_CHECK;
  #line 410
  return (RogueString*)(((RogueString*)(RogueString__from__Int32_Int32( ROGUE_ARG(THIS), i1_0, ROGUE_ARG(((((RogueString__count( ROGUE_ARG(THIS) )))) - (1))) ))));
}

#line 412
RogueString* RogueString__from__Int32_Int32( RogueString* THIS, RogueInt32 i1_0, RogueInt32 i2_1 )
{
  ROGUE_GC_CHECK;
  #line 413
  if (ROGUE_COND(((i1_0) < (0))))
  {
    i1_0 = ((RogueInt32)(0));
  }
  else if (ROGUE_COND(((i2_1) >= (((RogueString__count( ROGUE_ARG(THIS) )))))))
  {
    #line 414
    i2_1 = ((RogueInt32)(((((RogueString__count( ROGUE_ARG(THIS) )))) - (1))));
  }
  #line 416
  if (ROGUE_COND(((i1_0) > (i2_1))))
  {
    return (RogueString*)(Rogue_literal_strings[0]);
  }
  #line 418
  if (ROGUE_COND(((i1_0) == (i2_1))))
  {
    return (RogueString*)(((RogueString*)(RogueString__operatorPLUS__Character( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), i1_0 )))) ))));
  }
  #line 421
  RogueInt32 byte_i1 = RogueString_set_cursor( THIS, i1_0 );
  RogueInt32 byte_limit = RogueString_set_cursor( THIS, i2_1+1 );
  int new_count = (byte_limit - byte_i1);
  RogueString* result = RogueString_create_with_byte_count( new_count );
  memcpy( result->utf8, THIS->utf8+byte_i1, new_count );
  return RogueString_validate( result );

}

#line 429
RogueString* RogueString__from_first__Character_Logical( RogueString* THIS, RogueCharacter ch_0, RogueLogical ignore_case_1 )
{
  ROGUE_GC_CHECK;
  #line 430
  RogueOptionalInt32 i_2 = (((RogueString__locate__Character_OptionalInt32_Logical( ROGUE_ARG(THIS), ch_0, (RogueOptionalInt32__create()), ignore_case_1 ))));
  #line 431
  if (ROGUE_COND(!(i_2.exists)))
  {
    return (RogueString*)(Rogue_literal_strings[0]);
  }
  #line 432
  return (RogueString*)(((RogueString*)(RogueString__from__Int32( ROGUE_ARG(THIS), ROGUE_ARG(i_2.value) ))));
}

#line 449
RogueCharacter RogueString__get__Int32( RogueString* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 450
  return (RogueCharacter)(((RogueCharacter)(RogueString_character_at(THIS,index_0))));
}

#line 513
RogueCharacter RogueString__last( RogueString* THIS )
{
  ROGUE_GC_CHECK;
  #line 514
  return (RogueCharacter)(((RogueString__get__Int32( ROGUE_ARG(THIS), ROGUE_ARG(((((RogueString__count( ROGUE_ARG(THIS) )))) - (1))) ))));
}

#line 516
RogueString* RogueString__left_justified__Int32_Character( RogueString* THIS, RogueInt32 spaces_0, RogueCharacter fill_1 )
{
  ROGUE_GC_CHECK;
  #line 517
  if (ROGUE_COND(((((RogueString__count( ROGUE_ARG(THIS) )))) >= (spaces_0))))
  {
    return (RogueString*)(THIS);
  }
  #line 519
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_2,(((RogueStringBuilder*)(RogueStringBuilder__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))), spaces_0 )))));
  #line 520
  RogueStringBuilder__print__String( buffer_2, ROGUE_ARG(THIS) );
  #line 521
  {
    RogueInt32 _auto_142_3 = (((RogueString__count( ROGUE_ARG(THIS) ))));
    RogueInt32 _auto_143_4 = (spaces_0);
    for (;ROGUE_COND(((_auto_142_3) < (_auto_143_4)));++_auto_142_3)
    {
      ROGUE_GC_CHECK;
      RogueStringBuilder__print__Character_Logical( buffer_2, fill_1, true );
    }
  }
  #line 523
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_2 ))));
}

#line 538
RogueOptionalInt32 RogueString__locate__Character_OptionalInt32_Logical( RogueString* THIS, RogueCharacter ch_0, RogueOptionalInt32 optional_i1_1, RogueLogical ignore_case_2 )
{
  ROGUE_GC_CHECK;
  #line 539
  RogueInt32 i_3 = (0);
  #line 540
  RogueInt32 limit_4 = (((RogueString__count( ROGUE_ARG(THIS) ))));
  #line 541
  if (ROGUE_COND(optional_i1_1.exists))
  {
    i_3 = ((RogueInt32)(optional_i1_1.value));
  }
  #line 543
  if (ROGUE_COND(ignore_case_2))
  {
    #line 544
    while (ROGUE_COND(((i_3) < (limit_4))))
    {
      ROGUE_GC_CHECK;
      #line 545
      if (ROGUE_COND(((((RogueCharacter__to_lowercase( ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), i_3 )))) )))) == (((RogueCharacter__to_lowercase( ch_0 )))))))
      {
        return (RogueOptionalInt32)(RogueOptionalInt32( i_3, true ));
      }
      #line 546
      ++i_3;
    }
  }
  else
  {
    #line 549
    while (ROGUE_COND(((i_3) < (limit_4))))
    {
      ROGUE_GC_CHECK;
      #line 550
      if (ROGUE_COND(((((RogueString__get__Int32( ROGUE_ARG(THIS), i_3 )))) == (ch_0))))
      {
        return (RogueOptionalInt32)(RogueOptionalInt32( i_3, true ));
      }
      #line 551
      ++i_3;
    }
  }
  #line 554
  return (RogueOptionalInt32)((RogueOptionalInt32__create()));
}

#line 556
RogueOptionalInt32 RogueString__locate__String_OptionalInt32_Logical( RogueString* THIS, RogueString* other_0, RogueOptionalInt32 optional_i1_1, RogueLogical ignore_case_2 )
{
  ROGUE_GC_CHECK;
  #line 557
  RogueInt32 other_count_3 = (((RogueString__count( other_0 ))));
  #line 558
  if (ROGUE_COND(((other_count_3) == (1))))
  {
    return (RogueOptionalInt32)(((RogueString__locate__Character_OptionalInt32_Logical( ROGUE_ARG(THIS), ROGUE_ARG(((RogueString__get__Int32( other_0, 0 )))), optional_i1_1, false ))));
  }
  #line 560
  RogueInt32 this_limit_4 = (((((((RogueString__count( ROGUE_ARG(THIS) )))) - (other_count_3))) + (1)));
  #line 561
  if (ROGUE_COND(((((other_count_3) == (0))) || (((this_limit_4) <= (0))))))
  {
    return (RogueOptionalInt32)((RogueOptionalInt32__create()));
  }
  #line 563
  RogueInt32 i1_5 = 0;
  #line 564
  if (ROGUE_COND(optional_i1_1.exists))
  {
    #line 565
    i1_5 = ((RogueInt32)(((optional_i1_1.value) - (1))));
    #line 566
    if (ROGUE_COND(((i1_5) < (-1))))
    {
      i1_5 = ((RogueInt32)(-1));
    }
  }
  else
  {
    #line 568
    i1_5 = ((RogueInt32)(-1));
  }
  #line 571
  while (ROGUE_COND(((((RogueInt32)(++i1_5
  ))) < (this_limit_4))))
  {
    ROGUE_GC_CHECK;
    #line 572
    if (ROGUE_COND(((RogueString__contains_at__String_Int32_Logical( ROGUE_ARG(THIS), other_0, i1_5, ignore_case_2 )))))
    {
      return (RogueOptionalInt32)(RogueOptionalInt32( i1_5, true ));
    }
  }
  #line 575
  return (RogueOptionalInt32)((RogueOptionalInt32__create()));
}

#line 577
RogueOptionalInt32 RogueString__locate_last__Character_OptionalInt32_Logical( RogueString* THIS, RogueCharacter ch_0, RogueOptionalInt32 starting_index_1, RogueLogical ignore_case_2 )
{
  ROGUE_GC_CHECK;
  #line 578
  RogueInt32 i_3 = (((((RogueString__count( ROGUE_ARG(THIS) )))) - (1)));
  #line 579
  if (ROGUE_COND(starting_index_1.exists))
  {
    i_3 = ((RogueInt32)(starting_index_1.value));
  }
  #line 581
  if (ROGUE_COND(ignore_case_2))
  {
    #line 582
    while (ROGUE_COND(((i_3) >= (0))))
    {
      ROGUE_GC_CHECK;
      #line 583
      if (ROGUE_COND(((((RogueCharacter__to_lowercase( ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), i_3 )))) )))) == (((RogueCharacter__to_lowercase( ch_0 )))))))
      {
        return (RogueOptionalInt32)(RogueOptionalInt32( i_3, true ));
      }
      #line 584
      --i_3;
    }
  }
  else
  {
    #line 587
    while (ROGUE_COND(((i_3) >= (0))))
    {
      ROGUE_GC_CHECK;
      #line 588
      if (ROGUE_COND(((((RogueString__get__Int32( ROGUE_ARG(THIS), i_3 )))) == (ch_0))))
      {
        return (RogueOptionalInt32)(RogueOptionalInt32( i_3, true ));
      }
      #line 589
      --i_3;
    }
  }
  #line 592
  return (RogueOptionalInt32)((RogueOptionalInt32__create()));
}

#line 615
RogueString* RogueString__operatorPLUS__Byte( RogueString* THIS, RogueByte value_0 )
{
  ROGUE_GC_CHECK;
  #line 616
  return (RogueString*)(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(THIS) ))), ROGUE_ARG(((RogueInt32)(value_0))) ))));
}

#line 618
RogueString* RogueString__operatorPLUS__Character( RogueString* THIS, RogueCharacter value_0 )
{
  ROGUE_GC_CHECK;
  #line 619
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__Character_Logical( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), ROGUE_ARG(THIS) )))), value_0, true )))) ))));
}

#line 621
RogueString* RogueString__operatorPLUS__Int32( RogueString* THIS, RogueInt32 value_0 )
{
  ROGUE_GC_CHECK;
  #line 622
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__Int32( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), ROGUE_ARG(THIS) )))), value_0 )))) ))));
}

#line 681
RogueString* RogueString__operatorPLUS__String( RogueString* THIS, RogueString* value_0 )
{
  ROGUE_GC_CHECK;
  #line 682
  if (ROGUE_COND(((void*)value_0) == ((void*)NULL)))
  {
    return (RogueString*)((RogueString__operatorPLUS__String_String( ROGUE_ARG(THIS), Rogue_literal_strings[1] )));
  }
  #line 683
  if (ROGUE_COND(((((RogueString__count( ROGUE_ARG(THIS) )))) == (0))))
  {
    return (RogueString*)(value_0);
  }
  #line 684
  if (ROGUE_COND(((((RogueString__count( value_0 )))) == (0))))
  {
    return (RogueString*)(THIS);
  }
  #line 685
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), ROGUE_ARG(THIS) )))), value_0 )))) ))));
}

#line 687
RogueString* RogueString__operatorTIMES__Int32( RogueString* THIS, RogueInt32 value_0 )
{
  ROGUE_GC_CHECK;
  #line 688
  return (RogueString*)(((RogueString*)(RogueString__times__Int32( ROGUE_ARG(THIS), value_0 ))));
}

#line 690
RogueString* RogueString__pluralized__Int32( RogueString* THIS, RogueInt32 quantity_0 )
{
  ROGUE_GC_CHECK;
  #line 716
  ROGUE_DEF_LOCAL_REF(RogueString*,st_1,(((RogueString*)(RogueString__replacing__String_String_Logical( ROGUE_ARG(THIS), Rogue_literal_strings[21], ROGUE_ARG(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), quantity_0 )))), false )))));
  #line 718
  if (ROGUE_COND(((RogueString__contains__Character_Logical( st_1, (RogueCharacter)'/', false )))))
  {
    #line 719
    if (ROGUE_COND(((quantity_0) == (1))))
    {
      return (RogueString*)(((RogueString*)(RogueString__before_first__Character_Logical( st_1, (RogueCharacter)'/', false ))));
    }
    else
    {
      #line 720
      return (RogueString*)(((RogueString*)(RogueString__after_last__Character_Logical( st_1, (RogueCharacter)'/', false ))));
    }
  }
  else
  {
    #line 723
    RogueOptionalInt32 alt1_2 = (((RogueString__locate__Character_OptionalInt32_Logical( st_1, (RogueCharacter)'(', (RogueOptionalInt32__create()), false ))));
    #line 724
    if (ROGUE_COND(alt1_2.exists))
    {
      #line 725
      RogueOptionalInt32 alt2_3 = (((RogueString__locate__Character_OptionalInt32_Logical( st_1, (RogueCharacter)')', RogueOptionalInt32( ((alt1_2.value) + (1)), true ), false ))));
      #line 726
      if (ROGUE_COND(!(alt2_3.exists)))
      {
        return (RogueString*)(THIS);
      }
      #line 728
      if (ROGUE_COND(((quantity_0) == (1))))
      {
        #line 729
        return (RogueString*)((RogueString__operatorPLUS__String_String( ROGUE_ARG(((RogueString*)(RogueString__before__Int32( st_1, ROGUE_ARG(alt1_2.value) )))), ROGUE_ARG(((RogueString*)(RogueString__after__Int32( st_1, ROGUE_ARG(alt2_3.value) )))) )));
      }
      #line 733
      return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__before__Int32( st_1, ROGUE_ARG(alt1_2.value) )))) ))) )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__from__Int32_Int32( st_1, ROGUE_ARG(((alt1_2.value) + (1))), ROGUE_ARG(((alt2_3.value) - (1))) )))) ))) )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__after__Int32( st_1, ROGUE_ARG(alt2_3.value) )))) ))) )))) ))));
    }
    else
    {
      #line 735
      if (ROGUE_COND(((quantity_0) == (1))))
      {
        return (RogueString*)(st_1);
      }
      #line 738
      RogueInt32 index_4 = (0);
      #line 739
      RogueInt32 i_5 = (((RogueString__count( st_1 ))));
      #line 740
      while (ROGUE_COND(((i_5) > (0))))
      {
        ROGUE_GC_CHECK;
        #line 741
        --i_5;
        #line 742
        if (ROGUE_COND(((RogueCharacter__is_letter( ROGUE_ARG(((RogueString__get__Int32( st_1, i_5 )))) )))))
        {
          index_4 = ((RogueInt32)(i_5));
          goto _auto_446;
        }
      }
      _auto_446:;
      #line 745
      if (ROGUE_COND(((((RogueString__get__Int32( st_1, index_4 )))) == ((RogueCharacter)'s'))))
      {
        return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__before__Int32( st_1, ROGUE_ARG(((index_4) + (1))) )))) ))) )))), Rogue_literal_strings[22] )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__after__Int32( st_1, index_4 )))) ))) )))) ))));
      }
      else
      {
        #line 746
        return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__before__Int32( st_1, ROGUE_ARG(((index_4) + (1))) )))) ))) )))), Rogue_literal_strings[23] )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__after__Int32( st_1, index_4 )))) ))) )))) ))));
      }
    }
  }
}

#line 795
RogueString* RogueString__replacing__String_String_Logical( RogueString* THIS, RogueString* look_for_0, RogueString* replace_with_1, RogueLogical ignore_case_2 )
{
  ROGUE_GC_CHECK;
  #line 798
  RogueOptionalInt32 i1_3 = (((RogueString__locate__String_OptionalInt32_Logical( ROGUE_ARG(THIS), look_for_0, (RogueOptionalInt32__create()), ignore_case_2 ))));
  #line 799
  if (ROGUE_COND(!(i1_3.exists)))
  {
    return (RogueString*)(THIS);
  }
  #line 801
  RogueInt32 i0_4 = (0);
  #line 803
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_5,(((RogueStringBuilder*)(RogueStringBuilder__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))), ROGUE_ARG(((RogueInt32)(((((RogueReal64)(((RogueString__count( ROGUE_ARG(THIS) )))))) * (1.1))))) )))));
  #line 804
  while (ROGUE_COND(i1_3.exists))
  {
    ROGUE_GC_CHECK;
    #line 805
    {
      RogueInt32 i_6 = (i0_4);
      RogueInt32 _auto_145_7 = (i1_3.value);
      for (;ROGUE_COND(((i_6) < (_auto_145_7)));++i_6)
      {
        ROGUE_GC_CHECK;
        RogueStringBuilder__print__Character_Logical( buffer_5, ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), i_6 )))), true );
      }
    }
    #line 806
    RogueStringBuilder__print__String( buffer_5, replace_with_1 );
    #line 807
    i0_4 = ((RogueInt32)(((i1_3.value) + (((RogueString__count( look_for_0 )))))));
    #line 808
    i1_3 = ((RogueOptionalInt32)(((RogueString__locate__String_OptionalInt32_Logical( ROGUE_ARG(THIS), look_for_0, RogueOptionalInt32( i0_4, true ), ignore_case_2 )))));
  }
  #line 810
  {
    RogueInt32 i_8 = (i0_4);
    RogueInt32 _auto_146_9 = (((RogueString__count( ROGUE_ARG(THIS) ))));
    for (;ROGUE_COND(((i_8) < (_auto_146_9)));++i_8)
    {
      ROGUE_GC_CHECK;
      RogueStringBuilder__print__Character_Logical( buffer_5, ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), i_8 )))), true );
    }
  }
  #line 811
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_5 ))));
}

#line 868
RogueString_List* RogueString__split__Character_Logical( RogueString* THIS, RogueCharacter separator_0, RogueLogical ignore_case_1 )
{
  ROGUE_GC_CHECK;
  #line 869
  ROGUE_DEF_LOCAL_REF(RogueString_List*,result_2,(((RogueString_List*)(RogueString_List__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueString_List*,ROGUE_CREATE_OBJECT(String_List))) )))));
  #line 871
  RogueInt32 i1_3 = (0);
  #line 872
  RogueOptionalInt32 i2_4 = (((RogueString__locate__Character_OptionalInt32_Logical( ROGUE_ARG(THIS), separator_0, RogueOptionalInt32( i1_3, true ), ignore_case_1 ))));
  #line 873
  while (ROGUE_COND(i2_4.exists))
  {
    ROGUE_GC_CHECK;
    #line 874
    RogueString_List__add__String( result_2, ROGUE_ARG(((RogueString*)(RogueString__from__Int32_Int32( ROGUE_ARG(THIS), i1_3, ROGUE_ARG(((i2_4.value) - (1))) )))) );
    #line 875
    i1_3 = ((RogueInt32)(((i2_4.value) + (1))));
    #line 876
    i2_4 = ((RogueOptionalInt32)(((RogueString__locate__Character_OptionalInt32_Logical( ROGUE_ARG(THIS), separator_0, RogueOptionalInt32( i1_3, true ), ignore_case_1 )))));
  }
  #line 879
  RogueString_List__add__String( result_2, ROGUE_ARG(((RogueString*)(RogueString__from__Int32( ROGUE_ARG(THIS), i1_3 )))) );
  #line 881
  return (RogueString_List*)(result_2);
}

#line 929
RogueString* RogueString__times__Int32( RogueString* THIS, RogueInt32 n_0 )
{
  ROGUE_GC_CHECK;
  #line 930
  if (ROGUE_COND(((n_0) <= (0))))
  {
    return (RogueString*)(Rogue_literal_strings[0]);
  }
  #line 931
  if (ROGUE_COND(((n_0) == (1))))
  {
    return (RogueString*)(THIS);
  }
  #line 933
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,builder_1,(((RogueStringBuilder*)(RogueStringBuilder__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))), ROGUE_ARG(((((RogueString__count( ROGUE_ARG(THIS) )))) * (n_0))) )))));
  #line 934
  {
    RogueInt32 _auto_152_2 = (1);
    RogueInt32 _auto_153_3 = (n_0);
    for (;ROGUE_COND(((_auto_152_2) <= (_auto_153_3)));++_auto_152_2)
    {
      ROGUE_GC_CHECK;
      RogueStringBuilder__print__String( builder_1, ROGUE_ARG(THIS) );
    }
  }
  #line 936
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( builder_1 ))));
}

#line 959
RogueReal64 RogueString__to_Real64( RogueString* THIS )
{
  ROGUE_GC_CHECK;
  #line 960
  if (ROGUE_COND(((((RogueString__count( ROGUE_ARG(THIS) )))) == (0))))
  {
    return (RogueReal64)(0.0);
  }
  #line 961
  return (RogueReal64)(((RogueReal64)(strtod( (char*)THIS->utf8, 0 ))));
}

#line 1002
RogueString* RogueString__trimmed( RogueString* THIS )
{
  ROGUE_GC_CHECK;
  #line 1004
  RogueInt32 i1_0 = (0);
  #line 1005
  RogueInt32 i2_1 = (((((RogueString__count( ROGUE_ARG(THIS) )))) - (1)));
  #line 1007
  while (ROGUE_COND(((i1_0) <= (i2_1))))
  {
    ROGUE_GC_CHECK;
    #line 1008
    if (ROGUE_COND(((((RogueString__get__Int32( ROGUE_ARG(THIS), i1_0 )))) <= ((RogueCharacter)' '))))
    {
      ++i1_0;
    }
    else if (ROGUE_COND(((((RogueString__get__Int32( ROGUE_ARG(THIS), i2_1 )))) <= ((RogueCharacter)' '))))
    {
      #line 1009
      --i2_1;
    }
    else
    {
      #line 1010
      goto _auto_482;
    }
  }
  _auto_482:;
  #line 1013
  if (ROGUE_COND(((i1_0) > (i2_1))))
  {
    return (RogueString*)(Rogue_literal_strings[0]);
  }
  #line 1014
  if (ROGUE_COND(((((i1_0) == (0))) && (((i2_1) == (((((RogueString__count( ROGUE_ARG(THIS) )))) - (1))))))))
  {
    return (RogueString*)(THIS);
  }
  #line 1015
  return (RogueString*)(((RogueString*)(RogueString__from__Int32_Int32( ROGUE_ARG(THIS), i1_0, i2_1 ))));
}

#line 1037
RogueString_List* RogueString__word_wrap__Int32_String( RogueString* THIS, RogueInt32 width_0, RogueString* allow_break_after_1 )
{
  ROGUE_GC_CHECK;
  #line 1042
  return (RogueString_List*)(((RogueString_List*)(RogueString__split__Character_Logical( ROGUE_ARG(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueString__word_wrap__Int32_StringBuilder_String( ROGUE_ARG(THIS), width_0, ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), allow_break_after_1 )))) )))), (RogueCharacter)10, false ))));
}

#line 1044
RogueStringBuilder* RogueString__word_wrap__Int32_StringBuilder_String( RogueString* THIS, RogueInt32 width_0, RogueStringBuilder* buffer_1, RogueString* allow_break_after_2 )
{
  ROGUE_GC_CHECK;
  #line 1049
  RogueInt32 i1_3 = 0;
  RogueInt32 i2_4 = 0;
  #line 1050
  RogueInt32 len_5 = (((RogueString__count( ROGUE_ARG(THIS) ))));
  #line 1052
  if (ROGUE_COND(((len_5) == (0))))
  {
    return (RogueStringBuilder*)(buffer_1);
  }
  #line 1054
  RogueInt32 w_6 = (width_0);
  #line 1055
  RogueInt32 initial_indent_7 = (0);
  #line 1056
  {
    ROGUE_DEF_LOCAL_REF(RogueString*,_auto_483_0,(THIS));
    RogueInt32 _auto_484_0 = (0);
    RogueInt32 _auto_485_0 = (((((RogueString__count( _auto_483_0 )))) - (1)));
    for (;ROGUE_COND(((_auto_484_0) <= (_auto_485_0)));++_auto_484_0)
    {
      ROGUE_GC_CHECK;
      RogueCharacter ch_0 = (((RogueString__get__Int32( _auto_483_0, _auto_484_0 ))));
      #line 1057
      if (ROGUE_COND(((ch_0) != ((RogueCharacter)' '))))
      {
        goto _auto_486;
      }
      #line 1058
      ++initial_indent_7;
      #line 1059
      --w_6;
      #line 1060
      ++i1_3;
    }
  }
  _auto_486:;
  #line 1063
  if (ROGUE_COND(((w_6) <= (0))))
  {
    #line 1064
    w_6 = ((RogueInt32)(width_0));
    #line 1065
    initial_indent_7 = ((RogueInt32)(0));
    #line 1066
    RogueStringBuilder__println( buffer_1 );
  }
  else
  {
    #line 1068
    {
      RogueInt32 _auto_155_8 = (1);
      RogueInt32 _auto_156_9 = (((width_0) - (w_6)));
      for (;ROGUE_COND(((_auto_155_8) <= (_auto_156_9)));++_auto_155_8)
      {
        ROGUE_GC_CHECK;
        RogueStringBuilder__print__Character_Logical( buffer_1, (RogueCharacter)' ', true );
      }
    }
  }
  #line 1071
  RogueLogical needs_newline_10 = (false);
  #line 1072
  while (ROGUE_COND(((i2_4) < (len_5))))
  {
    ROGUE_GC_CHECK;
    #line 1075
    while (ROGUE_COND(((((((((i2_4) - (i1_3))) < (w_6))) && (((i2_4) < (len_5))))) && (((((RogueString__get__Int32( ROGUE_ARG(THIS), i2_4 )))) != ((RogueCharacter)10))))))
    {
      ROGUE_GC_CHECK;
      ++i2_4;
    }
    #line 1077
    if (ROGUE_COND(((((i2_4) - (i1_3))) == (w_6))))
    {
      #line 1079
      if (ROGUE_COND(((i2_4) >= (len_5))))
      {
        #line 1081
        i2_4 = ((RogueInt32)(len_5));
      }
      else if (ROGUE_COND(((((RogueString__get__Int32( ROGUE_ARG(THIS), i2_4 )))) != ((RogueCharacter)10))))
      {
        #line 1084
        while (ROGUE_COND(((((((RogueString__get__Int32( ROGUE_ARG(THIS), i2_4 )))) != ((RogueCharacter)' '))) && (((i2_4) > (i1_3))))))
        {
          ROGUE_GC_CHECK;
          --i2_4;
        }
        #line 1086
        if (ROGUE_COND(((i2_4) == (i1_3))))
        {
          #line 1089
          i2_4 = ((RogueInt32)(((i1_3) + (w_6))));
          #line 1090
          if (ROGUE_COND(!!(allow_break_after_2)))
          {
            #line 1091
            while (ROGUE_COND(((((((i2_4) > (i1_3))) && (!(((RogueString__contains__Character_Logical( allow_break_after_2, ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), ROGUE_ARG(((i2_4) - (1))) )))), false ))))))) && (((i2_4) > (i1_3))))))
            {
              ROGUE_GC_CHECK;
              --i2_4;
            }
            #line 1092
            if (ROGUE_COND(((i2_4) == (i1_3))))
            {
              #line 1094
              i2_4 = ((RogueInt32)(((i1_3) + (w_6))));
            }
          }
        }
      }
    }
    #line 1101
    if (ROGUE_COND(needs_newline_10))
    {
      #line 1102
      RogueStringBuilder__println( buffer_1 );
      #line 1103
      if (ROGUE_COND(!!(initial_indent_7)))
      {
        #line 1104
        {
          RogueInt32 _auto_157_11 = (1);
          RogueInt32 _auto_158_12 = (initial_indent_7);
          for (;ROGUE_COND(((_auto_157_11) <= (_auto_158_12)));++_auto_157_11)
          {
            ROGUE_GC_CHECK;
            RogueStringBuilder__print__Character_Logical( buffer_1, (RogueCharacter)' ', true );
          }
        }
      }
    }
    #line 1108
    {
      RogueInt32 i_13 = (i1_3);
      RogueInt32 _auto_159_14 = (((i2_4) - (1)));
      for (;ROGUE_COND(((i_13) <= (_auto_159_14)));++i_13)
      {
        ROGUE_GC_CHECK;
        RogueStringBuilder__print__Character_Logical( buffer_1, ROGUE_ARG(((RogueString__get__Int32( ROGUE_ARG(THIS), i_13 )))), true );
      }
    }
    #line 1109
    needs_newline_10 = ((RogueLogical)(true));
    #line 1111
    if (ROGUE_COND(((i2_4) == (len_5))))
    {
      #line 1112
      return (RogueStringBuilder*)(buffer_1);
    }
    else
    {
      #line 1114
      switch (((RogueString__get__Int32( ROGUE_ARG(THIS), i2_4 ))))
      {
        case (RogueCharacter)' ':
        {
          #line 1117
          while (ROGUE_COND(((((i2_4) < (len_5))) && (((((RogueString__get__Int32( ROGUE_ARG(THIS), i2_4 )))) == ((RogueCharacter)' '))))))
          {
            ROGUE_GC_CHECK;
            ++i2_4;
          }
          #line 1119
          if (ROGUE_COND(((((i2_4) < (len_5))) && (((((RogueString__get__Int32( ROGUE_ARG(THIS), i2_4 )))) == ((RogueCharacter)10))))))
          {
            ++i2_4;
          }
          #line 1121
          i1_3 = ((RogueInt32)(i2_4));
          break;
        }
        case (RogueCharacter)10:
        {
          #line 1124
          ++i2_4;
          #line 1126
          w_6 = ((RogueInt32)(width_0));
          #line 1127
          initial_indent_7 = ((RogueInt32)(0));
          #line 1128
          {
            RogueInt32 i_15 = (i2_4);
            RogueInt32 _auto_160_16 = (len_5);
            for (;ROGUE_COND(((i_15) < (_auto_160_16)));++i_15)
            {
              ROGUE_GC_CHECK;
              #line 1129
              if (ROGUE_COND(((((RogueString__get__Int32( ROGUE_ARG(THIS), i_15 )))) != ((RogueCharacter)' '))))
              {
                goto _auto_487;
              }
              #line 1130
              ++initial_indent_7;
              #line 1131
              --w_6;
              #line 1132
              ++i2_4;
            }
          }
          _auto_487:;
          #line 1135
          if (ROGUE_COND(((w_6) <= (0))))
          {
            #line 1136
            w_6 = ((RogueInt32)(width_0));
            #line 1137
            initial_indent_7 = ((RogueInt32)(0));
          }
          else
          {
            #line 1139
            {
              RogueInt32 _auto_161_17 = (1);
              RogueInt32 _auto_162_18 = (((width_0) - (w_6)));
              for (;ROGUE_COND(((_auto_161_17) <= (_auto_162_18)));++_auto_161_17)
              {
                ROGUE_GC_CHECK;
                RogueStringBuilder__print__Character_Logical( buffer_1, (RogueCharacter)' ', true );
              }
            }
          }
          break;
        }
      }
      #line 1143
      i1_3 = ((RogueInt32)(i2_4));
    }
  }
  #line 1148
  return (RogueStringBuilder*)(buffer_1);
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/StackTrace.rogue"
RogueClassStackTrace* RogueStackTrace__init_object( RogueClassStackTrace* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueClassStackTrace*)(THIS);
}

#line 52
RogueString* RogueStackTrace__to_String( RogueClassStackTrace* THIS )
{
  ROGUE_GC_CHECK;
  #line 53
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStackTrace__print__StringBuilder( ROGUE_ARG(THIS), ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))) )))) ))));
}

#line 1
RogueString* RogueStackTrace__type_name( RogueClassStackTrace* THIS )
{
  return (RogueString*)(Rogue_literal_strings[32]);
}

#line 8
RogueClassStackTrace* RogueStackTrace__init__Int32( RogueClassStackTrace* THIS, RogueInt32 omit_count_0 )
{
  ROGUE_GC_CHECK;
  #line 9
  ++omit_count_0;
  #line 10
  RogueDebugTrace* current = Rogue_call_stack;
  while (current && omit_count_0 > 0)
  {
    --omit_count_0;
    current = current->previous_trace;
  }
  if (current) THIS->count = current->count();

  #line 18
  THIS->entries = ((RogueString_List*)(RogueString_List__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueString_List*,ROGUE_CREATE_OBJECT(String_List))), ROGUE_ARG(THIS->count) )));
  #line 19
  while (ROGUE_COND(((RogueLogical)(current!=0))))
  {
    ROGUE_GC_CHECK;
    #line 20
    RogueString_List__add__String( ROGUE_ARG(THIS->entries), ROGUE_ARG(((RogueString*)(RogueString_create_from_utf8( current->to_c_string() )))) );
    #line 21
    current = current->previous_trace;

  }
  #line 24
  return (RogueClassStackTrace*)(THIS);
}

void RogueStackTrace__format( RogueClassStackTrace* THIS )
{
  ROGUE_GC_CHECK;
  #line 25
  if (ROGUE_COND(THIS->is_formatted))
  {
    return;
  }
  #line 26
  THIS->is_formatted = true;
  #line 28
  RogueInt32 max_characters_0 = (0);
  #line 29
  {
    ROGUE_DEF_LOCAL_REF(RogueString_List*,_auto_620_0,(THIS->entries));
    RogueInt32 _auto_621_0 = (0);
    RogueInt32 _auto_622_0 = (((_auto_620_0->count) - (1)));
    for (;ROGUE_COND(((_auto_621_0) <= (_auto_622_0)));++_auto_621_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueString*,entry_0,(((RogueString*)(_auto_620_0->data->as_objects[_auto_621_0]))));
      #line 30
      RogueOptionalInt32 sp_1 = (((RogueString__locate__Character_OptionalInt32_Logical( entry_0, (RogueCharacter)' ', (RogueOptionalInt32__create()), false ))));
      #line 31
      if (ROGUE_COND(sp_1.exists))
      {
        max_characters_0 = ((RogueInt32)((RogueMath__max__Int32_Int32( max_characters_0, ROGUE_ARG(sp_1.value) ))));
      }
    }
  }
  #line 34
  ++max_characters_0;
  #line 35
  {
    ROGUE_DEF_LOCAL_REF(RogueString_List*,_auto_623_0,(THIS->entries));
    RogueInt32 i_0 = (0);
    RogueInt32 _auto_624_0 = (((_auto_623_0->count) - (1)));
    for (;ROGUE_COND(((i_0) <= (_auto_624_0)));++i_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueString*,entry_0,(((RogueString*)(_auto_623_0->data->as_objects[i_0]))));
      #line 36
      if (ROGUE_COND(((RogueString__contains__Character_Logical( entry_0, (RogueCharacter)' ', false )))))
      {
        #line 37
        RogueString_List__set__Int32_String( ROGUE_ARG(THIS->entries), i_0, ROGUE_ARG((RogueString__operatorPLUS__String_String( ROGUE_ARG(((RogueString*)(RogueString__left_justified__Int32_Character( ROGUE_ARG(((RogueString*)(RogueString__before_first__Character_Logical( entry_0, (RogueCharacter)' ', false )))), max_characters_0, (RogueCharacter)' ' )))), ROGUE_ARG(((RogueString*)(RogueString__from_first__Character_Logical( entry_0, (RogueCharacter)' ', false )))) ))) );
      }
    }
  }
}

#line 41
void RogueStackTrace__print( RogueClassStackTrace* THIS )
{
  ROGUE_GC_CHECK;
  #line 42
  RogueStackTrace__print__StringBuilder( ROGUE_ARG(THIS), ROGUE_ARG(((RogueClassGlobal*)ROGUE_SINGLETON(Global))->global_output_buffer) );
  #line 43
  RogueGlobal__flush( ((RogueClassGlobal*)ROGUE_SINGLETON(Global)) );
}

#line 45
RogueStringBuilder* RogueStackTrace__print__StringBuilder( RogueClassStackTrace* THIS, RogueStringBuilder* buffer_0 )
{
  ROGUE_GC_CHECK;
  #line 46
  RogueStackTrace__format( ROGUE_ARG(THIS) );
  #line 47
  {
    ROGUE_DEF_LOCAL_REF(RogueString_List*,_auto_625_0,(THIS->entries));
    RogueInt32 _auto_626_0 = (0);
    RogueInt32 _auto_627_0 = (((_auto_625_0->count) - (1)));
    for (;ROGUE_COND(((_auto_626_0) <= (_auto_627_0)));++_auto_626_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueString*,entry_0,(((RogueString*)(_auto_625_0->data->as_objects[_auto_626_0]))));
      #line 48
      RogueStringBuilder__println__String( buffer_0, entry_0 );
    }
  }
  #line 50
  return (RogueStringBuilder*)(buffer_0);
}

#line 4 "../../Rogue/Programs/RogueC/Libraries/Standard/List.rogue"
RogueString_List* RogueString_List__init_object( RogueString_List* THIS )
{
  ROGUE_GC_CHECK;
  RogueGenericList__init_object( ROGUE_ARG(((RogueClassGenericList*)THIS)) );
  return (RogueString_List*)(THIS);
}

#line 10
RogueString_List* RogueString_List__init( RogueString_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 11
  RogueString_List__init__Int32( ROGUE_ARG(THIS), 0 );
  #line 13
  return (RogueString_List*)(THIS);
}

#line 633
RogueString* RogueString_List__to_String( RogueString_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 634
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 635
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)'[', true );
  #line 636
  RogueLogical first_1 = (true);
  #line 637
  {
    ROGUE_DEF_LOCAL_REF(RogueString_List*,_auto_918_0,(THIS));
    RogueInt32 _auto_919_0 = (0);
    RogueInt32 _auto_920_0 = (((_auto_918_0->count) - (1)));
    for (;ROGUE_COND(((_auto_919_0) <= (_auto_920_0)));++_auto_919_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueString*,value_0,(((RogueString*)(_auto_918_0->data->as_objects[_auto_919_0]))));
      #line 638
      if (ROGUE_COND(first_1))
      {
        first_1 = ((RogueLogical)(false));
      }
      else
      {
        #line 639
        RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)',', true );
      }
      #line 640
      if (ROGUE_COND(((void*)value_0) == ((void*)NULL)))
      {
        RogueStringBuilder__print__String( buffer_0, Rogue_literal_strings[1] );
      }
      else
      {
        #line 641
        RogueStringBuilder__print__String( buffer_0, value_0 );
      }
    }
  }
  #line 643
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)']', true );
  #line 644
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_0 ))));
}

#line 4
RogueString* RogueString_List__type_name( RogueString_List* THIS )
{
  return (RogueString*)(Rogue_literal_strings[46]);
}

#line 13
RogueString_List* RogueString_List__init__Int32( RogueString_List* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 14
  if (ROGUE_COND(((initial_capacity_0) != (((RogueString_List__capacity( ROGUE_ARG(THIS) )))))))
  {
    THIS->data = RogueType_create_array( initial_capacity_0, sizeof(RogueString*), true, 17 );
  }
  #line 15
  RogueString_List__clear( ROGUE_ARG(THIS) );
  #line 17
  return (RogueString_List*)(THIS);
}

#line 58
RogueString_List* RogueString_List__add__String( RogueString_List* THIS, RogueString* value_0 )
{
  ROGUE_GC_CHECK;
  #line 59
  RogueString_List__set__Int32_String( ROGUE_ARG(((RogueString_List*)(RogueString_List__reserve__Int32( ROGUE_ARG(THIS), 1 )))), ROGUE_ARG(THIS->count), value_0 );
  #line 60
  THIS->count = ((THIS->count) + (1));
  #line 61
  return (RogueString_List*)(THIS);
}

#line 68
RogueInt32 RogueString_List__capacity( RogueString_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 69
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    return (RogueInt32)(0);
  }
  #line 70
  return (RogueInt32)(THIS->data->count);
}

#line 72
RogueString_List* RogueString_List__clear( RogueString_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 73
  RogueString_List__discard_from__Int32( ROGUE_ARG(THIS), 0 );
  #line 74
  return (RogueString_List*)(THIS);
}

#line 174
RogueString_List* RogueString_List__discard_from__Int32( RogueString_List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 175
  if (ROGUE_COND(!!(THIS->data)))
  {
    #line 176
    RogueInt32 n_1 = (((THIS->count) - (index_0)));
    #line 177
    if (ROGUE_COND(((n_1) > (0))))
    {
      #line 178
      RogueArray__zero__Int32_Int32( ROGUE_ARG(((RogueArray*)THIS->data)), index_0, n_1 );
      #line 179
      THIS->count = index_0;
    }
  }
  #line 182
  return (RogueString_List*)(THIS);
}

#line 467
RogueString_List* RogueString_List__reserve__Int32( RogueString_List* THIS, RogueInt32 additional_elements_0 )
{
  ROGUE_GC_CHECK;
  #line 468
  RogueInt32 required_capacity_1 = (((THIS->count) + (additional_elements_0)));
  #line 469
  if (ROGUE_COND(((required_capacity_1) == (0))))
  {
    return (RogueString_List*)(THIS);
  }
  #line 471
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    #line 472
    if (ROGUE_COND(((required_capacity_1) == (1))))
    {
      required_capacity_1 = ((RogueInt32)(10));
    }
    #line 473
    THIS->data = RogueType_create_array( required_capacity_1, sizeof(RogueString*), true, 17 );
  }
  else if (ROGUE_COND(((required_capacity_1) > (THIS->data->count))))
  {
    #line 475
    RogueInt32 cap_2 = (((RogueString_List__capacity( ROGUE_ARG(THIS) ))));
    #line 476
    if (ROGUE_COND(((required_capacity_1) < (((cap_2) + (cap_2))))))
    {
      required_capacity_1 = ((RogueInt32)(((cap_2) + (cap_2))));
    }
    #line 477
    ROGUE_DEF_LOCAL_REF(RogueArray*,new_data_3,(RogueType_create_array( required_capacity_1, sizeof(RogueString*), true, 17 )));
    #line 478
    RogueArray_set(new_data_3,0,((RogueArray*)(THIS->data)),0,-1);
    #line 479
    THIS->data = new_data_3;
  }
  #line 482
  return (RogueString_List*)(THIS);
}

#line 562
void RogueString_List__set__Int32_String( RogueString_List* THIS, RogueInt32 index_0, RogueString* new_value_1 )
{
  ROGUE_GC_CHECK;
  #line 563
  THIS->data->as_objects[index_0] = new_value_1;
}

#line 823
RogueString* RogueString_List__join__String( RogueString_List* THIS, RogueString* separator_0 )
{
  ROGUE_GC_CHECK;
  #line 824
  RogueInt32 total_count_1 = (0);
  #line 825
  {
    ROGUE_DEF_LOCAL_REF(RogueString_List*,_auto_985_0,(THIS));
    RogueInt32 _auto_986_0 = (0);
    RogueInt32 _auto_987_0 = (((_auto_985_0->count) - (1)));
    for (;ROGUE_COND(((_auto_986_0) <= (_auto_987_0)));++_auto_986_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueString*,line_0,(((RogueString*)(_auto_985_0->data->as_objects[_auto_986_0]))));
      total_count_1 += ((RogueString__count( line_0 )));
    }
  }
  #line 827
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,builder_2,(((RogueStringBuilder*)(RogueStringBuilder__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))), total_count_1 )))));
  #line 828
  RogueLogical first_3 = (true);
  #line 829
  {
    ROGUE_DEF_LOCAL_REF(RogueString_List*,_auto_988_0,(THIS));
    RogueInt32 _auto_989_0 = (0);
    RogueInt32 _auto_990_0 = (((_auto_988_0->count) - (1)));
    for (;ROGUE_COND(((_auto_989_0) <= (_auto_990_0)));++_auto_989_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueString*,line_0,(((RogueString*)(_auto_988_0->data->as_objects[_auto_989_0]))));
      #line 830
      if (ROGUE_COND(first_3))
      {
        first_3 = ((RogueLogical)(false));
      }
      else
      {
        #line 831
        RogueStringBuilder__print__String( builder_2, separator_0 );
      }
      #line 832
      RogueStringBuilder__print__String( builder_2, line_0 );
    }
  }
  #line 835
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( builder_2 ))));
}

#line 18 "../../Rogue/Programs/RogueC/Libraries/Standard/Array.rogue"
RogueString* RogueArray_String___type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[51]);
}

#line 476 "../../Rogue/Programs/RogueC/Libraries/Standard/Primitives.rogue"
RogueStringBuilder* RogueInt64__print_in_power2_base__Int32_Int32_StringBuilder( RogueInt64 THIS, RogueInt32 base_0, RogueInt32 digits_1, RogueStringBuilder* buffer_2 )
{
  ROGUE_GC_CHECK;
  #line 477
  RogueInt32 bits_3 = (0);
  #line 478
  RogueInt32 temp_4 = (base_0);
  #line 479
  while (ROGUE_COND(((temp_4) > (1))))
  {
    ROGUE_GC_CHECK;
    ++bits_3;
    temp_4 = ((RogueInt32)(((temp_4) >> (1))));
  }
  #line 481
  RogueInt64 remaining_5 = ((RogueMath__shift_right__Int64_Int64( ROGUE_ARG(THIS), ROGUE_ARG(((RogueInt64)(bits_3))) )));
  #line 482
  if (ROGUE_COND(((((digits_1) > (1))) || (!!(remaining_5)))))
  {
    RogueInt64__print_in_power2_base__Int32_Int32_StringBuilder( remaining_5, base_0, ROGUE_ARG(((digits_1) - (1))), buffer_2 );
  }
  #line 483
  RogueStringBuilder__print__Character_Logical( buffer_2, ROGUE_ARG(((RogueInt32__to_digit__Logical( ROGUE_ARG(((RogueInt32)(((THIS) & (((RogueInt64)(((base_0) - (1))))))))), false )))), true );
  #line 484
  return (RogueStringBuilder*)(buffer_2);
}

#line 557
RogueString* RogueInt64__to_hex_string__Int32( RogueInt64 THIS, RogueInt32 digits_0 )
{
  ROGUE_GC_CHECK;
  #line 558
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueInt64__print_in_power2_base__Int32_Int32_StringBuilder( ROGUE_ARG(THIS), 16, digits_0, ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))) )))) ))));
}

#line 794
RogueLogical RogueCharacter__is_letter( RogueCharacter THIS )
{
  ROGUE_GC_CHECK;
  #line 795
  return (RogueLogical)(((((((THIS) >= ((RogueCharacter)'a'))) && (((THIS) <= ((RogueCharacter)'z'))))) || (((((THIS) >= ((RogueCharacter)'A'))) && (((THIS) <= ((RogueCharacter)'Z')))))));
}

#line 807
RogueLogical RogueCharacter__is_uppercase( RogueCharacter THIS )
{
  ROGUE_GC_CHECK;
  #line 808
  return (RogueLogical)(((((THIS) >= ((RogueCharacter)'A'))) && (((THIS) <= ((RogueCharacter)'Z')))));
}

#line 835
RogueString* RogueCharacter__to_String( RogueCharacter THIS )
{
  ROGUE_GC_CHECK;
  #line 836
  return (RogueString*)(((RogueString*)(RogueString__operatorPLUS__Character( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), ROGUE_ARG(THIS) ))));
}

#line 892
RogueCharacter RogueCharacter__to_lowercase( RogueCharacter THIS )
{
  ROGUE_GC_CHECK;
  #line 893
  if (ROGUE_COND(!(((RogueCharacter__is_uppercase( ROGUE_ARG(THIS) ))))))
  {
    return (RogueCharacter)(THIS);
  }
  #line 894
  return (RogueCharacter)((RogueCharacter__create__Character( ROGUE_ARG(((THIS) + ((((RogueCharacter)'a') - ((RogueCharacter)'A'))))) )));
}

#line 4 "../../Rogue/Programs/RogueC/Libraries/Standard/List.rogue"
RogueCharacter_List* RogueCharacter_List__init_object( RogueCharacter_List* THIS )
{
  ROGUE_GC_CHECK;
  RogueGenericList__init_object( ROGUE_ARG(((RogueClassGenericList*)THIS)) );
  return (RogueCharacter_List*)(THIS);
}

#line 10
RogueCharacter_List* RogueCharacter_List__init( RogueCharacter_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 11
  RogueCharacter_List__init__Int32( ROGUE_ARG(THIS), 0 );
  #line 13
  return (RogueCharacter_List*)(THIS);
}

#line 633
RogueString* RogueCharacter_List__to_String( RogueCharacter_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 634
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 635
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)'[', true );
  #line 636
  RogueLogical first_1 = (true);
  #line 637
  {
    ROGUE_DEF_LOCAL_REF(RogueCharacter_List*,_auto_1302_0,(THIS));
    RogueInt32 _auto_1303_0 = (0);
    RogueInt32 _auto_1304_0 = (((_auto_1302_0->count) - (1)));
    for (;ROGUE_COND(((_auto_1303_0) <= (_auto_1304_0)));++_auto_1303_0)
    {
      ROGUE_GC_CHECK;
      RogueCharacter value_0 = (_auto_1302_0->data->as_characters[_auto_1303_0]);
      #line 638
      if (ROGUE_COND(first_1))
      {
        first_1 = ((RogueLogical)(false));
      }
      else
      {
        #line 639
        RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)',', true );
      }
      #line 640
      if (ROGUE_COND((false)))
      {
      }
      else
      {
        #line 641
        RogueStringBuilder__print__String( buffer_0, ROGUE_ARG(((RogueString*)(RogueCharacter__to_String( value_0 )))) );
      }
    }
  }
  #line 643
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)']', true );
  #line 644
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_0 ))));
}

#line 4
RogueString* RogueCharacter_List__type_name( RogueCharacter_List* THIS )
{
  return (RogueString*)(Rogue_literal_strings[45]);
}

#line 13
RogueCharacter_List* RogueCharacter_List__init__Int32( RogueCharacter_List* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 14
  if (ROGUE_COND(((initial_capacity_0) != (((RogueCharacter_List__capacity( ROGUE_ARG(THIS) )))))))
  {
    THIS->data = RogueType_create_array( initial_capacity_0, sizeof(RogueCharacter), false, 23 );
  }
  #line 15
  RogueCharacter_List__clear( ROGUE_ARG(THIS) );
  #line 17
  return (RogueCharacter_List*)(THIS);
}

#line 58
RogueCharacter_List* RogueCharacter_List__add__Character( RogueCharacter_List* THIS, RogueCharacter value_0 )
{
  ROGUE_GC_CHECK;
  #line 59
  RogueCharacter_List__set__Int32_Character( ROGUE_ARG(((RogueCharacter_List*)(RogueCharacter_List__reserve__Int32( ROGUE_ARG(THIS), 1 )))), ROGUE_ARG(THIS->count), value_0 );
  #line 60
  THIS->count = ((THIS->count) + (1));
  #line 61
  return (RogueCharacter_List*)(THIS);
}

#line 68
RogueInt32 RogueCharacter_List__capacity( RogueCharacter_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 69
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    return (RogueInt32)(0);
  }
  #line 70
  return (RogueInt32)(THIS->data->count);
}

#line 72
RogueCharacter_List* RogueCharacter_List__clear( RogueCharacter_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 73
  RogueCharacter_List__discard_from__Int32( ROGUE_ARG(THIS), 0 );
  #line 74
  return (RogueCharacter_List*)(THIS);
}

#line 174
RogueCharacter_List* RogueCharacter_List__discard_from__Int32( RogueCharacter_List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 175
  if (ROGUE_COND(!!(THIS->data)))
  {
    #line 176
    RogueInt32 n_1 = (((THIS->count) - (index_0)));
    #line 177
    if (ROGUE_COND(((n_1) > (0))))
    {
      #line 178
      RogueArray__zero__Int32_Int32( ROGUE_ARG(((RogueArray*)THIS->data)), index_0, n_1 );
      #line 179
      THIS->count = index_0;
    }
  }
  #line 182
  return (RogueCharacter_List*)(THIS);
}

#line 467
RogueCharacter_List* RogueCharacter_List__reserve__Int32( RogueCharacter_List* THIS, RogueInt32 additional_elements_0 )
{
  ROGUE_GC_CHECK;
  #line 468
  RogueInt32 required_capacity_1 = (((THIS->count) + (additional_elements_0)));
  #line 469
  if (ROGUE_COND(((required_capacity_1) == (0))))
  {
    return (RogueCharacter_List*)(THIS);
  }
  #line 471
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    #line 472
    if (ROGUE_COND(((required_capacity_1) == (1))))
    {
      required_capacity_1 = ((RogueInt32)(10));
    }
    #line 473
    THIS->data = RogueType_create_array( required_capacity_1, sizeof(RogueCharacter), false, 23 );
  }
  else if (ROGUE_COND(((required_capacity_1) > (THIS->data->count))))
  {
    #line 475
    RogueInt32 cap_2 = (((RogueCharacter_List__capacity( ROGUE_ARG(THIS) ))));
    #line 476
    if (ROGUE_COND(((required_capacity_1) < (((cap_2) + (cap_2))))))
    {
      required_capacity_1 = ((RogueInt32)(((cap_2) + (cap_2))));
    }
    #line 477
    ROGUE_DEF_LOCAL_REF(RogueArray*,new_data_3,(RogueType_create_array( required_capacity_1, sizeof(RogueCharacter), false, 23 )));
    #line 478
    RogueArray_set(new_data_3,0,((RogueArray*)(THIS->data)),0,-1);
    #line 479
    THIS->data = new_data_3;
  }
  #line 482
  return (RogueCharacter_List*)(THIS);
}

#line 562
void RogueCharacter_List__set__Int32_Character( RogueCharacter_List* THIS, RogueInt32 index_0, RogueCharacter new_value_1 )
{
  ROGUE_GC_CHECK;
  #line 563
  THIS->data->as_characters[index_0] = (new_value_1);
}

#line 18 "../../Rogue/Programs/RogueC/Libraries/Standard/Array.rogue"
RogueString* RogueArray_Character___type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[55]);
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/Console.rogue"
RogueClassConsole* RogueConsole__init_object( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 49
  THIS->error = ((RogueClassConsoleErrorPrinter*)(((RogueObject*)Rogue_call_ROGUEM0( 1, ROGUE_ARG(((RogueObject*)ROGUE_CREATE_REF(RogueClassConsoleErrorPrinter*,ROGUE_CREATE_OBJECT(ConsoleErrorPrinter)))) ))));
  #line 51
  THIS->immediate_mode = false;
  #line 54
  THIS->is_blocking = true;
  #line 56
  THIS->decode_bytes = true;
  #line 58
  THIS->output_buffer = ((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )));
  #line 59
  THIS->input_buffer = ((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )));
  #line 62
  THIS->input_bytes = ((RogueByte_List*)(RogueByte_List__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueByte_List*,ROGUE_CREATE_OBJECT(Byte_List))) )));
  #line 1
  return (RogueClassConsole*)(THIS);
}

#line 70
RogueClassConsole* RogueConsole__init( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 71
#if (ROGUE_CONSOLE_FULL)
    tcgetattr( STDIN_FILENO, &THIS->original_terminal_settings );
    THIS->original_stdin_flags = fcntl( STDIN_FILENO, F_GETFL );
#endif

  #line 76
  RogueGlobal__on_exit___Function___( ((RogueClassGlobal*)ROGUE_SINGLETON(Global)), ROGUE_ARG(((RogueClass_Function___*)(((RogueClassFunction_1670*)(RogueFunction_1670__init__Console( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassFunction_1670*,ROGUE_CREATE_OBJECT(Function_1670))), ROGUE_ARG(THIS) )))))) );
  #line 78
  return (RogueClassConsole*)(THIS);
}

#line 46 "../../Rogue/Programs/RogueC/Libraries/Standard/Reader.rogue"
RogueString* RogueConsole__to_String( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 47
  ROGUE_DEF_LOCAL_REF(RogueCharacter_List*,buffer_0,(((RogueCharacter_List*)(RogueCharacter_List__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueCharacter_List*,ROGUE_CREATE_OBJECT(Character_List))) )))));
  #line 48
  while (ROGUE_COND(((RogueConsole__has_another( ROGUE_ARG(THIS) )))))
  {
    ROGUE_GC_CHECK;
    RogueCharacter_List__add__Character( buffer_0, ROGUE_ARG(((RogueConsole__read( ROGUE_ARG(THIS) )))) );
  }
  #line 49
  if (ROGUE_COND((false)))
  {
  }
  else
  {
    #line 52
    return (RogueString*)(((RogueString*)(RogueCharacter_List__to_String( buffer_0 ))));
  }
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/Console.rogue"
RogueString* RogueConsole__type_name( RogueClassConsole* THIS )
{
  return (RogueString*)(Rogue_literal_strings[33]);
}

#line 95
RogueLogical RogueConsole__has_another( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 96
  if (ROGUE_COND(!(THIS->next_input_character.exists)))
  {
    #line 97
    RogueConsole__fill_input_queue( ROGUE_ARG(THIS) );
    #line 98
    if (ROGUE_COND(!!(THIS->input_bytes->count)))
    {
      #line 99
      RogueByte b1_0 = (((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) ))));
      #line 100
      {
        {
          {
            #line 101
            if ( !(THIS->decode_bytes) ) goto _auto_1673;
            #line 102
            if ( !(THIS->input_bytes->count) ) goto _auto_1673;
            #line 105
            if (ROGUE_COND(((((RogueInt32)(b1_0))) == (27))))
            {
              #line 107
              if ( !(((((THIS->input_bytes->count) >= (2))) && (((((RogueInt32)(((RogueByte_List__first( ROGUE_ARG(THIS->input_bytes) )))))) == (91))))) ) goto _auto_1673;
              #line 108
              RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) );
              #line 109
              THIS->next_input_character = RogueOptionalInt32( ((((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) - (65))) + (17)), true );
            }
            else
            {
              #line 112
              if ( !(((((RogueInt32)(b1_0))) >= (192))) ) goto _auto_1673;
              #line 114
              RogueInt32 result_1 = 0;
              #line 117
              if (ROGUE_COND(((((((RogueInt32)(b1_0))) & (224))) == (192))))
              {
                #line 118
                if ( !(((THIS->input_bytes->count) >= (1))) ) goto _auto_1673;
                #line 119
                result_1 = ((RogueInt32)(((((RogueInt32)(b1_0))) & (31))));
                #line 120
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
              }
              else if (ROGUE_COND(((((((RogueInt32)(b1_0))) & (240))) == (224))))
              {
                #line 122
                if ( !(((THIS->input_bytes->count) >= (2))) ) goto _auto_1673;
                #line 123
                result_1 = ((RogueInt32)(((((RogueInt32)(b1_0))) & (15))));
                #line 124
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 125
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
              }
              else if (ROGUE_COND(((((((RogueInt32)(b1_0))) & (248))) == (240))))
              {
                #line 127
                if ( !(((THIS->input_bytes->count) >= (3))) ) goto _auto_1673;
                #line 128
                result_1 = ((RogueInt32)(((((RogueInt32)(b1_0))) & (7))));
                #line 129
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 130
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 131
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
              }
              else if (ROGUE_COND(((((((RogueInt32)(b1_0))) & (252))) == (248))))
              {
                #line 133
                if ( !(((THIS->input_bytes->count) >= (4))) ) goto _auto_1673;
                #line 134
                result_1 = ((RogueInt32)(((((RogueInt32)(b1_0))) & (3))));
                #line 135
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 136
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 137
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 138
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
              }
              else
              {
                #line 140
                if ( !(((THIS->input_bytes->count) >= (5))) ) goto _auto_1673;
                #line 141
                result_1 = ((RogueInt32)(((((RogueInt32)(b1_0))) & (1))));
                #line 142
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 143
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 144
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 145
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
                #line 146
                result_1 = ((RogueInt32)(((((result_1) << (6))) | (((((RogueInt32)(((RogueByte_List__remove_first( ROGUE_ARG(THIS->input_bytes) )))))) & (63))))));
              }
              #line 149
              THIS->next_input_character = RogueOptionalInt32( result_1, true );
            }
            }
          goto _auto_1672;
        }
        _auto_1673:;
        {
          #line 153
          THIS->next_input_character = RogueOptionalInt32( ((RogueInt32)(b1_0)), true );
          }
      }
      _auto_1672:;
      #line 157
      if (ROGUE_COND(((((THIS->input_bytes->count) > (0))) && (((THIS->input_bytes->count) < (6))))))
      {
        #line 160
        RogueConsole__fill_input_queue( ROGUE_ARG(THIS) );
      }
    }
  }
  #line 165
  return (RogueLogical)(THIS->next_input_character.exists);
}

#line 192
RogueCharacter RogueConsole__read( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 193
  if (ROGUE_COND(!(((RogueConsole__has_another( ROGUE_ARG(THIS) ))))))
  {
    return (RogueCharacter)((RogueCharacter)0);
  }
  #line 194
  RogueInt32 result_0 = (THIS->next_input_character.value);
  #line 195
  THIS->next_input_character = (RogueOptionalInt32__create());
  #line 196
  return (RogueCharacter)(((RogueCharacter)(result_0)));
}

#line 47 "../../Rogue/Programs/RogueC/Libraries/Standard/PrintWriter.rogue"
RogueClassConsole* RogueConsole__flush( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 48
  RogueConsole__write__StringBuilder( ROGUE_ARG(THIS), ROGUE_ARG(THIS->output_buffer) );
  #line 49
  RogueStringBuilder__clear( ROGUE_ARG(THIS->output_buffer) );
  #line 50
  return (RogueClassConsole*)(THIS);
}

#line 287 "../../Rogue/Programs/RogueC/Libraries/Standard/Console.rogue"
RogueClassConsole* RogueConsole__write__StringBuilder( RogueClassConsole* THIS, RogueStringBuilder* buffer_0 )
{
  ROGUE_GC_CHECK;
  #line 293
  fwrite( buffer_0->utf8->data->as_bytes, 1, buffer_0->utf8->count, stdout );
  fflush( stdout );

  #line 296
  return (RogueClassConsole*)(THIS);
}

#line 84
void RogueConsole__fill_input_queue( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 86
  RogueInt32 n_0 = 0;
  #line 87
  char bytes[8];
  n_0 = (RogueInt32) ROGUE_READ_CALL( STDIN_FILENO, &bytes, 8 );

  #line 89
  if (ROGUE_COND(((n_0) > (0))))
  {
    #line 90
    {
      RogueInt32 i_1 = (0);
      RogueInt32 _auto_488_2 = (n_0);
      for (;ROGUE_COND(((i_1) < (_auto_488_2)));++i_1)
      {
        ROGUE_GC_CHECK;
        #line 91
        RogueByte_List__add__Byte( ROGUE_ARG(THIS->input_bytes), ROGUE_ARG(((RogueByte)(((RogueByte)bytes[i_1])))) );
      }
    }
  }
}

#line 214
void RogueConsole__reset_input_mode( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 215
#if (ROGUE_CONSOLE_FULL)
    tcsetattr( STDIN_FILENO, TCSANOW, &THIS->original_terminal_settings );
    fcntl( STDIN_FILENO, F_SETFL, THIS->original_stdin_flags );
#endif

}

#line 268
RogueInt32 RogueConsole__width( RogueClassConsole* THIS )
{
  ROGUE_GC_CHECK;
  #line 269
#if (ROGUE_CONSOLE_FULL)
    struct winsize sz;
    ioctl( STDOUT_FILENO, TIOCGWINSZ, &sz );

    return sz.ws_col;
#else
    return 80;
#endif

}

#line 47 "../../Rogue/Programs/RogueC/Libraries/Standard/PrintWriter.rogue"
RogueClassPrintWriter_output_buffer_* RoguePrintWriter_output_buffer___flush( RogueObject* THIS )
{
  switch (THIS->type->index)
  {
    case 27:
      return (RogueClassPrintWriter_output_buffer_*)RogueConsole__flush( (RogueClassConsole*)THIS );
    case 29:
      return (RogueClassPrintWriter_output_buffer_*)RogueConsoleErrorPrinter__flush( (RogueClassConsoleErrorPrinter*)THIS );
    default:
      return 0;
  }
}

#line 38
RogueClassPrintWriter_output_buffer_* RoguePrintWriter_output_buffer___write__StringBuilder( RogueObject* THIS, RogueStringBuilder* buffer_0 )
{
  switch (THIS->type->index)
  {
    case 27:
      return (RogueClassPrintWriter_output_buffer_*)RogueConsole__write__StringBuilder( (RogueClassConsole*)THIS, buffer_0 );
    case 29:
      return (RogueClassPrintWriter_output_buffer_*)RogueConsoleErrorPrinter__write__StringBuilder( (RogueClassConsoleErrorPrinter*)THIS, buffer_0 );
    default:
      return 0;
  }
}

#line 300 "../../Rogue/Programs/RogueC/Libraries/Standard/Console.rogue"
RogueClassConsoleErrorPrinter* RogueConsoleErrorPrinter__init_object( RogueClassConsoleErrorPrinter* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 302
  THIS->output_buffer = ((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )));
  #line 300
  return (RogueClassConsoleErrorPrinter*)(THIS);
}

RogueString* RogueConsoleErrorPrinter__type_name( RogueClassConsoleErrorPrinter* THIS )
{
  return (RogueString*)(Rogue_literal_strings[41]);
}

#line 47 "../../Rogue/Programs/RogueC/Libraries/Standard/PrintWriter.rogue"
RogueClassConsoleErrorPrinter* RogueConsoleErrorPrinter__flush( RogueClassConsoleErrorPrinter* THIS )
{
  ROGUE_GC_CHECK;
  #line 48
  RogueConsoleErrorPrinter__write__StringBuilder( ROGUE_ARG(THIS), ROGUE_ARG(THIS->output_buffer) );
  #line 49
  RogueStringBuilder__clear( ROGUE_ARG(THIS->output_buffer) );
  #line 50
  return (RogueClassConsoleErrorPrinter*)(THIS);
}

#line 314 "../../Rogue/Programs/RogueC/Libraries/Standard/Console.rogue"
RogueClassConsoleErrorPrinter* RogueConsoleErrorPrinter__write__StringBuilder( RogueClassConsoleErrorPrinter* THIS, RogueStringBuilder* buffer_0 )
{
  ROGUE_GC_CHECK;
  #line 319
  fwrite( buffer_0->utf8->data->as_bytes, 1, buffer_0->utf8->count, stderr );
  fflush( stderr );

  #line 322
  return (RogueClassConsoleErrorPrinter*)(THIS);
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/Math.rogue"
RogueClassMath* RogueMath__init_object( RogueClassMath* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueClassMath*)(THIS);
}

RogueString* RogueMath__type_name( RogueClassMath* THIS )
{
  return (RogueString*)(Rogue_literal_strings[34]);
}

#line 9 "../../Rogue/Programs/RogueC/Libraries/Standard/Global.rogue"
RogueClassFunction_540* RogueFunction_540__init_object( RogueClassFunction_540* THIS )
{
  ROGUE_GC_CHECK;
  Rogue_Function_____init_object( ROGUE_ARG(((RogueClass_Function___*)THIS)) );
  return (RogueClassFunction_540*)(THIS);
}

RogueString* RogueFunction_540__type_name( RogueClassFunction_540* THIS )
{
  return (RogueString*)(Rogue_literal_strings[58]);
}

void RogueFunction_540__call( RogueClassFunction_540* THIS )
{
  RogueGlobal__flush( ((RogueClassGlobal*)ROGUE_SINGLETON(Global)) );
}

#line 11 "../Boss.rogue"
RogueClassBossScanTableBuilder* RogueBossScanTableBuilder__init_object( RogueClassBossScanTableBuilder* THIS )
{
  ROGUE_GC_CHECK;
  RogueScanTableBuilder__init_object( ROGUE_ARG(((RogueClassScanTableBuilder*)THIS)) );
  return (RogueClassBossScanTableBuilder*)(THIS);
}

#line 13
RogueClassBossScanTableBuilder* RogueBossScanTableBuilder__init( RogueClassBossScanTableBuilder* THIS )
{
  ROGUE_GC_CHECK;
  #line 14
  RogueScanState__accept( ROGUE_ARG(((RogueClassScanState*)(RogueScanState__link__Character_Int32_Int32( ROGUE_ARG(THIS->start), (RogueCharacter)10, ROGUE_ARG(((RogueTokenType__to_Int32( RogueClassTokenType( 0 ) )))), 0 )))) );
  #line 16
  ROGUE_DEF_LOCAL_REF(RogueClassScanState*,id_0,(([&]()->RogueClassScanState*{
    ROGUE_DEF_LOCAL_REF(RogueClassScanState*,_auto_541_0,(((RogueClassScanState*)(RogueScanState__init__Int32_Int32_StateFlags( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassScanState*,ROGUE_CREATE_OBJECT(ScanState))), ROGUE_ARG(((RogueTokenType__to_Int32( RogueClassTokenType( 1 ) )))), 0, RogueClassStateFlags( 0 ) )))));
    RogueScanState__accept( _auto_541_0 );
     return _auto_541_0;
  }())
  ));
  #line 17
  RogueScanState__link_alpha__ScanState( ROGUE_ARG(THIS->start), id_0 );
  #line 18
  ([&]()->RogueClassScanState*{
    ROGUE_DEF_LOCAL_REF(RogueClassScanState*,_auto_542_0,(id_0));
    RogueScanState__link_alpha__ScanState( _auto_542_0, id_0 );
    RogueScanState__link_numeric__ScanState( _auto_542_0, id_0 );
     return _auto_542_0;
  }());
  #line 20
  {
    #line 21
    ROGUE_DEF_LOCAL_REF(RogueClassScanState*,string_start_1,(((RogueClassScanState*)(RogueScanState__link__Character_TransitionActions_Int32_Int32_Int32( ROGUE_ARG(THIS->start), (RogueCharacter)'"', RogueClassTransitionActions( 8 ), 0, 0, 0 )))));
    #line 22
    ROGUE_DEF_LOCAL_REF(RogueClassScanState*,string_ch_2,(((RogueClassScanState*)(RogueScanState__init__Int32_Int32_StateFlags( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassScanState*,ROGUE_CREATE_OBJECT(ScanState))), 0, 0, RogueClassStateFlags( 0 ) )))));
    #line 23
    RogueScanState__advance__ScanState( string_start_1, string_ch_2 );
    #line 24
    ([&]()->RogueClassScanState*{
      ROGUE_DEF_LOCAL_REF(RogueClassScanState*,_auto_543_0,(string_ch_2));
      RogueScanState__link_alpha__ScanState( _auto_543_0, string_ch_2 );
      RogueScanState__link_numeric__ScanState( _auto_543_0, string_ch_2 );
       return _auto_543_0;
    }());
    #line 25
    RogueScanState__accept( ROGUE_ARG(((RogueClassScanState*)(RogueScanState__link__Character_Int32_Int32( string_ch_2, (RogueCharacter)'"', ROGUE_ARG(((RogueTokenType__to_Int32( RogueClassTokenType( 2 ) )))), 0 )))) );
  }
  #line 28
  return (RogueClassBossScanTableBuilder*)(THIS);
}

#line 11
RogueString* RogueBossScanTableBuilder__type_name( RogueClassBossScanTableBuilder* THIS )
{
  return (RogueString*)(Rogue_literal_strings[61]);
}

#line 101 "../ScanTableBuilder.rogue"
RogueClassScanTableBuilder* RogueScanTableBuilder__init_object( RogueClassScanTableBuilder* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 103
  THIS->start = ((RogueClassScanState*)(RogueScanState__init__Int32_Int32_StateFlags( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassScanState*,ROGUE_CREATE_OBJECT(ScanState))), 0, 0, RogueClassStateFlags( 0 ) )));
  #line 101
  return (RogueClassScanTableBuilder*)(THIS);
}

RogueString* RogueScanTableBuilder__type_name( RogueClassScanTableBuilder* THIS )
{
  return (RogueString*)(Rogue_literal_strings[37]);
}

#line 24
RogueClassScanState* RogueScanState__init_object( RogueClassScanState* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 36
  THIS->transitions = ((RogueTransition_List*)(RogueTransition_List__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueTransition_List*,ROGUE_CREATE_OBJECT(Transition_List))) )));
  #line 24
  return (RogueClassScanState*)(THIS);
}

RogueString* RogueScanState__type_name( RogueClassScanState* THIS )
{
  return (RogueString*)(Rogue_literal_strings[42]);
}

#line 39
RogueClassScanState* RogueScanState__init__Int32_Int32_StateFlags( RogueClassScanState* THIS, RogueInt32 _auto_636_0, RogueInt32 _auto_637_1, RogueClassStateFlags _auto_638_2 )
{
  ROGUE_GC_CHECK;
  THIS->flags = _auto_638_2;
  THIS->option = _auto_637_1;
  THIS->_type = _auto_636_0;
  #line 40
  RogueTransition_List__add__Transition( ROGUE_ARG(((RogueTransition_List*)(RogueTransition_List__add__Transition( ROGUE_ARG(((RogueTransition_List*)(RogueTransition_List__add__Transition( ROGUE_ARG(THIS->transitions), ROGUE_ARG(((RogueClassTransition*)(NULL))) )))), ROGUE_ARG(((RogueClassTransition*)(NULL))) )))), ROGUE_ARG(((RogueClassTransition*)(NULL))) );
  #line 42
  return (RogueClassScanState*)(THIS);
}

RogueClassScanState* RogueScanState__accept( RogueClassScanState* THIS )
{
  ROGUE_GC_CHECK;
  #line 43
  THIS->flags = ((RogueStateFlags__operatorOR__StateFlags( THIS->flags, RogueClassStateFlags( 1 ) )));
  #line 44
  return (RogueClassScanState*)(THIS);
}

#line 46
void RogueScanState__advance__ScanState( RogueClassScanState* THIS, RogueClassScanState* target_state_0 )
{
  ROGUE_GC_CHECK;
  #line 47
  THIS->flags = ((RogueStateFlags__operatorOR__StateFlags( THIS->flags, RogueClassStateFlags( 4 ) )));
  #line 48
  RogueScanState__link_default__ScanState( ROGUE_ARG(THIS), target_state_0 );
}

#line 53
RogueClassScanState* RogueScanState__find_link__Character( RogueClassScanState* THIS, RogueCharacter ch_0 )
{
  ROGUE_GC_CHECK;
  #line 54
  {
    ROGUE_DEF_LOCAL_REF(RogueTransition_List*,_auto_1700_0,(THIS->transitions));
    RogueInt32 _auto_1701_0 = (3);
    RogueInt32 _auto_1702_0 = (((_auto_1700_0->count) - (1)));
    for (;ROGUE_COND(((_auto_1701_0) <= (_auto_1702_0)));++_auto_1701_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueClassTransition*,t_0,(((RogueClassTransition*)(_auto_1700_0->data->as_objects[_auto_1701_0]))));
      #line 55
      if (ROGUE_COND(((t_0->character) == (ch_0))))
      {
        return (RogueClassScanState*)(t_0->target_state);
      }
    }
  }
  #line 57
  return (RogueClassScanState*)(((RogueClassScanState*)(NULL)));
}

#line 59
RogueClassScanState* RogueScanState__link__Character_Int32_Int32( RogueClassScanState* THIS, RogueCharacter ch_0, RogueInt32 type_1, RogueInt32 option_2 )
{
  ROGUE_GC_CHECK;
  #line 60
  return (RogueClassScanState*)(((RogueClassScanState*)(RogueScanState__link__Character_TransitionActions_Int32_Int32_Int32( ROGUE_ARG(THIS), ch_0, RogueClassTransitionActions( 0 ), 0, type_1, option_2 ))));
}

#line 62
RogueClassScanState* RogueScanState__link__Character_TransitionActions_Int32_Int32_Int32( RogueClassScanState* THIS, RogueCharacter ch_0, RogueClassTransitionActions action_1, RogueInt32 value_2, RogueInt32 type_3, RogueInt32 option_4 )
{
  ROGUE_GC_CHECK;
  #line 63
  ROGUE_DEF_LOCAL_REF(RogueClassScanState*,state_5,(((RogueClassScanState*)(RogueScanState__find_link__Character( ROGUE_ARG(THIS), ch_0 )))));
  #line 64
  if (ROGUE_COND(!!(state_5)))
  {
    #line 65
    state_5->_type |= type_3;
    #line 66
    state_5->option |= option_4;
    #line 67
    state_5->flags = ((RogueStateFlags__operatorOR__StateFlags( state_5->flags, THIS->flags )));
    #line 68
    return (RogueClassScanState*)(state_5);
  }
  else
  {
    #line 70
    return (RogueClassScanState*)(((RogueClassScanState*)(RogueScanState__new_link__Character_TransitionActions_Int32_Int32_Int32( ROGUE_ARG(THIS), ch_0, action_1, value_2, type_3, option_4 ))));
  }
}

#line 85
void RogueScanState__link_alpha__ScanState( RogueClassScanState* THIS, RogueClassScanState* target_state_0 )
{
  ROGUE_GC_CHECK;
  #line 86
  RogueTransition_List__set__Int32_Transition( ROGUE_ARG(THIS->transitions), 1, ROGUE_ARG(((RogueClassTransition*)(RogueTransition__init__Character_TransitionActions_Int32_ScanState( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassTransition*,ROGUE_CREATE_OBJECT(Transition))), (RogueCharacter)0, RogueClassTransitionActions( 0 ), 0, target_state_0 )))) );
}

#line 88
void RogueScanState__link_default__ScanState( RogueClassScanState* THIS, RogueClassScanState* target_state_0 )
{
  ROGUE_GC_CHECK;
  #line 89
  RogueTransition_List__set__Int32_Transition( ROGUE_ARG(THIS->transitions), 0, ROGUE_ARG(((RogueClassTransition*)(RogueTransition__init__Character_TransitionActions_Int32_ScanState( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassTransition*,ROGUE_CREATE_OBJECT(Transition))), (RogueCharacter)0, RogueClassTransitionActions( 0 ), 0, target_state_0 )))) );
}

#line 91
void RogueScanState__link_numeric__ScanState( RogueClassScanState* THIS, RogueClassScanState* target_state_0 )
{
  ROGUE_GC_CHECK;
  #line 92
  RogueTransition_List__set__Int32_Transition( ROGUE_ARG(THIS->transitions), 2, ROGUE_ARG(((RogueClassTransition*)(RogueTransition__init__Character_TransitionActions_Int32_ScanState( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassTransition*,ROGUE_CREATE_OBJECT(Transition))), (RogueCharacter)0, RogueClassTransitionActions( 0 ), 0, target_state_0 )))) );
}

#line 94
RogueClassScanState* RogueScanState__new_link__Character_TransitionActions_Int32_Int32_Int32( RogueClassScanState* THIS, RogueCharacter ch_0, RogueClassTransitionActions action_1, RogueInt32 value_2, RogueInt32 type_3, RogueInt32 option_4 )
{
  ROGUE_GC_CHECK;
  #line 95
  ROGUE_DEF_LOCAL_REF(RogueClassScanState*,state_5,(((RogueClassScanState*)(RogueScanState__init__Int32_Int32_StateFlags( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassScanState*,ROGUE_CREATE_OBJECT(ScanState))), type_3, option_4, RogueClassStateFlags( 0 ) )))));
  #line 96
  RogueTransition_List__add__Transition( ROGUE_ARG(THIS->transitions), ROGUE_ARG(((RogueClassTransition*)(RogueTransition__init__Character_TransitionActions_Int32_ScanState( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassTransition*,ROGUE_CREATE_OBJECT(Transition))), ch_0, action_1, value_2, state_5 )))) );
  #line 97
  return (RogueClassScanState*)(state_5);
}

#line 4 "../../Rogue/Programs/RogueC/Libraries/Standard/List.rogue"
RogueInt32_List* RogueInt32_List__init_object( RogueInt32_List* THIS )
{
  ROGUE_GC_CHECK;
  RogueGenericList__init_object( ROGUE_ARG(((RogueClassGenericList*)THIS)) );
  return (RogueInt32_List*)(THIS);
}

#line 10
RogueInt32_List* RogueInt32_List__init( RogueInt32_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 11
  RogueInt32_List__init__Int32( ROGUE_ARG(THIS), 0 );
  #line 13
  return (RogueInt32_List*)(THIS);
}

#line 633
RogueString* RogueInt32_List__to_String( RogueInt32_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 634
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 635
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)'[', true );
  #line 636
  RogueLogical first_1 = (true);
  #line 637
  {
    ROGUE_DEF_LOCAL_REF(RogueInt32_List*,_auto_1711_0,(THIS));
    RogueInt32 _auto_1712_0 = (0);
    RogueInt32 _auto_1713_0 = (((_auto_1711_0->count) - (1)));
    for (;ROGUE_COND(((_auto_1712_0) <= (_auto_1713_0)));++_auto_1712_0)
    {
      ROGUE_GC_CHECK;
      RogueInt32 value_0 = (_auto_1711_0->data->as_int32s[_auto_1712_0]);
      #line 638
      if (ROGUE_COND(first_1))
      {
        first_1 = ((RogueLogical)(false));
      }
      else
      {
        #line 639
        RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)',', true );
      }
      #line 640
      if (ROGUE_COND((false)))
      {
      }
      else
      {
        #line 641
        RogueStringBuilder__print__String( buffer_0, ROGUE_ARG(((RogueString*)(RogueInt32__to_String( value_0 )))) );
      }
    }
  }
  #line 643
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)']', true );
  #line 644
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_0 ))));
}

#line 4
RogueString* RogueInt32_List__type_name( RogueInt32_List* THIS )
{
  return (RogueString*)(Rogue_literal_strings[49]);
}

#line 13
RogueInt32_List* RogueInt32_List__init__Int32( RogueInt32_List* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 14
  if (ROGUE_COND(((initial_capacity_0) != (((RogueInt32_List__capacity( ROGUE_ARG(THIS) )))))))
  {
    THIS->data = RogueType_create_array( initial_capacity_0, sizeof(RogueInt32), false, 9 );
  }
  #line 15
  RogueInt32_List__clear( ROGUE_ARG(THIS) );
  #line 17
  return (RogueInt32_List*)(THIS);
}

#line 58
RogueInt32_List* RogueInt32_List__add__Int32( RogueInt32_List* THIS, RogueInt32 value_0 )
{
  ROGUE_GC_CHECK;
  #line 59
  RogueInt32_List__set__Int32_Int32( ROGUE_ARG(((RogueInt32_List*)(RogueInt32_List__reserve__Int32( ROGUE_ARG(THIS), 1 )))), ROGUE_ARG(THIS->count), value_0 );
  #line 60
  THIS->count = ((THIS->count) + (1));
  #line 61
  return (RogueInt32_List*)(THIS);
}

#line 68
RogueInt32 RogueInt32_List__capacity( RogueInt32_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 69
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    return (RogueInt32)(0);
  }
  #line 70
  return (RogueInt32)(THIS->data->count);
}

#line 72
RogueInt32_List* RogueInt32_List__clear( RogueInt32_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 73
  RogueInt32_List__discard_from__Int32( ROGUE_ARG(THIS), 0 );
  #line 74
  return (RogueInt32_List*)(THIS);
}

#line 174
RogueInt32_List* RogueInt32_List__discard_from__Int32( RogueInt32_List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 175
  if (ROGUE_COND(!!(THIS->data)))
  {
    #line 176
    RogueInt32 n_1 = (((THIS->count) - (index_0)));
    #line 177
    if (ROGUE_COND(((n_1) > (0))))
    {
      #line 178
      RogueArray__zero__Int32_Int32( ROGUE_ARG(((RogueArray*)THIS->data)), index_0, n_1 );
      #line 179
      THIS->count = index_0;
    }
  }
  #line 182
  return (RogueInt32_List*)(THIS);
}

#line 467
RogueInt32_List* RogueInt32_List__reserve__Int32( RogueInt32_List* THIS, RogueInt32 additional_elements_0 )
{
  ROGUE_GC_CHECK;
  #line 468
  RogueInt32 required_capacity_1 = (((THIS->count) + (additional_elements_0)));
  #line 469
  if (ROGUE_COND(((required_capacity_1) == (0))))
  {
    return (RogueInt32_List*)(THIS);
  }
  #line 471
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    #line 472
    if (ROGUE_COND(((required_capacity_1) == (1))))
    {
      required_capacity_1 = ((RogueInt32)(10));
    }
    #line 473
    THIS->data = RogueType_create_array( required_capacity_1, sizeof(RogueInt32), false, 9 );
  }
  else if (ROGUE_COND(((required_capacity_1) > (THIS->data->count))))
  {
    #line 475
    RogueInt32 cap_2 = (((RogueInt32_List__capacity( ROGUE_ARG(THIS) ))));
    #line 476
    if (ROGUE_COND(((required_capacity_1) < (((cap_2) + (cap_2))))))
    {
      required_capacity_1 = ((RogueInt32)(((cap_2) + (cap_2))));
    }
    #line 477
    ROGUE_DEF_LOCAL_REF(RogueArray*,new_data_3,(RogueType_create_array( required_capacity_1, sizeof(RogueInt32), false, 9 )));
    #line 478
    RogueArray_set(new_data_3,0,((RogueArray*)(THIS->data)),0,-1);
    #line 479
    THIS->data = new_data_3;
  }
  #line 482
  return (RogueInt32_List*)(THIS);
}

#line 562
void RogueInt32_List__set__Int32_Int32( RogueInt32_List* THIS, RogueInt32 index_0, RogueInt32 new_value_1 )
{
  ROGUE_GC_CHECK;
  #line 563
  THIS->data->as_int32s[index_0] = (new_value_1);
}

#line 18 "../../Rogue/Programs/RogueC/Libraries/Standard/Array.rogue"
RogueString* RogueArray_Int32___type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[56]);
}

#line 4 "../../Rogue/Programs/RogueC/Libraries/Standard/List.rogue"
RogueTransition_List* RogueTransition_List__init_object( RogueTransition_List* THIS )
{
  ROGUE_GC_CHECK;
  RogueGenericList__init_object( ROGUE_ARG(((RogueClassGenericList*)THIS)) );
  return (RogueTransition_List*)(THIS);
}

#line 10
RogueTransition_List* RogueTransition_List__init( RogueTransition_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 11
  RogueTransition_List__init__Int32( ROGUE_ARG(THIS), 0 );
  #line 13
  return (RogueTransition_List*)(THIS);
}

#line 633
RogueString* RogueTransition_List__to_String( RogueTransition_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 634
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,buffer_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 635
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)'[', true );
  #line 636
  RogueLogical first_1 = (true);
  #line 637
  {
    ROGUE_DEF_LOCAL_REF(RogueTransition_List*,_auto_1782_0,(THIS));
    RogueInt32 _auto_1783_0 = (0);
    RogueInt32 _auto_1784_0 = (((_auto_1782_0->count) - (1)));
    for (;ROGUE_COND(((_auto_1783_0) <= (_auto_1784_0)));++_auto_1783_0)
    {
      ROGUE_GC_CHECK;
      ROGUE_DEF_LOCAL_REF(RogueClassTransition*,value_0,(((RogueClassTransition*)(_auto_1782_0->data->as_objects[_auto_1783_0]))));
      #line 638
      if (ROGUE_COND(first_1))
      {
        first_1 = ((RogueLogical)(false));
      }
      else
      {
        #line 639
        RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)',', true );
      }
      #line 640
      if (ROGUE_COND(((void*)value_0) == ((void*)NULL)))
      {
        RogueStringBuilder__print__String( buffer_0, Rogue_literal_strings[1] );
      }
      else
      {
        #line 641
        RogueStringBuilder__print__String( buffer_0, ROGUE_ARG(((RogueString*)Rogue_call_ROGUEM0( 6, ((RogueObject*)value_0) ))) );
      }
    }
  }
  #line 643
  RogueStringBuilder__print__Character_Logical( buffer_0, (RogueCharacter)']', true );
  #line 644
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( buffer_0 ))));
}

#line 4
RogueString* RogueTransition_List__type_name( RogueTransition_List* THIS )
{
  return (RogueString*)(Rogue_literal_strings[50]);
}

#line 13
RogueTransition_List* RogueTransition_List__init__Int32( RogueTransition_List* THIS, RogueInt32 initial_capacity_0 )
{
  ROGUE_GC_CHECK;
  #line 14
  if (ROGUE_COND(((initial_capacity_0) != (((RogueTransition_List__capacity( ROGUE_ARG(THIS) )))))))
  {
    THIS->data = RogueType_create_array( initial_capacity_0, sizeof(RogueClassTransition*), true, 38 );
  }
  #line 15
  RogueTransition_List__clear( ROGUE_ARG(THIS) );
  #line 17
  return (RogueTransition_List*)(THIS);
}

#line 58
RogueTransition_List* RogueTransition_List__add__Transition( RogueTransition_List* THIS, RogueClassTransition* value_0 )
{
  ROGUE_GC_CHECK;
  #line 59
  RogueTransition_List__set__Int32_Transition( ROGUE_ARG(((RogueTransition_List*)(RogueTransition_List__reserve__Int32( ROGUE_ARG(THIS), 1 )))), ROGUE_ARG(THIS->count), value_0 );
  #line 60
  THIS->count = ((THIS->count) + (1));
  #line 61
  return (RogueTransition_List*)(THIS);
}

#line 68
RogueInt32 RogueTransition_List__capacity( RogueTransition_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 69
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    return (RogueInt32)(0);
  }
  #line 70
  return (RogueInt32)(THIS->data->count);
}

#line 72
RogueTransition_List* RogueTransition_List__clear( RogueTransition_List* THIS )
{
  ROGUE_GC_CHECK;
  #line 73
  RogueTransition_List__discard_from__Int32( ROGUE_ARG(THIS), 0 );
  #line 74
  return (RogueTransition_List*)(THIS);
}

#line 174
RogueTransition_List* RogueTransition_List__discard_from__Int32( RogueTransition_List* THIS, RogueInt32 index_0 )
{
  ROGUE_GC_CHECK;
  #line 175
  if (ROGUE_COND(!!(THIS->data)))
  {
    #line 176
    RogueInt32 n_1 = (((THIS->count) - (index_0)));
    #line 177
    if (ROGUE_COND(((n_1) > (0))))
    {
      #line 178
      RogueArray__zero__Int32_Int32( ROGUE_ARG(((RogueArray*)THIS->data)), index_0, n_1 );
      #line 179
      THIS->count = index_0;
    }
  }
  #line 182
  return (RogueTransition_List*)(THIS);
}

#line 467
RogueTransition_List* RogueTransition_List__reserve__Int32( RogueTransition_List* THIS, RogueInt32 additional_elements_0 )
{
  ROGUE_GC_CHECK;
  #line 468
  RogueInt32 required_capacity_1 = (((THIS->count) + (additional_elements_0)));
  #line 469
  if (ROGUE_COND(((required_capacity_1) == (0))))
  {
    return (RogueTransition_List*)(THIS);
  }
  #line 471
  if (ROGUE_COND(!(!!(THIS->data))))
  {
    #line 472
    if (ROGUE_COND(((required_capacity_1) == (1))))
    {
      required_capacity_1 = ((RogueInt32)(10));
    }
    #line 473
    THIS->data = RogueType_create_array( required_capacity_1, sizeof(RogueClassTransition*), true, 38 );
  }
  else if (ROGUE_COND(((required_capacity_1) > (THIS->data->count))))
  {
    #line 475
    RogueInt32 cap_2 = (((RogueTransition_List__capacity( ROGUE_ARG(THIS) ))));
    #line 476
    if (ROGUE_COND(((required_capacity_1) < (((cap_2) + (cap_2))))))
    {
      required_capacity_1 = ((RogueInt32)(((cap_2) + (cap_2))));
    }
    #line 477
    ROGUE_DEF_LOCAL_REF(RogueArray*,new_data_3,(RogueType_create_array( required_capacity_1, sizeof(RogueClassTransition*), true, 38 )));
    #line 478
    RogueArray_set(new_data_3,0,((RogueArray*)(THIS->data)),0,-1);
    #line 479
    THIS->data = new_data_3;
  }
  #line 482
  return (RogueTransition_List*)(THIS);
}

#line 562
void RogueTransition_List__set__Int32_Transition( RogueTransition_List* THIS, RogueInt32 index_0, RogueClassTransition* new_value_1 )
{
  ROGUE_GC_CHECK;
  #line 563
  THIS->data->as_objects[index_0] = new_value_1;
}

#line 19 "../ScanTableBuilder.rogue"
RogueClassTransition* RogueTransition__init_object( RogueClassTransition* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueClassTransition*)(THIS);
}

#line 21
RogueClassTransition* RogueTransition__init( RogueClassTransition* THIS )
{
  ROGUE_GC_CHECK;
  return (RogueClassTransition*)(THIS);
}

#line 19
RogueString* RogueTransition__type_name( RogueClassTransition* THIS )
{
  return (RogueString*)(Rogue_literal_strings[43]);
}

RogueClassTransition* RogueTransition__init__Character_TransitionActions_Int32_ScanState( RogueClassTransition* THIS, RogueCharacter _auto_639_0, RogueClassTransitionActions _auto_640_1, RogueInt32 _auto_641_2, RogueClassScanState* _auto_642_3 )
{
  ROGUE_GC_CHECK;
  THIS->target_state = _auto_642_3;
  THIS->value = _auto_641_2;
  THIS->actions = _auto_640_1;
  THIS->character = _auto_639_0;
  return (RogueClassTransition*)(THIS);
}

#line 18 "../../Rogue/Programs/RogueC/Libraries/Standard/Array.rogue"
RogueString* RogueArray_Transition___type_name( RogueArray* THIS )
{
  return (RogueString*)(Rogue_literal_strings[57]);
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/Runtime.rogue"
RogueClassRuntime* RogueRuntime__init_object( RogueClassRuntime* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueClassRuntime*)(THIS);
}

RogueString* RogueRuntime__type_name( RogueClassRuntime* THIS )
{
  return (RogueString*)(Rogue_literal_strings[38]);
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/System.rogue"
RogueClassSystem* RogueSystem__init_object( RogueClassSystem* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueClassSystem*)(THIS);
}

RogueString* RogueSystem__type_name( RogueClassSystem* THIS )
{
  return (RogueString*)(Rogue_literal_strings[39]);
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/WeakReference.rogue"
RogueWeakReference* RogueWeakReference__init_object( RogueWeakReference* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  return (RogueWeakReference*)(THIS);
}

RogueString* RogueWeakReference__type_name( RogueWeakReference* THIS )
{
  return (RogueString*)(Rogue_literal_strings[40]);
}

#line 12
void RogueWeakReference__on_cleanup( RogueWeakReference* THIS )
{
  ROGUE_GC_CHECK;
  #line 13
  if (Rogue_weak_references == THIS)
  {
    Rogue_weak_references = THIS->next_weak_reference;
  }
  else
  {
    RogueWeakReference* cur = Rogue_weak_references;
    while (cur && cur->next_weak_reference != THIS)
    {
      cur = cur->next_weak_reference;
    }
    if (cur) cur->next_weak_reference = cur->next_weak_reference->next_weak_reference;
  }

}

#line 76 "../../Rogue/Programs/RogueC/Libraries/Standard/Console.rogue"
RogueClassFunction_1670* RogueFunction_1670__init_object( RogueClassFunction_1670* THIS )
{
  ROGUE_GC_CHECK;
  Rogue_Function_____init_object( ROGUE_ARG(((RogueClass_Function___*)THIS)) );
  return (RogueClassFunction_1670*)(THIS);
}

RogueString* RogueFunction_1670__type_name( RogueClassFunction_1670* THIS )
{
  return (RogueString*)(Rogue_literal_strings[59]);
}

void RogueFunction_1670__call( RogueClassFunction_1670* THIS )
{
  RogueConsole__reset_input_mode( ROGUE_ARG(THIS->console) );
}

RogueClassFunction_1670* RogueFunction_1670__init__Console( RogueClassFunction_1670* THIS, RogueClassConsole* _auto_1671_0 )
{
  ROGUE_GC_CHECK;
  THIS->console = _auto_1671_0;
  return (RogueClassFunction_1670*)(THIS);
}

#line 3 "../../Rogue/Programs/RogueC/Libraries/Standard/Object.rogue"
void RogueObject__init_object( RogueObject* THIS )
{
  ROGUE_GC_CHECK;
}

#line 6
RogueObject* RogueObject__init( RogueObject* THIS )
{
  ROGUE_GC_CHECK;
  #line 9
  return (RogueObject*)(THIS);
}

#line 15
RogueInt64 RogueObject__object_id( RogueObject* THIS )
{
  ROGUE_GC_CHECK;
  #line 16
  RogueInt64 addr_0 = 0;
  #line 17
  addr_0 = (RogueInt64)(intptr_t)THIS;

  #line 18
  return (RogueInt64)(addr_0);
}

#line 23
RogueString* RogueObject__to_String( RogueObject* THIS )
{
  ROGUE_GC_CHECK;
  #line 24
  return (RogueString*)(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), Rogue_literal_strings[6] )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)Rogue_call_ROGUEM0( 12, ROGUE_ARG(THIS) ))) ))) )))), Rogue_literal_strings[8] )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueInt64__to_hex_string__Int32( ROGUE_ARG(((RogueObject__object_id( ROGUE_ARG(THIS) )))), 16 )))) ))) )))), Rogue_literal_strings[9] )))) ))));
}

#line 1
RogueString* RogueObject__type_name( RogueObject* THIS )
{
  return (RogueString*)(Rogue_literal_strings[7]);
}

#line 1 "../../Rogue/Programs/RogueC/Libraries/Standard/Exception.rogue"
RogueException* RogueException__init_object( RogueException* THIS )
{
  ROGUE_GC_CHECK;
  RogueObject__init_object( ROGUE_ARG(((RogueObject*)THIS)) );
  #line 4
  THIS->stack_trace = ((RogueClassStackTrace*)(RogueStackTrace__init__Int32( ROGUE_ARG(ROGUE_CREATE_REF(RogueClassStackTrace*,ROGUE_CREATE_OBJECT(StackTrace))), 1 )));
  #line 1
  return (RogueException*)(THIS);
}

#line 9
RogueException* RogueException__init( RogueException* THIS )
{
  ROGUE_GC_CHECK;
  #line 10
  THIS->message = ((RogueString*)Rogue_call_ROGUEM0( 12, ROGUE_ARG(THIS) ));
  #line 12
  return (RogueException*)(THIS);
}

#line 24
RogueString* RogueException__to_String( RogueException* THIS )
{
  ROGUE_GC_CHECK;
  #line 25
  return (RogueString*)(THIS->message);
}

#line 1
RogueString* RogueException__type_name( RogueException* THIS )
{
  return (RogueString*)(Rogue_literal_strings[2]);
}

#line 7
RogueException* RogueException__init__String( RogueException* THIS, RogueString* _auto_226_0 )
{
  ROGUE_GC_CHECK;
  THIS->message = _auto_226_0;
  #line 9
  return (RogueException*)(THIS);
}

#line 12
void RogueException__display( RogueException* THIS )
{
  ROGUE_GC_CHECK;
  #line 13
  ROGUE_DEF_LOCAL_REF(RogueStringBuilder*,builder_0,(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))));
  #line 14
  RogueStringBuilder__println__String( builder_0, ROGUE_ARG((RogueString__operatorTIMES__String_Int32( Rogue_literal_strings[3], ROGUE_ARG(((RogueInt32__or_smaller__Int32( ROGUE_ARG(((((RogueConsole__width( ((RogueClassConsole*)ROGUE_SINGLETON(Console)) )))) - (1))), 79 )))) ))) );
  #line 15
  RogueStringBuilder__println__String( builder_0, ROGUE_ARG(((RogueString*)Rogue_call_ROGUEM0( 12, ROGUE_ARG(THIS) ))) );
  #line 16
  RogueStringBuilder__println__String( builder_0, ROGUE_ARG(((RogueString*)(RogueString_List__join__String( ROGUE_ARG(((RogueString_List*)(RogueString__word_wrap__Int32_String( ROGUE_ARG(THIS->message), 79, Rogue_literal_strings[4] )))), Rogue_literal_strings[5] )))) );
  #line 17
  if (ROGUE_COND(!!(THIS->stack_trace->entries->count)))
  {
    #line 18
    RogueStringBuilder__println( builder_0 );
    #line 19
    RogueStringBuilder__println__String( builder_0, ROGUE_ARG(((RogueString*)(RogueString__trimmed( ROGUE_ARG(((RogueString*)(RogueStackTrace__to_String( ROGUE_ARG(THIS->stack_trace) )))) )))) );
  }
  #line 21
  RogueStringBuilder__println__String( builder_0, ROGUE_ARG((RogueString__operatorTIMES__String_Int32( Rogue_literal_strings[3], ROGUE_ARG(((RogueInt32__or_smaller__Int32( ROGUE_ARG(((((RogueConsole__width( ((RogueClassConsole*)ROGUE_SINGLETON(Console)) )))) - (1))), 79 )))) ))) );
  #line 22
  RogueGlobal__println__Object( ((RogueClassGlobal*)ROGUE_SINGLETON(Global)), ROGUE_ARG(((RogueObject*)(builder_0))) );
}

#line 27
RogueString* RogueException__format( RogueException* THIS )
{
  ROGUE_GC_CHECK;
  #line 28
  ROGUE_DEF_LOCAL_REF(RogueString*,st_0,(((RogueString*)(RogueStackTrace__to_String( ROGUE_ARG(THIS->stack_trace) )))));
  #line 29
  if (ROGUE_COND(((void*)st_0) == ((void*)NULL)))
  {
    st_0 = ((RogueString*)(Rogue_literal_strings[10]));
  }
  #line 30
  st_0 = ((RogueString*)(((RogueString*)(RogueString__trimmed( st_0 )))));
  #line 31
  if (ROGUE_COND(!!(((RogueString__count( st_0 ))))))
  {
    st_0 = ((RogueString*)((RogueString__operatorPLUS__String_String( Rogue_literal_strings[5], st_0 ))));
  }
  #line 32
  return (RogueString*)((RogueString__operatorPLUS__String_String( ROGUE_ARG(((((THIS))) ? (ROGUE_ARG(((RogueString*)(RogueException__to_String( ROGUE_ARG(THIS) ))))) : ROGUE_ARG(Rogue_literal_strings[11]))), st_0 )));
}

#line 42
RogueClassError* RogueError__init_object( RogueClassError* THIS )
{
  ROGUE_GC_CHECK;
  RogueException__init_object( ROGUE_ARG(((RogueException*)THIS)) );
  return (RogueClassError*)(THIS);
}

RogueString* RogueError__type_name( RogueClassError* THIS )
{
  return (RogueString*)(Rogue_literal_strings[60]);
}

#line 67
RogueClassOutOfBoundsError* RogueOutOfBoundsError__init_object( RogueClassOutOfBoundsError* THIS )
{
  ROGUE_GC_CHECK;
  RogueError__init_object( ROGUE_ARG(((RogueClassError*)THIS)) );
  return (RogueClassOutOfBoundsError*)(THIS);
}

RogueString* RogueOutOfBoundsError__type_name( RogueClassOutOfBoundsError* THIS )
{
  return (RogueString*)(Rogue_literal_strings[62]);
}

#line 69
RogueClassOutOfBoundsError* RogueOutOfBoundsError__init__String( RogueClassOutOfBoundsError* THIS, RogueString* _auto_721_0 )
{
  ROGUE_GC_CHECK;
  THIS->message = _auto_721_0;
  #line 71
  return (RogueClassOutOfBoundsError*)(THIS);
}

#line 34
RogueClassOutOfBoundsError* RogueOutOfBoundsError___throw( RogueClassOutOfBoundsError* THIS )
{
  ROGUE_GC_CHECK;
  #line 38
  throw THIS;

}

#line 71
RogueClassOutOfBoundsError* RogueOutOfBoundsError__init__Int32_Int32( RogueClassOutOfBoundsError* THIS, RogueInt32 index_0, RogueInt32 limit_1 )
{
  ROGUE_GC_CHECK;
  #line 72
  switch (limit_1)
  {
    case 0:
    {
      #line 73
      RogueOutOfBoundsError__init__String( ROGUE_ARG(THIS), ROGUE_ARG(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), Rogue_literal_strings[16] )))), ROGUE_ARG(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), index_0 )))) )))), Rogue_literal_strings[17] )))) )))) );
      break;
    }
    case 1:
    {
      #line 74
      RogueOutOfBoundsError__init__String( ROGUE_ARG(THIS), ROGUE_ARG(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), Rogue_literal_strings[16] )))), ROGUE_ARG(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), index_0 )))) )))), Rogue_literal_strings[18] )))) )))) );
      break;
    }
    default:
    {
      #line 75
      RogueOutOfBoundsError__init__String( ROGUE_ARG(THIS), ROGUE_ARG(((RogueString*)(RogueStringBuilder__to_String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__print__String( ROGUE_ARG(((RogueStringBuilder*)(RogueStringBuilder__init( ROGUE_ARG(ROGUE_CREATE_REF(RogueStringBuilder*,ROGUE_CREATE_OBJECT(StringBuilder))) )))), Rogue_literal_strings[16] )))), ROGUE_ARG(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), index_0 )))) )))), Rogue_literal_strings[19] )))), ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], ROGUE_ARG(((RogueString*)(RogueString__pluralized__Int32( Rogue_literal_strings[20], limit_1 )))) ))) )))), Rogue_literal_strings[24] )))), ROGUE_ARG(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), 0 )))) )))), Rogue_literal_strings[25] )))), ROGUE_ARG(((RogueString*)(RogueString__operatorPLUS__Int32( ROGUE_ARG((RogueString__operatorPLUS__String_String( Rogue_literal_strings[0], Rogue_literal_strings[0] ))), ROGUE_ARG(((limit_1) - (1))) )))) )))), Rogue_literal_strings[26] )))) )))) );
    }
  }
  #line 76
  return (RogueClassOutOfBoundsError*)(THIS);
}

#line 3 "../ScanTableBuilder.rogue"
RogueClassStateFlags RogueStateFlags__operatorOR__StateFlags( RogueClassStateFlags THIS, RogueClassStateFlags other_0 )
{
  return (RogueClassStateFlags)(RogueClassStateFlags( ((THIS.value) | (other_0.value)) ));
}

#line 120 "../../Rogue/Programs/RogueC/Libraries/Standard/System.rogue"
RogueString* RogueSystemEnvironment__get__String( RogueClassSystemEnvironment THIS, RogueString* name_0 )
{
  ROGUE_GC_CHECK;
  #line 121
  ROGUE_DEF_LOCAL_REF(RogueString*,result_1,0);
  #line 123
  char* c_result = getenv( (char*)name_0->utf8 );
  if (c_result)
  {
    result_1 = RogueString_create_from_utf8( c_result );
  }

  #line 129
  return (RogueString*)(((((result_1))) ? (ROGUE_ARG(result_1)) : ROGUE_ARG(((RogueString*)(NULL)))));
}

#line 3 "../Boss.rogue"
RogueInt32 RogueTokenType__to_Int32( RogueClassTokenType THIS )
{
  return (RogueInt32)(THIS.value);
}


void Rogue_configure( int argc, const char* argv[] )
{
  if (Rogue_configured) return;
  Rogue_configured = 1;

  Rogue_argc = argc;
  Rogue_argv = argv;

  Rogue_thread_register();
  Rogue_configure_gc();
  Rogue_configure_types();
  set_terminate( Rogue_terminate_handler );
  for (int i=0; i<Rogue_accessible_type_count; ++i)
  {
    *(Rogue_accessible_type_pointers[i]) = &Rogue_types[Rogue_accessible_type_indices[i]];
  }

  for (int i=0; i<Rogue_literal_string_count; ++i)
  {
    Rogue_define_literal_string( i, Rogue_literal_c_strings[i], -1 );
  }

}

void Rogue_init_thread()
{
  ROGUE_THREAD_LOCALS_INIT(RogueStringBuilder_work_bytes, RogueStringBuilder_pool);
  RogueStringBuilder__init_class_thread_local();
}

void Rogue_deinit_thread()
{
  ROGUE_THREAD_LOCALS_DEINIT(RogueStringBuilder_work_bytes, RogueStringBuilder_pool);
}

#ifdef ROGUE_AUTO_LAUNCH
__attribute__((constructor))
#endif
void Rogue_launch()
{
  Rogue_configure(0, NULL);
  RogueRuntime__init_class();
  RogueSystem__init_class();
  RogueStateFlags__init_class();
  RogueTransitionActions__init_class();
  RogueTokenType__init_class();

  Rogue_init_thread();

  #ifdef ROGUE_PLATFORM_WINDOWS
  char executable_filepath_buffer[ 2048 ];
  if ( !GetModuleFileName( NULL, executable_filepath_buffer, 2048 ) )
  {
    strcpy( executable_filepath_buffer, "rogue.exe" );
  }
  RogueSystem_executable_filepath = RogueString_create_from_utf8( executable_filepath_buffer, -1 );
  #else
  RogueSystem_executable_filepath = RogueString_create_from_utf8(
      Rogue_argc ? Rogue_argv[0] : "Rogue", -1 );
  #endif

  for (int i=1; i<Rogue_argc; ++i)
  {
    RogueString_List__add__String( RogueSystem_command_line_arguments,
        RogueString_create_from_utf8( Rogue_argv[i], -1 ) );
  }

  // Instantiate essential singletons
  ROGUE_SINGLETON( Global );

  RogueGlobal__on_launch( (RogueClassGlobal*) (ROGUE_SINGLETON(Global)) );
  Rogue_collect_garbage();
}

bool Rogue_update_tasks()
{
  // Returns true if any tasks are still active
  try
  {
    Rogue_collect_garbage();
    return false;
  }
  catch (RogueException* err)
  {
    printf( "Uncaught exception\n" );
    RogueException__display( err );
    return false;
  }
}

int main( int argc, const char* argv[] )
{
  try
  {
    Rogue_configure( argc, argv );
    Rogue_launch();

    while (Rogue_update_tasks()) {}

    Rogue_quit();
  }
  catch (RogueException* err)
  {
    printf( "Uncaught exception\n" );
    RogueException__display( err );
  }

  return 0;
}
