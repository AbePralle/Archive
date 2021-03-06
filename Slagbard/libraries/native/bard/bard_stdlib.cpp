//=============================================================================
//  bard_stdlib.cpp
//
//  Bard Virtual Machine - functions for native I/O.
//
//  $(BARD_VERSION)
//  ---------------------------------------------------------------------------
//
//  Copyright 2008-2011 Plasmaworks LLC
//
//  Licensed under the Apache License, Version 2.0 (the "License"); 
//  you may not use this file except in compliance with the License. 
//  You may obtain a copy of the License at 
//
//    http://www.apache.org/licenses/LICENSE-2.0 
//
//  Unless required by applicable law or agreed to in writing, 
//  software distributed under the License is distributed on an 
//  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
//  either express or implied. See the License for the specific 
//  language governing permissions and limitations under the License.
//
//  ---------------------------------------------------------------------------
//
//  History:
//    2008.12.27 / Abe Pralle - Created
//    2010.12.10 / Abe Pralle - Revamped for Bard VM 3.2
//=============================================================================

#include "bard.h"

#if !defined(RVL)
#  include <sys/types.h> 
#endif

#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#if defined(RVL)
#  include <revolution/os.h>
#  include <time.h>
#  include <unistd.h>
#elif !defined(_WIN32)
#  include <sys/time.h>
#  include <unistd.h>
#  include <signal.h>
#  include <dirent.h>
#  include <sys/socket.h>
#  include <sys/uio.h>
#  include <sys/stat.h>
#  include <netdb.h>
#  include <errno.h>
#  include <pthread.h>
#endif

#if defined(ANDROID)
#  include <netinet/in.h>
#endif

#if defined(_WIN32)
#  include <direct.h>
#  define chdir _chdir
#endif

#if TARGET_OS_IPHONE 
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif

/*
BardReal64 Bard_pi;
*/

#define BVM_FILE_BUFFER_SIZE 512

struct BardFileInfo : BardResource
{
  int     remaining;
  FILE*   fp;

  BardFileInfo( FILE* fp, int remaining )
  {
    this->fp = fp;
    this->remaining = remaining;
  }

  ~BardFileInfo()
  {
    if (fp)
    {
      fflush( fp );
      fclose( fp );
      fp = NULL;
    }
  }
};

/*
//-----------------------------------------------------------------------------
//  Introspection call stacks
//-----------------------------------------------------------------------------
#define INTROSPECTION_MAX_REF_STACK_SIZE 16
#define INTROSPECTION_MAX_DATA_STACK_SIZE 16*8
BardGlobalRef introspection_ref_stack[INTROSPECTION_MAX_REF_STACK_SIZE];
int   introspection_ref_stack_count = 0;
char  introspection_data_stack[INTROSPECTION_MAX_DATA_STACK_SIZE];
char* introspection_data_stack_ptr = introspection_data_stack + INTROSPECTION_MAX_DATA_STACK_SIZE;
int   introspection_data_stack_count = 0;
*/


//-----------------------------------------------------------------------------
//  Native Sockets
//-----------------------------------------------------------------------------
#define SOCKET_BUFFER_SIZE 1024

struct GenericSocketInfo : BardResource
{
  int  read_pos;
  int  write_pos;
  int  read_buffer_count;

  bool okay;

  bool resolved;
  bool closed;
  char read_buffer[SOCKET_BUFFER_SIZE];
  char write_buffer[SOCKET_BUFFER_SIZE];


  char remote_ip[NI_MAXHOST];

  GenericSocketInfo()
  {
    init();
  }

  void init()
  {
    closed = false;
    read_pos = 0;
    write_pos = 0;
    read_buffer_count = 0;
    okay = false;
    resolved = false;
    strcpy( remote_ip, "0.0.0.0" );
  }

  virtual ~GenericSocketInfo()
  {
  }

  virtual int status()
  {
    //   -1 - connection closed
    //    0 - never connected
    //    1 - connecting
    //    2 - connected
    return 0;
  }

  virtual int available()
  {
    return 0;
  }

  virtual int peek()
  {
    if (available() == 0) return -1;
    else return read_buffer[read_pos];
  }

  virtual int read()
  {
    if (available() == 0) return -1;
    else return read_buffer[read_pos++];
  }

  virtual bool write( int value )
  {
    return false;
  }
};

#if defined(UNIX)
struct SocketInfo : GenericSocketInfo
{
  int   sock;
  int   port;
  pthread_t thread;
  char* addr;

  SocketInfo( char* addr, int port )
  {
    init(addr,port);
  }

  void init( char* addr, int port )
  {
    GenericSocketInfo::init();

    int len = strlen(addr);
    this->addr = new char[len+1];
    strcpy( this->addr, addr );

    this->port = port;

    if (0 == pthread_create(&thread, NULL, establish_connection, this) )
    {
      // thread launched successfully
    }
    else
    {
      // error launching thread
      close(sock);
      sock = -1;
      resolved = true;
    }
  }


  static void* establish_connection( void* info );

  SocketInfo( int s )
  {
    init(s);
  }

  void init( int s )
  {
    GenericSocketInfo::init();
    sock = s;
    fcntl( sock, F_SETFL, O_NONBLOCK );
    okay = true;
    resolved = true;
    addr = NULL;
  }

  ~SocketInfo()
  {
    close_socket();
  }

  void close_socket()
  {
    if (addr)
    {
      delete addr;
      addr = NULL;
    }

    if (sock != -1)
    {
      close( sock );
      sock = -1;
    }
  }

  int status()
  {
    //   -1 - connection closed
    //    0 - never connected
    //    1 - connecting
    //    2 - connected
    if (resolved)
    {
      if (sock == -1) return 0;
      else if (closed == true) return -1;
      else return 2;
    }
    else
    {
      return 1;
    }
  }

  int available()
  {
    if ( !resolved ) return 0;

    if (read_pos < read_buffer_count) return (read_buffer_count - read_pos);

    read_pos = 0;
    read_buffer_count = ::read( sock, read_buffer, SOCKET_BUFFER_SIZE );

    if (read_buffer_count == -1)
    {
      if (errno != EAGAIN)
      {
        closed = true;
        okay = false;
      }
      return 0;
    }

    if (read_buffer_count == 0)
    {
      // end of file
      closed = true;
      okay = false;
      return 0;
    }

    return read_buffer_count;
  }

  bool write( int value )
  {
    if (write_pos == SOCKET_BUFFER_SIZE || !okay) 
    {
      closed = true;
      okay = false;
      return false;
    }

    write_buffer[write_pos++] = (char) value;

    int n = 0;
#if !defined(ANDROID)
    try
    {
      n = ::send( sock, write_buffer, write_pos, 0 );
    }
    catch (...)
    {
      closed = true;
      okay = false;
      return false;
    }
#else
    n = ::send( sock, write_buffer, write_pos, 0 );
#endif

    // couldn't write any this time?
    if (n == 0) return true;

    if (n > 0)
    {
      write_pos -= n;
      memmove( write_buffer, write_buffer+n, write_pos );
      return true;
    }
    else
    {
      if (errno == EAGAIN) return true;
      closed = true;
      okay = false;
      return false;
    }
  }

};

void* SocketInfo::establish_connection( void* this_ptr )
{
  SocketInfo* THIS = (SocketInfo*) this_ptr;

  THIS->sock = socket( AF_INET, SOCK_STREAM, 0 );
  if (THIS->sock == -1)
  {
    THIS->resolved = true;
    return 0;
  }

  char port_string[40];
  snprintf( port_string, sizeof(port_string), "%d", THIS->port );
  addrinfo requirements;
  memset( &requirements, 0, sizeof(requirements) );
  requirements.ai_family = AF_INET;
  requirements.ai_socktype = SOCK_STREAM;
  requirements.ai_protocol = IPPROTO_TCP;

  addrinfo *info, *cur;
  if (getaddrinfo( THIS->addr, port_string, NULL, &info ) != 0)
  {
    THIS->resolved = true;
    THIS->sock = -1;
    return 0;
  }

  cur = info;
  while (cur)
  {
    if (connect(THIS->sock,cur->ai_addr, cur->ai_addrlen) == 0)
    {
      THIS->okay = true;
      char sbuf[NI_MAXSERV];

      if (getnameinfo(cur->ai_addr, cur->ai_addrlen, THIS->remote_ip, 
            sizeof(THIS->remote_ip), sbuf, sizeof(sbuf), 
            NI_NUMERICHOST | NI_NUMERICSERV)) 
      {
        strcpy( THIS->remote_ip, "0.0.0.0" );
      }
      break;
    }
    cur = cur->ai_next;
  }
  freeaddrinfo( info );

  if (THIS->okay)
  {
    fcntl( THIS->sock, F_SETFL, O_NONBLOCK );
  }
  else
  {
    THIS->sock = -1;
  }

  THIS->resolved = true;
  return 0;
}

#elif defined(_WIN32)
DWORD WINAPI set_up_socket( LPVOID data );

struct SocketInfo : GenericSocketInfo
{
  SOCKET sock;
  HANDLE thread_handle;
  char*  address;
  int port;

  SocketInfo( char* addr, int port )
  {
    init(addr,port);
  }

  void init( char* addr, int port )
  {
    GenericSocketInfo::init();

    int len = strlen(addr);
    address = new char[len+1];
    strcpy_s( address, len+1, addr );
    this->port = port;

    thread_handle = CreateThread( NULL, 0, set_up_socket, this, 0, NULL );
    if (thread_handle == NULL)
    {
      closesocket( sock );
      sock = INVALID_SOCKET;
      resolved = true;
      return;
    }

  }

  SocketInfo( SOCKET s )
  {
    init(s);
  }

  void init( SOCKET s )
  {
    GenericSocketInfo::init();

    sock = s;
    okay = true;
    resolved = true;
    thread_handle = NULL;
    if (address) 
    {
      delete address;
      address = NULL;
    }
  }

  ~SocketInfo()
  {
    close_socket();
  }

  void close_socket()
  {
    if (thread_handle != NULL)
    {
      CloseHandle( thread_handle );
      thread_handle = NULL;
    }
    if (sock != INVALID_SOCKET)
    {
      closesocket( sock );
      sock = INVALID_SOCKET;
    }
  }

  int status()
  {
    //   -1 - connection closed
    //    0 - never connected
    //    1 - connecting
    //    2 - connected
    if (resolved)
    {
      if (sock == INVALID_SOCKET) return 0;
      else if (closed == true) return -1;
      else return 2;
    }
    else
    {
      return 1;
    }
  }

  int available()
  {
    if ( !resolved ) return 0;

    if (read_pos < read_buffer_count) return (read_buffer_count - read_pos);

    read_pos = 0;
    read_buffer_count = recv( sock, read_buffer, SOCKET_BUFFER_SIZE, 0 );

    if (read_buffer_count == SOCKET_ERROR)
    {
      if (WSAGetLastError() != WSAEWOULDBLOCK)
      {
        closed = true;
        okay = false;
      }
      return 0;
    }

    if (read_buffer_count == 0)
    {
      // end of file
      closed = true;
      okay = false;
      return 0;
    }

    return read_buffer_count;
  }

  bool write( int value )
  {
    if (write_pos == SOCKET_BUFFER_SIZE || !okay) 
    {
      closed = true;
      okay = false;
      return false;
    }

    write_buffer[write_pos++] = (char) value;

    int n = 0;
    try
    {
      n = send( sock, write_buffer, write_pos, 0 );
    }
    catch (...)
    {
      closed = true;
      okay = false;
      return false;
    }

    // couldn't write any this time?
    if (n == 0) return true;

    if (n > 0)
    {
      write_pos -= n;
      memmove( write_buffer, write_buffer+n, write_pos );
      return true;
    }
    else
    {
      if (WSAGetLastError() == WSAEWOULDBLOCK) return true;
      closed = true;
      okay = false;
      return false;
    }
  }

};

DWORD WINAPI set_up_socket( LPVOID data )
{
  SocketInfo* socket_info = (SocketInfo*) data;

  socket_info->sock = socket( AF_INET, SOCK_STREAM, 0 );

  if (socket_info->sock == INVALID_SOCKET)
  {
    socket_info->resolved = true;
    return 1;
  }

  char port_string[40];
  sprintf_s( port_string, sizeof(port_string), "%d", socket_info->port );

  // disable ipv6only
  //int ipv6only = 0;
  //if (SOCKET_ERROR == setsockopt( socket_info->sock, IPPROTO_IPV6,
        //IPV6_V6ONLY, (char*)&ipv6only, sizeof(ipv6only) ) )
  //{
      //closesocket(socket_info->sock);
      //socket_info->sock = INVALID_SOCKET;
      //return 1;
  //}

  addrinfo requirements;
  memset( &requirements, 0, sizeof(requirements) );
  requirements.ai_family = AF_INET;
  requirements.ai_socktype = SOCK_STREAM;
  requirements.ai_protocol = IPPROTO_TCP;

  addrinfo *info, *cur;
  if (getaddrinfo( socket_info->address, port_string, NULL, &info ) != 0)
  {
    closesocket(socket_info->sock);
    socket_info->sock = INVALID_SOCKET;
    socket_info->resolved = true;
    return 1;
  }

  cur = info;
  while (cur)
  {
    if (connect(socket_info->sock,cur->ai_addr, (int)cur->ai_addrlen) == 0)
    {
      socket_info->okay = true;

      char sbuf[NI_MAXSERV];

      if (getnameinfo(cur->ai_addr, (int) cur->ai_addrlen, 
            socket_info->remote_ip, sizeof(socket_info->remote_ip), sbuf,
          sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV)) 
      {
        strcpy_s( socket_info->remote_ip, NI_MAXSERV, "0.0.0.0" );
      }
      break;
    }
    cur = cur->ai_next;
  }
  freeaddrinfo( info );

  if (socket_info->okay)
  {
    u_long iMode = 1; // non-blocking
    ioctlsocket( socket_info->sock, FIONBIO, &iMode );
    socket_info->okay = true;
  }
  else
  {
    closesocket(socket_info->sock);
    socket_info->sock = INVALID_SOCKET;
    socket_info->resolved = true;
    return 1;
  }

  socket_info->resolved = true;

  return 0;
}

#else
  // Dummy socket implementation
  struct SocketInfo : GenericSocketInfo
  {
    SocketInfo( char* addr, int port ) : GenericSocketInfo()
    {
      fprintf( stder, "This platform does not support socket use.\n" );
    }

    SocketInfo( int s ) : GenericSocketInfo()
    {
      fprintf( stder, "This platform does not support socket use.\n" );
    }
  };
#endif


struct GenericServerSocketInfo : BardResource
{
  bool bound;

  GenericServerSocketInfo()
  {
    init();
  }

  void init()
  {
    bound = false;
  }

  virtual ~GenericServerSocketInfo()
  {
  }

  virtual int status()
  {
    //   -1 - could not create
    //    0 - connection closed
    //    1 - connected
    return -1;
  }

  virtual BardNativeData* accept_connection()
  {
    return 0;
  }

  virtual void close_socket()
  {
  }
};

#if defined(UNIX)
struct ServerSocketInfo : GenericServerSocketInfo
{
  int  sock;
  sockaddr_in address;
  socklen_t   address_size;
  

  ServerSocketInfo( int port )
  {
    init(port);
  }

  void init( int port )
  {
    GenericServerSocketInfo::init();

    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if (sock == -1) return;

    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
    address.sin_family = AF_INET;
    address_size = sizeof(address);

    // allow socket address to be quickly reused between program launches
    int opt = 1;
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) );

    if (bind(sock,(sockaddr*) &address, address_size) != 0) return;

    getsockname( sock, (sockaddr*) &address, &address_size );

    if (listen(sock,SOMAXCONN) != 0) return;

    bound = true;

    fcntl( sock, F_SETFL, O_NONBLOCK );

  }

  ~ServerSocketInfo()
  {
    close_socket();
  }

  int status()
  {
    //   -1 - could not bind
    //    0 - connection closed
    //    1 - connected
    if ( !bound ) return -1;
    if (sock == -1) return 0;
    return 1;
  }

  BardNativeData* accept_connection()
  {
    if (sock == -1 || !bound) return NULL;

    int s = accept(sock,(sockaddr*) &address, &address_size);
    if (s < 0) return NULL;

    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    if (getnameinfo((sockaddr*) &address, address_size, hbuf, sizeof(hbuf), sbuf,
        sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV)) 
    {
      // error getting IP address
      return NULL;
    }

    SocketInfo* client_socket = new SocketInfo(s);
    strcpy( client_socket->remote_ip, hbuf );
    return (BardNativeData*) BardNativeData::create( client_socket, BardNativeDataDeleteResource );
  }

  void close_socket() 
  {
    if (sock != -1)
    {
      close( sock );
      sock = -1;
    }
  }
};

#elif defined(_WIN32)
struct ServerSocketInfo : GenericServerSocketInfo
{
  SOCKET sock;
  sockaddr_in address;
  socklen_t   address_size;
  

  ServerSocketInfo( int port )
  {
    init(port);
  }

  void init( int port )
  {
    GenericServerSocketInfo::init();

    sock = socket( AF_INET, SOCK_STREAM, 0 );
    if (sock == INVALID_SOCKET) return;

    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( port );
    address.sin_family = AF_INET;
    address_size = sizeof(address);

    // allow socket address to be quickly reused between program launches
    int opt = 1;
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &opt, sizeof(opt) );

    if (bind(sock,(sockaddr*) &address, address_size) != 0) return;

    getsockname( sock, (sockaddr*) &address, &address_size );

    if (listen(sock,SOMAXCONN) != 0) return;

    bound = true;

    u_long iMode = 1; // non-blocking
    ioctlsocket( sock, FIONBIO, &iMode );
  }

  ~ServerSocketInfo()
  {
    close_socket();
  }

  int status()
  {
    //   -1 - could not bind
    //    0 - connection closed
    //    1 - connected
    if ( !bound ) return -1;
    if (sock == INVALID_SOCKET) return 0;
    return 1;
  }

  BardNativeData* accept_connection()
  {
    if (sock == INVALID_SOCKET || !bound) return NULL;

    SOCKET s = accept(sock,(sockaddr*) &address, &address_size);
    if (s == INVALID_SOCKET) return NULL;

    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

    if (getnameinfo((sockaddr*) &address, address_size, hbuf, sizeof(hbuf), sbuf,
        sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV)) 
    {
      // error getting IP address
      return NULL;
    }

    SocketInfo* client_socket = new SocketInfo(s);
    strcpy_s( client_socket->remote_ip, NI_MAXHOST, hbuf );
    return (BardNativeData*) BardNativeData::create( client_socket, BardNativeDataDeleteResource );
  }

  void close_socket() 
  {
    if (sock != INVALID_SOCKET)
    {
      closesocket( sock );
      sock = INVALID_SOCKET;
    }
  }
};

#else
// Dummy socket implementation
struct ServerSocketInfo : GenericServerSocketInfo
{
  ServerSocketInfo( int port ) : GenericServerSocketInfo()
  {
    fprintf( stderr, "This platform does not support server socket use.\n" );
  }
};
#endif



//-----------------------------------------------------------------------------
//  GenericArray
//-----------------------------------------------------------------------------
void GenericArray__clear__Int32_Int32()
{
  BardInt32 i2 = BARD_POP_INT32();
  BardInt32 i1 = BARD_POP_INT32();
  BardArray* array = (BardArray*) BARD_POP_REF();
  BVM_NULL_CHECK( array, return );

  if (i1 > i2) return;

#if defined(BARD_VM)
  if ( i1 < 0 || i2 >= array->array_count)
  {
    BVM_THROW_TYPE( bvm.type_invalid_operand_error, return );
  }
#endif

  int count = (i2 - i1) + 1;
  if (array->type->is_reference_array())
  {
    ++count;
    BardArray** cur = ((BardArray**) array->data) + i1 - 1;
    while (--count)
    {
      if (*(++cur))
      {
        --((*cur)->reference_count);
        *cur = NULL;
      }
    }
  }
  else
  {
    int b = array->type->element_size;
    memset( ((char*)array->data)+i1*b, 0, count*b );
  }
}

void GenericArray__count()
{
  BardArray* array = (BardArray*) BARD_POP_REF();
  BVM_NULL_CHECK( array, return );
  BARD_PUSH_INT32( array->array_count );
}

void GenericArray__copy_from__GenericArray_Int32_Int32_Int32()
{
  // method copy_from( GenericArray src_array, Int32 src_index, Int32 dest_index, 
  //                   Int32 number ) 
  BardInt32  count = BARD_POP_INT32(); 
  BardInt32  dest_index = BARD_POP_INT32(); 
  BardInt32  src_index = BARD_POP_INT32(); 
  BardArray* src_array = (BardArray*) BARD_POP_REF();
  BardArray* dest_array = (BardArray*) BARD_POP_REF();

  if ( !count ) return;

  BVM_NULL_CHECK( src_array && dest_array && count > 0, return );

  BardTypeInfo* src_type  = src_array->type;

  int b = src_type->element_size;
#if defined(BARD_VM)
  BardTypeInfo* dest_type = dest_array->type;

  if (src_type != dest_type)
  {
    // Allow as long as both are reference arrays or both are primitive
    // arrays with the same data size.
    if ( (src_type->is_reference_array() ^ dest_type->is_reference_array())
        || (b != dest_type->element_size) )
    {
      BVM_THROW_TYPE( bvm.type_invalid_operand_error, return );
    }
  }
#endif

#if defined(BARD_VM)
  int src_i2  = (src_index + count) - 1;
  int dest_i2 = (dest_index + count) - 1;

  if ( src_index < 0 || dest_index < 0
      || src_i2  >= src_array->array_count
      || dest_i2 >= dest_array->array_count)
  {
    BVM_THROW_TYPE( bvm.type_invalid_operand_error, return );
  }
#endif

  if (src_type->is_reference_array())
  {
    if (src_array == dest_array && dest_index > src_index)
    {
      // Handle overlapping regions
      BardObject** src_ptr  = (((BardObject**)src_array->data) + src_index) + count;
      BardObject** dest_ptr = (((BardObject**)dest_array->data) + dest_index) + count;

      ++count;
      while (--count)
      {
        if (*(--dest_ptr)) --((*dest_ptr)->reference_count);
        *dest_ptr = *(--src_ptr);
        if (*dest_ptr) ++((*dest_ptr)->reference_count);
      }
    }
    else
    {
      BardObject** src_ptr  = (((BardObject**)src_array->data) + src_index) - 1;
      BardObject** dest_ptr = (((BardObject**)dest_array->data) + dest_index) - 1;

      ++count;
      while (--count)
      {
        if (*(++dest_ptr)) --((*dest_ptr)->reference_count);
        *dest_ptr = *(++src_ptr);
        if (*dest_ptr) ++((*dest_ptr)->reference_count);
      }
    }
  }
  else
  {
    if (src_array == dest_array)
    {
      // memmove handles overlapping regions
      memmove( ((char*)dest_array->data) + dest_index*b, ((char*)src_array->data) + src_index*b, 
          count * b );
    }
    else
    {
      // memcpy is perhaps faster than memmove?
      memcpy( ((char*)dest_array->data) + dest_index*b, ((char*)src_array->data) + src_index*b, 
          count * b );
    }
  }
}


//--------------------------------------------------------------------
//  Date
//--------------------------------------------------------------------
void Date__ms_to_ymdhmsms( BardInt64 timestamp, 
    int* y, int* mo, int *d, int* h, int* m, int* s, int* ms )
{
  time_t cur_seconds = (time_t) (timestamp/1000);
  tm* date = localtime( &cur_seconds );
  *y  = date->tm_year + 1900;
  *mo = date->tm_mon + 1;
  *d  = date->tm_mday;
  *h  = date->tm_hour;
  *m  = date->tm_min;
  *s  = date->tm_sec;
  *ms = timestamp % 1000LL;
}

BardInt64 Date__ymdhms_to_ms( int y, int mo, int d, int h, int m, int s )
{
  // Date.timestamp().Int64
  time_t cur_seconds = 0;
  tm* zero_date = localtime( &cur_seconds );

  tm cur_date;
  memcpy( &cur_date, zero_date, sizeof(tm) );

  cur_date.tm_year = y - 1900;
  cur_date.tm_mon  = mo - 1;
  cur_date.tm_mday = d;
  cur_date.tm_hour = h;
  cur_date.tm_min  = m;
  cur_date.tm_sec  = s;
  cur_date.tm_isdst = -1;

  return ((BardInt64)mktime(&cur_date)) * 1000LL;
}

void Date__init__Int64()
{
  BardInt64 timestamp = BARD_POP_INT64();
  BardDate* context = (BardDate*) BARD_POP_REF();
  int y, mo, d, h, m, s, ms;
  Date__ms_to_ymdhmsms( timestamp, &y, &mo, &d, &h, &m, &s, &ms );
  context->year = y;
  context->month = mo;
  context->day = d;
  context->hour = h;
  context->minute = m;
  context->second = s;
  context->millisecond = ms;
}

void Date__timestamp()
{
  BardDate* context = (BardDate*) BARD_POP_REF();

  BARD_PUSH_INT64(
    Date__ymdhms_to_ms(
      context->year,
      context->month,
      context->day,
      context->hour,
      context->minute,
      context->second
    )
  );
}

//-----------------------------------------------------------------------------
//  File
//-----------------------------------------------------------------------------
#if !defined(ANDROID)
static int bard_is_directory( char* filename )
{
#if defined(_WIN32)
  int i = strlen(filename)-1;
  while (i > 0 && (filename[i] == '/' || filename[i] == '\\')) filename[i--] = 0;

  // Windows allows dir\* to count as a directory; guard against.
  for (i=0; filename[i]; ++i)
  {
    if (filename[i] == '*' || filename[i] == '?') return 0;
  }

  WIN32_FIND_DATA entry;
  HANDLE dir = FindFirstFile( filename, &entry );
  if (dir != INVALID_HANDLE_VALUE)
  {
    if (entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
      FindClose( dir );
      return 1;
    }
  }
  FindClose( dir );
  return 0;

#elif defined(UNIX)
  DIR* dir = opendir( filename );
  if (dir)
  {
    closedir( dir );
    return 1;
  }
  else
  {
    return 0;
  }

#elif defined(RVL)
  OSHalt( "ERROR: is_directory() not implemented on Wii\n" );
#else
  // not _WIN32 or UNIX
TODO: is_directory
#endif
}


BardLogical bard_file_exists( BardString* filepath )
{
  char filename[PATH_MAX];
  filepath->to_ascii( filename, PATH_MAX );
  if (bard_is_directory(filename)) return true;

  FILE* fp = fopen( filename, "rb" );
  if ( !fp ) return false;
  fclose(fp);
  return true;
}

BardLogical bard_file_is_directory( BardString* filepath )
{
  char filename[PATH_MAX];
  filepath->to_ascii( filename, PATH_MAX );
  return bard_is_directory( filename );
}

void bard_file_directory_listing( BardString* filepath, BardObject* list )
{
  // keep list accessible to gc
  BARD_PUSH_REF( list );

  char filename[PATH_MAX];
  filepath->to_ascii( filename, PATH_MAX );

  if ( !bard_is_directory(filename) )
  {
    bard_throw_file_error( filename );
    return;
  }

#if defined(_WIN32)
  {
    WIN32_FIND_DATA entry;
    HANDLE dir;
    
    // Add a \* to the end of the filename for windows.
    int len = strlen(filename);
    filename[len] = '\\';
    filename[len+1] = '*';
    filename[len+2] = 0;

    dir = FindFirstFile( filename, &entry );

    if (dir != INVALID_HANDLE_VALUE)
    {
      do
      {
        int keep = 1;
        if (entry.cFileName[0] == '.')
        {
          switch (entry.cFileName[1])
          {
            case 0:   
              keep = 0; 
              break;
            case '.':
              keep = entry.cFileName[2] != 0;
              break;
          }
        }
        if (keep)
        {
          BARD_PUSH_REF( list );
          BARD_PUSH_REF( (BardObject*) BardString::create(entry.cFileName) );
          BARD_CALL( list->type, "add(String)" );
          BARD_DISCARD_REF();  // return value
        }
      }
      while (FindNextFile(dir,&entry));

      FindClose( dir );
    }
  }
#elif defined(UNIX)
  {
    DIR* dir = opendir( filename );
    struct dirent* entry = readdir( dir );
    while (entry != NULL)
    {
      int keep = 1;
      if (entry->d_name[0] == '.')
      {
        switch (entry->d_name[1])
        {
          case 0:   
            keep = 0; 
            break;
          case '.':
            keep = entry->d_name[2] != 0;
            break;
        }
      }
      if (keep)
      {
        BARD_PUSH_REF( list );
        BARD_PUSH_REF( (BardObject*) BardString::create(entry->d_name) );
        BARD_CALL( list->type, "add(String)" );
        BARD_DISCARD_REF();  // return value
      }
      entry = readdir( dir );
    }
    closedir( dir );
  }
  
#elif defined(RVL)
  OSHalt( "ERROR: File::directory_listing() not implemented on Wii\n" );
#else
  TODO: file listing;
#endif

  BARD_DISCARD_REF();
}

BardString* bard_file_absolute_filepath( BardString* filepath_obj )
{
  char filepath[PATH_MAX+3];
  filepath_obj->to_ascii( filepath, PATH_MAX );

#if defined(_WIN32)
  {
    char long_name[PATH_MAX+4];
    char full_name[PATH_MAX+4];

    if (GetLongPathName(filepath, long_name, PATH_MAX+4) == 0)
    {
      strcpy_s( long_name, PATH_MAX+4, filepath );
    }

    if (GetFullPathName(long_name, PATH_MAX+4, full_name, 0) == 0)
    {
      // bail with name unchanged 
      return filepath_obj;
    }

    return BardString::create(full_name);
  }
  
#else
  {
    int original_dir_fd;
    int new_dir_fd;
    char filename[PATH_MAX];
    int is_dir = bard_is_directory(filepath);
    original_dir_fd = open( ".", O_RDONLY );

    if ( is_dir )
    {
      filename[0] = 0;
    }
    else
    {
      // fchdir only works with a path, not a path+filename (filepath)
      int i=strlen( filepath ) - 1;
      while (i >= 0 && filepath[i] != '/') --i;
      strcpy( filename, filepath+i+1 );
      filepath[i] = 0;
    }
    new_dir_fd = open( filepath, O_RDONLY );

    if (original_dir_fd >= 0 && new_dir_fd >= 0)
    {
      fchdir( new_dir_fd );
      getcwd( filepath, PATH_MAX );
      if ( !is_dir ) 
      {
        strcat( filepath, "/" );
        strcat( filepath, filename );
      }
      fchdir( original_dir_fd );
    }
    if (original_dir_fd >= 0) close( original_dir_fd );
    if (new_dir_fd >= 0) close( new_dir_fd );

    return BardString::create(filepath);
  }
#endif
}

void bard_file_rename( BardString* filepath_obj, BardString* new_name_obj )
{
  // File::rename(String)
  char new_name[PATH_MAX];
  new_name_obj->to_ascii( new_name, PATH_MAX );

  char filepath[PATH_MAX];
  filepath_obj->to_ascii( filepath, PATH_MAX );

#if defined(_WIN32)
  if (MoveFile(filepath,new_name))
  {
    return;
  }

#elif defined(UNIX)

  if (0 == rename(filepath,new_name))
  {
    return;
  }

#else
#error Must define bard_file_rename__String for this OS.
#endif
  bard_throw_file_error( filepath );
}

void bard_file_delete( BardString* filepath_obj )
{
  // File::delete()
  char filepath[PATH_MAX];
  filepath_obj->to_ascii( filepath, PATH_MAX );

#if defined(_WIN32)

  if ( DeleteFile(filepath) || RemoveDirectory(filepath))
  {
    return;
  }
 
#elif defined(UNIX)
  if (0 == remove(filepath))
  {
    return;
  }

#else
# error Must define bard_file_delete() for this OS.
#endif

  bard_throw_file_error( filepath );
}

bool bard_get_file_timestamp( const char* filepath, BardInt64* timestamp )
{
#if defined(_WIN32)
  HANDLE handle = CreateFile( filepath, 0, 0, NULL, OPEN_EXISTING, 
      FILE_ATTRIBUTE_NORMAL, NULL );
  if (handle != INVALID_HANDLE_VALUE)
  {
    BY_HANDLE_FILE_INFORMATION info;
    if (GetFileInformationByHandle( handle, &info ))
    {
      BardInt64 result = info.ftLastWriteTime.dwHighDateTime;
      result <<= 32;
      result |= info.ftLastWriteTime.dwLowDateTime;
      result /= 10000; // convert from Crazyseconds to Milliseconds
      result -= 11644473600000;  // base on Jan 1, 1970 instead of Jan 1, 1601 (?!)
      CloseHandle(handle);
      *timestamp = result;
      return true;
    }
    CloseHandle(handle);
  }

#elif defined(UNIX)
  int file_id = open( filepath, O_RDONLY );
  if (file_id >= 0)
  {
    struct stat info;
    if (0 == fstat(file_id, &info))
    {
#if defined(__APPLE__)
      BardInt64 result = info.st_mtimespec.tv_sec;
      result *= 1000000000;
      result += info.st_mtimespec.tv_nsec;
      result /= 1000000;  // convert to milliseconds
#else
      BardInt64 result = (BardInt64) info.st_mtime;
      result *= 1000;  // convert to ms
#endif
      close(file_id);
      *timestamp = result;
      return true;
    }
    close(file_id);
  }

#else
# error Must define bard_file_timestamp() for this OS.
#endif
  return false;
}

BardInt64 bard_file_timestamp( BardString* filepath_obj )
{
  // File::timestamp().Int64
  char filepath[PATH_MAX];
  filepath_obj->to_ascii( filepath, PATH_MAX );

  BardInt64 timestamp;
  if (bard_get_file_timestamp(filepath,&timestamp))
  {
    return timestamp;
  }
  else
  {
    bard_throw_file_not_found_error( filepath );
    return 0;
  }
}

void bard_file_touch( BardString* filepath_obj )
{
  // File::touch()
  char filepath[PATH_MAX];
  filepath_obj->to_ascii( filepath, PATH_MAX );

  FILE* fp = fopen( filepath, "rb+" );
  if (fp)
  {
    fseek( fp, -1, SEEK_END );
    int ch = getc(fp);
    if (ch == -1)
    {
      fclose(fp);
      fopen(filepath,"wb");
    }
    else
    {
      fseek( fp, -1, SEEK_END );
      putc(ch,fp);
    }
    fclose(fp);
  }
  else
  {
    fp = fopen(filepath,"wb");
    if (fp) fclose(fp);
    else bard_throw_file_error( filepath );
    return;
  }
}

void bard_file_change_dir( BardString* filepath_obj )
{
  // File::change_dir()
  char filepath[PATH_MAX];
  filepath_obj->to_ascii( filepath, PATH_MAX );

  if (0 != chdir(filepath))
  {
    bard_throw_file_error( filepath );
    return;
  }
}

void File__exists()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  BARD_PUSH_LOGICAL( bard_file_exists( filepath ) );
}

void File__is_directory()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  BARD_PUSH_LOGICAL( bard_file_is_directory( filepath ) );
}

void File__directory_listing__ArrayList_of_String()
{
  BardObject* list    = BARD_POP_REF();
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  bard_file_directory_listing( filepath, list );
}

void File__absolute_filepath()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  BARD_PUSH_REF( bard_file_absolute_filepath( filepath ) );
}

void File__rename__String()
{
  BardObject* new_name = BARD_POP_REF();
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  bard_file_rename( filepath, (BardString*) new_name );
}

void File__delete()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  bard_file_delete( filepath );
}

void File__timestamp()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  BARD_PUSH_INT64( bard_file_timestamp( filepath ) );
}

void File__touch()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  bard_file_touch( filepath );
}

void File__native_mkdir()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  printf( "WARNING: File::native_mkdir() is a skeleton implementation." );
}

void File__change_dir()
{
  BardObject* context = BARD_POP_REF();
  BardString* filepath = ((BardFile*)context)->property_filepath;
  bard_file_change_dir( filepath );
}

//-----------------------------------------------------------------------------
//  FileReader
//-----------------------------------------------------------------------------
void FileReader__init__String()
{
  BardObject*  filename_string = BARD_POP_REF();
  BardObject*  reader = BARD_POP_REF();

  // method init( String filename )
  char filename[512];

#if defined(BARD_VM)
  if ( !filename_string || !reader )
  {
    bvm.throw_exception( bvm.type_null_reference_error );
    return;
  }
#endif

  ((BardString*)filename_string)->to_ascii( filename, 512 );

  bard_adjust_filename_for_os( filename, 512 );

  if (bard_is_directory(filename))
  {
    bard_throw_file_error( filename );
    return;
  }

  FILE* fp = fopen( filename, "rb" );
  if (fp)
  {
    int remaining;
    fseek( fp, 0, SEEK_END );
    remaining = ftell(fp);
    fseek( fp, 0, SEEK_SET );

    if (remaining == 0) 
    {
      // success but zero-length file
      fclose(fp);
      return;
    }

    BARD_PUSH_REF( reader );
    BardObject* data_obj = BardNativeData::create( 
        new BardFileInfo(fp,remaining), BardNativeDataDeleteResource );
    BARD_DISCARD_REF();

    BARD_SET_REF( reader, "native_data", data_obj );
    return;
  }

  // no luck
  bard_throw_file_not_found_error( filename );
  return;
}

static BardFileInfo* get_reader_file_info( BardObject* reader )
{
  BARD_GET_REF( native_data, reader, "native_data" );

  if ( !native_data ) return NULL;
  return (BardFileInfo*) (((BardNativeData*)native_data)->data);
}

static void close_reader( BardObject* reader )
{
  BARD_GET_REF( native_data, reader, "native_data" );
  if ( !native_data ) return;
  BARD_SET_REF( reader, "native_data", (BardObject*)NULL );

  ((BardNativeData*) native_data)->release();
}

void FileReader__close()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );
  close_reader(reader);
}

void FileReader__has_another()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  // Bard files auto-close after the last character so if it's still open that
  // means there's another.
  BardFileInfo* info = get_reader_file_info(reader);
  BARD_PUSH_LOGICAL( (info ? 1 : 0) );
}

void FileReader__peek()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  BardFileInfo* info = get_reader_file_info(reader);
  if (info && info->fp)
  {
    int ch = getc(info->fp);
    ungetc( ch, info->fp );
    BARD_PUSH_CHAR( ch );
  }
  else
  {
    bard_throw_no_next_value_error();
  }
}

void FileReader__read()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  BardFileInfo* info = get_reader_file_info(reader);
  if (info && info->fp)
  {
    BardChar result = getc(info->fp);

    if ( !(--info->remaining) )
    {
      // Close the file if we're reading the last character.
      close_reader(reader);
    }

    BARD_PUSH_CHAR( result );
  }
  else
  {
    bard_throw_no_next_value_error();
  }
}

void FileReader__read__Array_of_Byte_Int32_Int32()
{
  BardInt32  count = BARD_POP_INT32();
  BardInt32  index = BARD_POP_INT32();
  BardArray* array = (BardArray*) BARD_POP_REF();
  BardObject* reader = BARD_POP_REF();

  BVM_NULL_CHECK( reader && array, return );

#if defined(BARD_VM)
  if (index < 0 || count < 0 || index+count > array->array_count)
  {
    bvm.throw_exception( bvm.type_out_of_bounds_error );
    return;
  }
#endif

  BardFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (count > info->remaining) count = info->remaining;

    if (info->fp)
    {
      count = fread( ((BardByte*)array->data) + index, 1, count, info->fp );
    }

    if (count == info->remaining)
    {
      // close file 
      close_reader(reader);
    }
    else
    {
      info->remaining -= count;
    }

    BARD_PUSH_INT32( count );
  }
  else
  {
    // no data left in file
    BARD_PUSH_INT32( 0 );
  }
}

void FileReader__read__Array_of_Char_Int32_Int32()
{
  BardInt32   count = BARD_POP_INT32();
  BardInt32   index = BARD_POP_INT32();
  BardArray*  array = (BardArray*) BARD_POP_REF();
  BardObject* reader = BARD_POP_REF();

  BVM_NULL_CHECK( reader && array, return );

#if defined(BARD_VM)
  if (index < 0 || count < 0 || index+count > array->array_count)
  {
    bvm.throw_exception( bvm.type_out_of_bounds_error );
    return;
  }
#endif

  BardFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (count > info->remaining) count = info->remaining;

    if (info->fp)
    {
      count = fread( ((BardChar*)array->data) + index, 1, count, info->fp );
    }

    if (count == info->remaining)
    {
      // close file 
      close_reader(reader);
    }
    else
    {
      info->remaining -= count;
    }

    // convert from bytes to characters
    int c = count + 1;
    BardChar* dest_ptr = ((BardChar*)array->data) + index + count;
    char* src_ptr  = ((char*)(((BardChar*)array->data) + index)) + count;
    while (--c)
    {
      *(--dest_ptr) = (BardChar) *(--src_ptr);
    }

    BARD_PUSH_INT32( count );
  }
  else
  {
    // no data left in file
    BARD_PUSH_INT32( 0 );
  }
}

void FileReader__remaining()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  BardFileInfo* info = get_reader_file_info(reader);
  BARD_PUSH_INT32( (info ? info->remaining : 0) );
}

void FileReader__skip__Int32()
{
  BardInt32   skip_count = BARD_POP_INT32();
  BardObject* reader = BARD_POP_REF();

  BVM_NULL_CHECK( reader, return );

  BardFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (skip_count < info->remaining)
    {
      info->remaining -= skip_count;
      if (info->fp)
      {
        fseek( info->fp, skip_count, SEEK_CUR );
      }
      return;
    }
    else if (skip_count == info->remaining)
    {
      close_reader(reader);
      return;
    }
  }

  close_reader(reader);
  bard_throw_file_error();
}

void FileReader__position()
{
  BardObject* reader = BARD_POP_REF();
  BVM_NULL_CHECK( reader, return );

  BardFileInfo* info = get_reader_file_info(reader);
  if (info)
  {
    if (info->fp)
    {
      BARD_PUSH_INT32( ftell(info->fp ) );
      return;
    }
  }
  else
  {
    close_reader(reader);
    bard_throw_file_error();
    return;
  }
  BARD_PUSH_INT32( 0 );
}


///////////////////////////////////////////////////////////////////////////////
//  FileWriter
///////////////////////////////////////////////////////////////////////////////
static BardFileInfo* get_writer_file_info( BardObject* writer )
{
  BARD_GET_REF( native_data, writer, "native_data" );
  if ( !native_data ) return NULL;

  return (BardFileInfo*) (((BardNativeData*)native_data)->data);
}

static void close_writer( BardObject* writer )
{
  BARD_GET_REF( native_data, writer, "native_data" );
  if ( !native_data ) return;
  BARD_SET_REF( writer, "native_data", (BardObject*)NULL );
  ((BardNativeData*) native_data)->release();
}

void FileWriter__init__String_Logical()
{
  BardLogical append = BARD_POP_LOGICAL();
  BardObject* filename_string = BARD_POP_REF();
  BardObject* writer = BARD_POP_REF();

  char filename[512];
  ((BardString*)filename_string)->to_ascii( filename, 512 );

  const char* mode = "wb";
  if (append) mode = "ab";

  FILE* fp = fopen( filename, mode );
  if ( !fp )
  {
    bard_throw_file_error( filename );
    return;
  }

  BARD_PUSH_REF( writer );
  BardObject* data_obj = BardNativeData::create( 
      new BardFileInfo(fp,0), BardNativeDataDeleteResource );
  BARD_DISCARD_REF();

  BARD_SET_REF( (writer), "native_data", data_obj );
}

void FileWriter__close()
{
  close_writer( BARD_POP_REF() );
}

void FileWriter__write__Char()
{
  BardChar ch = BARD_POP_CHAR();
  BardObject* writer = BARD_POP_REF();

  BardFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    putc(ch,info->fp);
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

  BVM_NULL_CHECK( array, return );

  BardFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    int limit = index + count;
    if (index < 0 || count < 0 || limit > array->array_count)
    {
      bard_throw_invalid_operand_error();
      return;
    }

    BardChar* src = ((BardChar*)array->data) + index - 1;
    ++count;
    while (--count)
    {
      char ch = (char) *(++src);
      putc(ch,info->fp);
    }
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

  BVM_NULL_CHECK( array, return );

  BardFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    int limit = index + count;
    if (index < 0 || count < 0 || limit > array->array_count)
    {
      bard_throw_invalid_operand_error();
      return;
    }

    fwrite( ((char*)array->data)+index, 1, count, info->fp );
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

  BVM_NULL_CHECK( st, return );

  BardFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    BardChar* src = st->characters - 1;
    int count = st->count + 1;
    while (--count)
    {
      char ch = (char) *(++src);
      putc(ch,info->fp);
    }
  }
  else
  {
    bard_throw_file_error();
  }
}


void FileWriter__position()
{
  BardObject* writer = BARD_POP_REF();

  BardFileInfo* info = get_writer_file_info(writer);
  if (info)
  {
    FILE* fp = info->fp;
    BARD_PUSH_INT32( ftell(fp) );
  }
  else
  {
    bard_throw_file_error();
  }
}

#endif // !defined(ANDROID)

//-----------------------------------------------------------------------------
//  Global
//-----------------------------------------------------------------------------
BardInt64 bard_get_time_ms()
{
#if defined(_WIN32)
  struct __timeb64 time_struct;
  BardInt64 time_ms;
  _ftime64_s( &time_struct );
  time_ms = (BardInt64) time_struct.time;
  time_ms *= 1000;
  time_ms += time_struct.millitm;
  return time_ms;
#elif defined(RVL)
  BardInt64 time_ms = OSTicksToMilliseconds(OSGetTime());
  return time_ms;

#else
  struct timeval time_struct;
  BardInt64 time_ms;
  gettimeofday( &time_struct, 0 );
  time_ms = (BardInt64) time_struct.tv_sec;
  time_ms *= 1000;
  time_ms += (time_struct.tv_usec / 1000);
  return time_ms;
#endif
}

void Global__time_ms()
{
  BARD_DISCARD_REF();
  BARD_PUSH_INT64( bard_get_time_ms() );
}

void Global__sleep__Int32()
{
  BardInt32 sleep_ms = BARD_POP_INT32();
  BARD_DISCARD_REF();

  if (sleep_ms <= 0) return;

#if defined(_WIN32)
  Sleep( (DWORD) sleep_ms );
#elif defined(RVL)
  vm_int64 target_ms = get_time_ms() + sleep_ms;
  while (get_time_ms() < target_ms)
  {
  }
#else
  usleep( sleep_ms * 1000 );
#endif
}


/*

//-----------------------------------------------------------------------------
//  Math
//-----------------------------------------------------------------------------
void Math__abs__Int64()
{
  BardInt64 *addr = &BARD_PEEK_BardInt64();
  if (*addr < 0) *addr = -(*addr);
  BARD_DISCARD_REF();  // Discard context object
}

void Math__abs__Int32()
{
#if (BYTE_ALIGNMENT < 8)
  BardInt32 *addr = &BARD_PEEK_INT32();
  if (*addr < 0) *addr = -(*addr);
  BARD_DISCARD_REF();  // Discard context object
#else
  BardInt64 *addr = &BARD_PEEK_INT64();
  if (*addr < 0) *addr = -(*addr);
  BARD_DISCARD_REF();  // Discard context object
#endif
}

void Math__abs__Real64()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  if (*addr < 0) *addr = -(*addr);
  BARD_DISCARD_REF();  // Discard context object 
}

void Math__floor__Real64()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) floor( *addr );
  BARD_DISCARD_REF();  // Discard context object
}

void Math__sqrt__Real64()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) sqrt( *addr );
  BARD_DISCARD_REF();  // Discard context object
}

void Math__sin__Radians()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) sin( *addr );
  BARD_DISCARD_REF();  // Discard context object 
}

void Math__cos__Radians()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) cos( *addr );
  BARD_DISCARD_REF();  // Discard context object
}

void Math__tan__Radians()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) tan( *addr );
  BARD_DISCARD_REF();  // Discard context object 
}

void Math__asin__Real64()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) asin( *addr );
  BARD_DISCARD_REF();  // Discard context object
}

void Math__acos__Real64()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) acos( *addr );
  BARD_DISCARD_REF();  // Discard context object 
}

void Math__atan__Real64()
{
  BardReal64 *addr = &BARD_PEEK_BardReal64();
  *addr = (BardReal64) atan( *addr );
  BARD_DISCARD_REF();  // Discard context object
}

void Math__atan2__Real64_Real64()
{
  BardReal64 x = BARD_POP_REAL64();
  BardReal64 *y_addr = &BARD_PEEK_BardReal64();
  *y_addr = (BardReal64) atan2( *y_addr,x );
  BARD_DISCARD_REF();  // Discard context object 
}
*/

//=============================================================================
//  NativeData
//=============================================================================
void NativeData__clean_up()
{
  BardNativeData* context = (BardNativeData*) BARD_POP_REF();
  if (context) context->release();
}


//=============================================================================
//  Network
//=============================================================================
#if !defined(ANDROID)
void Network__is_connected()
{
  BARD_POP_REF();
  BARD_PUSH_LOGICAL( true );  // TODO
}

void Network__is_mobile()
{
  BARD_POP_REF();
  BARD_PUSH_LOGICAL( false );  // TODO
}

void Network__mac_address()
{
  BARD_POP_REF();
  BARD_PUSH_REF( BardString::create("(Network.mac_address not implemented)") );  // TODO
}
#endif


//=============================================================================
//  Object
//=============================================================================
void Object__hash_code()
{
  // Object memory address is the default hash code
  BardObject* context = BARD_POP_REF();
  BVM_NULL_CHECK( context, return );
#ifdef __amd64__
  BARD_PUSH_INT32( (BardInt32)(int)(uint64_t)context );
#else
  BARD_PUSH_INT32( (BardInt32)(int)context );
#endif
}

/*
void Object__runtime_type()
{
  // Object::runtime_type().RuntimeType
  BardObject* obj = BARD_POP_REF();
  BARD_PUSH_REF( obj->type->get_runtime_type() );
}
*/

//--------------------------------------------------------------------
//  ParseReader
//--------------------------------------------------------------------
void ParseReader__prep_data()
{
  BardObject*  context = BARD_POP_REF();

  BardParseReader* reader = (BardParseReader*) context;

  BVM_NULL_CHECK( reader, return );
  BVM_NULL_CHECK( reader->property_data, return );

  if (reader->property_spaces_per_tab < 0) reader->property_spaces_per_tab = 0;
  int spaces_per_tab = reader->property_spaces_per_tab;

  // Count how many characters we'll have after removing \r and changing
  // each tab into a given number of spaces.
  BardChar* read_pos  = ((BardChar*) reader->property_data->data) - 1;
  int remaining = reader->property_remaining + 1;
  int count = 0;
  while (--remaining)
  {
    switch (*(++read_pos))
    {
      case  9: count += spaces_per_tab; break;
      case 13: break;
      default: ++count;
    }
  }

  // Create a new array of the appropriate size.
  BARD_PUSH_REF( context );
  BardArray* new_array = BARD_TYPE_ARRAY_OF_CHAR->create( count );
  BARD_DISCARD_REF();

  // Copy the data into it while filtering
  BardChar* write_pos = ((BardChar*) new_array->data) - 1;
  read_pos  = ((BardChar*) reader->property_data->data) - 1;
  remaining = reader->property_remaining + 1;
  while (--remaining)
  {
    BardChar ch = *(++read_pos);
    switch (ch)
    {
      case  9: 
      {
        int spaces = spaces_per_tab + 1;
        while (--spaces) *(++write_pos) = 32;
        break;
      }

      case 13: break;

      default:
        *(++write_pos) = ch;
    }
  }

  reader->property_data = new_array;
  ++new_array->reference_count;

  reader->property_remaining = count;
}

void ParseReader__has_another()
{
  BardParseReader* reader = (BardParseReader*) BARD_POP_REF();
  BARD_PUSH_LOGICAL( reader->property_remaining > 0 );
}

void ParseReader__peek()
{
  BardParseReader* reader = (BardParseReader*) BARD_POP_REF();
  BARD_PUSH_CHAR( ((BardChar*)reader->property_data->data)[reader->property_pos] );
}

void ParseReader__peek__Int32()
{
  BardInt32 num_ahead = BARD_POP_INT32();
  BardParseReader* reader = (BardParseReader*) BARD_POP_REF();
  --num_ahead;
  if ((unsigned int) num_ahead >= (unsigned int) reader->property_remaining)
  {
    BARD_PUSH_CHAR(0);
  }
  else
  {
    BARD_PUSH_CHAR(((BardChar*)reader->property_data->data)[reader->property_pos+num_ahead]);
  }
}

void ParseReader__read()
{
  BardParseReader* reader = (BardParseReader*) BARD_POP_REF();
  --reader->property_remaining;
  BardChar ch = ((BardChar*)reader->property_data->data)[reader->property_pos++];
  switch (ch)
  {
    case 10:
      ++reader->property_line;
      reader->property_column = 1;
      break;

    default:
      ++reader->property_column;
  }
  BARD_PUSH_CHAR(ch);
}

void ParseReader__consume__Char()
{
  BardChar ch = BARD_POP_CHAR();
  BardParseReader* reader = (BardParseReader*) BARD_POP_REF();
  if ( !reader->property_remaining )
  {
    BARD_PUSH_LOGICAL(0);
    return;
  }

  BARD_PUSH_REF( reader );
  ParseReader__peek();
  BardChar next_ch = BARD_POP_CHAR();
  if (ch == next_ch)
  {
    BARD_PUSH_REF( reader );
    ParseReader__read();
    BARD_POP_CHAR();
    BARD_PUSH_LOGICAL(1);
  }
  else
  {
    BARD_PUSH_LOGICAL(0);
  }
}

void ParseReader__consume__String()
{
  BardString* st = (BardString*) BARD_POP_REF();

  BardParseReader* reader = (BardParseReader*) BARD_POP_REF();
  int count = st->count;
  if (count > reader->property_remaining)
  {
    BARD_PUSH_LOGICAL(0);
    return;
  }

  if (0 == memcmp( st->characters, 
        ((BardChar*) reader->property_data->data) + reader->property_pos, count<<1 ))
  {
    reader->property_pos += count;
    reader->property_column += count;
    reader->property_remaining -= count;
    BARD_PUSH_LOGICAL(1);
  }
  else
  {
    BARD_PUSH_LOGICAL(0);
  }
}


//--------------------------------------------------------------------
//  Process
//--------------------------------------------------------------------
void Process__init__String()
{
  BardString* cmd_line_string = (BardString*) BARD_POP_REF();
  BardObject* process_obj = BARD_POP_REF();

  BVM_NULL_CHECK( process_obj && cmd_line_string, return );

  char* cmd_line = cmd_line_string->to_new_ascii();
  int result = system(cmd_line);
  delete cmd_line;

  ((BardProcess*)process_obj)->exit_code = result;
}

void Process__update()
{
  BARD_DISCARD_REF();
}

void Process__release()
{
  BARD_DISCARD_REF();
}


//=============================================================================
//  Runtime
//=============================================================================

/*
//--------------------------------------------------------------------
//  Runtime
//--------------------------------------------------------------------
void Runtime__find_type_index__String()
{
  // Runtime.find_type_index(String).Int32
  BardString* name_obj = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();  // discard singleton context

  BVM_NULL_CHECK( name_obj, bvm.type_null_reference_error, return );

  char buffer[256];
  name_obj->to_ascii( buffer, 256 );
  BardTypeInfo* type = bvm_find_type( buffer );

  if (type)
  {
    BARD_PUSH_INTEGER( type->index );
  }
  else
  {
    bvm_throw_exception_of_type( bvm.type_invalid_operand_error );
  }
}


//--------------------------------------------------------------------
//  RuntimeMethod
//--------------------------------------------------------------------
static BardMethodInfo* get_method_info()
{
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  if ( !context )
  {
    bvm_throw_exception_of_type( bvm.type_null_reference_error );
    return NULL;
  }

  BardTypeInfo* type = context->type;

  BARD_GET( BardInt32, method_index, obj, "method_index" );

  if ((unsigned int) method_index >= (unsigned int) type->num_methods)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return NULL;
  }

  return type->methods[method_index];
}

void RuntimeMethod__name()
{
  // RuntimeMethod::name().String
  BardMethodInfo* m = get_method_info();
  if (m) BARD_PUSH_REF( BardString::create(m->name.data) );
}

void RuntimeMethod__return_type()
{
  // RuntimeMethod::return_type().Int32
  BardMethodInfo* m = get_method_info();
  if (m)
  {
    BardTypeInfo* return_type = m->return_type;

    if (return_type)
    {
      BARD_PUSH_REF( return_type->get_runtime_type() );
    }
    else
    {
      BARD_PUSH_REF( NULL );
    }
  }
}

void RuntimeMethod__parameter_types()
{
  // RuntimeMethod::parameters().RuntimeType[]
  BardMethodInfo* m = get_method_info();
  if (m)
  {
    int num_params = m->param_info->num_parameters;

    BARD_FIND_TYPE( type_runtime_type_list, "ArrayList<<RuntimeType>>" );

    bvm_push_new_arraylist( type_runtime_type_list, num_params, true );
    BardArrayList** arraylist_ptr = (BardArrayList**) &BARD_PEEK_REF();

    for (int i=0; i<num_params; ++i)
    {
      BardObject* next_param_type = m->param_info->parameter_types[i]->get_runtime_type();
      ((BardObject**)((*arraylist_ptr)->array->data))[i] = next_param_type;
    }
  }
}

void RuntimeMethod__signature()
{
  // RuntimeMethod::signature().String
  BardMethodInfo* m = get_method_info();
  if (m) BARD_PUSH_REF( BardString::create(m->signature.data) );
}

void RuntimeMethod__arg__Object()
{
  BardObject* param = BARD_POP_REF();

  if (introspection_ref_stack_count == INTROSPECTION_MAX_REF_STACK_SIZE)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
  }

  introspection_ref_stack[introspection_ref_stack_count++].retain( param );
}

void RuntimeMethod__arg__Int64()
{
  if (introspection_data_stack_count + 8 >= INTROSPECTION_MAX_DATA_STACK_SIZE)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
  }

  introspection_data_stack_ptr -= 8;
  *((BardInt64*)introspection_data_stack_ptr) = BARD_POP_INT64();
  introspection_data_stack_count += 8;
}

void RuntimeMethod__arg__Int32()
{
  if (introspection_data_stack_count + 4 >= INTROSPECTION_MAX_DATA_STACK_SIZE)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
  }

  introspection_data_stack_ptr -= 4;
  *((BardInt32*)introspection_data_stack_ptr) = BARD_POP_INT32();
  introspection_data_stack_count += 4;
}

BardMethodInfo* RuntimeMethod_call_helper()
{
  // RuntimeMethod::signature().String
  BardObject* obj = BARD_PEEK_REF();

  BardMethodInfo* m = get_method_info();
  if ( !m ) return 0;

  BARD_GET( BardObject*, call_context, obj, "context" );
  if ( !call_context )
  {
    bvm_throw_exception_of_type( bvm.type_null_reference_error );
    return NULL;
  }

  BardParamInfo* param_info = m->param_info;
  if (introspection_data_stack_count != param_info->param_data_size
      || (introspection_ref_stack_count + 1) != param_info->num_ref_params)
  {
    bvm_throw_exception_of_type( bvm.type_invalid_operand_error );
    return NULL;
  }

  BARD_PUSH_REF( call_context );
  for (int i=0; i<introspection_ref_stack_count; ++i)
  {
    BARD_PUSH_REF( introspection_ref_stack[i].object );
    introspection_ref_stack[i].release();
  }
  introspection_ref_stack_count = 0;

  int bytes = introspection_data_stack_count;
  if (bytes)
  {
    bvm.regs.stack.data -= bytes;
    memcpy( bvm.regs.stack.data, introspection_data_stack_ptr, bytes );
    introspection_data_stack_ptr = introspection_data_stack + INTROSPECTION_MAX_DATA_STACK_SIZE;
    introspection_data_stack_count = 0;
  }

  bvm_call( m );

  return m;
}

void RuntimeMethod__call()
{
  RuntimeMethod_call_helper();
}

void RuntimeMethod__call_Object()
{
  BardMethodInfo* m = RuntimeMethod_call_helper();
  if (m)
  {
    if ( !(m->return_type && m->return_type->is_reference()) )
    {
      bvm_throw_exception_of_type( bvm.type_type_cast_error );
    }
  }
}

void RuntimeMethod__call_Int64()
{
  BardMethodInfo* m = RuntimeMethod_call_helper();
  if (m)
  {
    if ( !(m->return_type && m->return_type->stack_size == 8) )
    {
      bvm_throw_exception_of_type( bvm.type_type_cast_error );
    }
  }
}

void RuntimeMethod__call_Int32()
{
  BardMethodInfo* m = RuntimeMethod_call_helper();
  if (m)
  {
    if ( !(m->return_type && m->return_type->stack_size == 4) )
    {
      bvm_throw_exception_of_type( bvm.type_type_cast_error );
    }
  }
}


//--------------------------------------------------------------------
//  RuntimeMethods
//--------------------------------------------------------------------
void RuntimeMethods__array_count
{
  // RuntimeMethods::array_count.Int32
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );

  BARD_PUSH_INTEGER( context->type->num_methods );
}

//--------------------------------------------------------------------
//  RuntimeProperties
//--------------------------------------------------------------------
void RuntimeProperties__array_count
{
  // RuntimeProperties::array_count.Int32
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );

  BARD_PUSH_INTEGER( context->type->num_properties );
}


//--------------------------------------------------------------------
//  RuntimeProperty
//--------------------------------------------------------------------
void RuntimeProperty__type()
{
  // RuntimeProperty.type().RuntimeType
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );
  BardTypeInfo* type = context->type;

  BARD_GET( BardInt32, property_index, obj, "property_index" );

  if ((unsigned int) property_index >= (unsigned int) type->num_properties)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BARD_PUSH_REF( type->property_types[property_index]->get_runtime_type() );
}

void RuntimeProperty__name()
{
  // RuntimeProperty.name().String
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );
  BardTypeInfo* type = context->type;

  BARD_GET( BardInt32, property_index, obj, "property_index" );

  if ((unsigned int) property_index >= (unsigned int) type->num_properties)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BARD_PUSH_REF( BardString::create(type->property_names[property_index].data) );
}


void RuntimeProperty__as_Object()
{
  // RuntimeProperty::as_Object().Object
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BARD_GET( BardInt32,   property_index, obj, "property_index" );

  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );

  if ((unsigned int) property_index >= (unsigned int) context->type->num_properties)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BardTypeInfo* context_type = context->type;

  if ( !context_type->property_types[property_index]->is_reference() )
  {
    bvm_throw_exception_of_type( bvm.type_type_cast_error );
    return;
  }

  int offset = context_type->property_offsets[property_index];
  BARD_PUSH_REF( BVM_DEREFERENCE(BardObject*, context, offset) );
}

void RuntimeProperty__as_Object__Object()
{
  BardObject* new_value = BARD_POP_REF();
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BARD_GET( BardInt32,   property_index, obj, "property_index" );

  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );

  BardTypeInfo* context_type = context->type;

  if ((unsigned int) property_index >= (unsigned int) context_type->num_properties)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BardTypeInfo* property_type = context_type->property_types[property_index];

  if ( !property_type->is_reference() )
  {
    bvm_throw_exception_of_type( bvm.type_type_cast_error );
    return;
  }

  if (new_value && !new_value->type->instance_of(property_type))
  {
    bvm_throw_exception_of_type( bvm.type_invalid_operand_error );
    return;
  }

  int offset = context_type->property_offsets[property_index];
  BVM_DEREFERENCE(BardObject*, context, offset) = new_value;
}

void RuntimeProperty_read_primitive( int size )
{
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BARD_GET( BardInt32,   property_index, obj, "property_index" );

  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );

  if ((unsigned int) property_index >= (unsigned int) context->type->num_properties)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BardTypeInfo* context_type = context->type;
  BardTypeInfo* property_type = context_type->property_types[property_index];

  if ( property_type->is_reference() || property_type->data_size != size)
  {
    bvm_throw_exception_of_type( bvm.type_type_cast_error );
    return;
  }

  int offset = context_type->property_offsets[property_index];
  switch (size)
  {
    case 8: BARD_PUSH_INTEGER( BVM_DEREFERENCE(BardInt64,context,offset) ); break;
    case 4: BARD_PUSH_INTEGER( BVM_DEREFERENCE(BardInt32,context,offset) ); break;
    case 2: BARD_PUSH_INTEGER( BVM_DEREFERENCE(BardChar,context,offset) ); break;
    case 1: BARD_PUSH_INTEGER( BVM_DEREFERENCE(BardByte,context,offset) ); break;
    default:
      bvm_throw_exception_of_type( bvm.type_invalid_operand_error );
  }
}

void RuntimeProperty_write_primitive( int size )
{
  BardObject* obj = BARD_POP_REF();

  BARD_GET( BardObject*, context, obj, "context" );
  BARD_GET( BardInt32,   property_index, obj, "property_index" );

  BVM_NULL_CHECK( context, bvm.type_null_reference_error, return );

  if ((unsigned int) property_index >= (unsigned int) context->type->num_properties)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BardTypeInfo* context_type = context->type;
  BardTypeInfo* property_type = context_type->property_types[property_index];

  if ( property_type->is_reference() || property_type->data_size != size)
  {
    bvm_throw_exception_of_type( bvm.type_type_cast_error );
    return;
  }

  int offset = context_type->property_offsets[property_index];
  switch (size)
  {
    case 8:
      {
        BardInt64 new_value = BARD_POP_INT64();
        BVM_DEREFERENCE(BardInt64,context,offset) = new_value; 
        break;
      }

    case 4:
      {
        BardInt32 new_value = BARD_POP_INT32();
        BVM_DEREFERENCE(BardInt32,context,offset) = new_value; 
        break;
      }

    case 2:
      {
        BardChar new_value = (BardChar) BARD_POP_INT32();
        BVM_DEREFERENCE(BardChar,context,offset) = new_value; 
        break;
      }

    case 1:
      {
        BardByte new_value = (BardByte) BARD_POP_INT32();
        BVM_DEREFERENCE(BardByte,context,offset) = new_value; 
        break;
      }

    default:
      bvm_throw_exception_of_type( bvm.type_invalid_operand_error );
  }
}

void RuntimeProperty__as_Int64()        { RuntimeProperty_read_primitive(8);  }
void RuntimeProperty__as_Int64__Int64() { RuntimeProperty_write_primitive(8); }
void RuntimeProperty__as_Int32()        { RuntimeProperty_read_primitive(4);  }
void RuntimeProperty__as_Int32__Int32() { RuntimeProperty_write_primitive(4); }
void RuntimeProperty__as_Char()         { RuntimeProperty_read_primitive(2);  }
void RuntimeProperty__as_Char__Char()   { RuntimeProperty_write_primitive(2); }
void RuntimeProperty__as_Byte()         { RuntimeProperty_read_primitive(1);  }
void RuntimeProperty__as_Byte__Byte()   { RuntimeProperty_write_primitive(1); }

//--------------------------------------------------------------------
//  RuntimeType
//--------------------------------------------------------------------
void RuntimeType__name()
{
  // RuntimeType.name().String
  BardObject* obj = BARD_POP_REF();
  BARD_GET( BardInt32, index, obj, "index" );

  if ((unsigned int) index >= (unsigned int) bvm.num_types)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BardTypeInfo* type = &bvm.types[index];

  BardObject* name_obj = type->name_string_obj;
  if ( !name_obj )
  {
    name_obj = bvm_create_permanent_string(type->name.data);
    type->name_string_obj = name_obj;
  }
  BARD_PUSH_REF( name_obj );
}

void RuntimeType__instance_of__RuntimeType()
{
  // RuntimeType::instance_of(RuntimeType).Logical
  BardObject* obj2 = BARD_POP_REF();
  BVM_NULL_CHECK( obj2, bvm.type_null_reference_error, return );
  BARD_GET( BardInt32, type2_index, obj2, "index" );

  BardObject* obj = BARD_POP_REF();
  BARD_GET( BardInt32, type1_index, obj, "index" );

  if ( ((unsigned int) type1_index >= (unsigned int) bvm.num_types)
      || ((unsigned int) type2_index >= (unsigned int) bvm.num_types) )
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BardTypeInfo* type1 = &bvm.types[type1_index];
  BardTypeInfo* type2 = &bvm.types[type2_index];

  BARD_PUSH_INTEGER( type1->instance_of(type2) );
}

void RuntimeType__create_instance()
{
  BardObject* obj = BARD_POP_REF();
  BARD_GET( BardInt32, index, obj, "index" );

  if ((unsigned int) index >= (unsigned int) bvm.num_types)
  {
    bvm_throw_exception_of_type( bvm.type_out_of_bounds_error );
    return;
  }

  BARD_PUSH_REF( bvm_create_object( &bvm.types[index] ) );
}
*/

//=============================================================================
// SHA1
//=============================================================================
#if !defined(ANDROID)
void SHA1__hmac__String_String()
{
  BARD_POP_REF();
  BARD_POP_REF();
  BARD_POP_REF();
  BARD_PUSH_REF( BardString::create("0000000000000000000000000000000000000000") );
}
#endif // !ANDROID


//////////////////////////////////////////////////////////////////////
//  Socket
//  SocketReader
//  SocketWriter
//  ServerSocket
//////////////////////////////////////////////////////////////////////
static SocketInfo* get_socket_socket_info( BardObject* socket )
{
  BardNativeData* native_data = ((BardSocket*) socket)->native_data;
  if ( !native_data ) return NULL;

  return (SocketInfo*) (native_data->data);
}

static SocketInfo* get_reader_socket_info( BardObject* reader )
{
  BardNativeData* native_data = ((BardNativeDataWrapper*) reader)->native_data;
  if ( !native_data ) return NULL;
  return (SocketInfo*) (native_data->data);
}

static SocketInfo* get_writer_socket_info( BardObject* writer )
{
  BardNativeData* native_data = ((BardNativeDataWrapper*) writer)->native_data;
  if ( !native_data ) return NULL;
  return (SocketInfo*) (native_data->data);
}

static ServerSocketInfo* get_server_socket_info( BardObject* server_socket )
{
  BardNativeData* native_data = ((BardNativeDataWrapper*) server_socket)->native_data;
  if ( !native_data ) return NULL;
  return (ServerSocketInfo*) (native_data->data);
}

static void close_socket( BardObject* obj )
{
  BardNativeData* native_data = ((BardNativeDataWrapper*) obj)->native_data;
  if ( !native_data ) return;
  --(((BardNativeDataWrapper*)obj)->native_data->reference_count);
  ((BardNativeDataWrapper*)obj)->native_data = NULL;
  native_data->release();
}

static void close_server_socket( BardObject* server_socket )
{
  close_socket( server_socket );
}


void Socket__native_init__String_Int32()
{
  BardInt32   port = BARD_POP_INT32();
  BardString* addr_string = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();

  char* addr = addr_string->to_new_ascii();
  BardObject* result = BardNativeData::create( new SocketInfo(addr,port), BardNativeDataDeleteResource );
  delete addr;

  BARD_PUSH_REF( result );
}

void Socket__connection_pending()
{
  BardObject* context = BARD_POP_REF();
  BVM_NULL_CHECK( context, return );

  SocketInfo* socket = get_socket_socket_info(context);
  int result = 0;
  if (socket && socket->status() == 1) result = 1;
  BARD_PUSH_LOGICAL( result );
}

void Socket__is_connected()
{
  BardObject* context = BARD_POP_REF();
  BVM_NULL_CHECK( context, return );

  SocketInfo* socket = get_socket_socket_info(context);
  int result = 0;
  if (socket && socket->status() == 2) result = 1;
  BARD_PUSH_LOGICAL( result );
}

void Socket__native_remote_ip()
{
  BardObject* socket_obj = BARD_POP_REF();
  BVM_NULL_CHECK( socket_obj, return );

  SocketInfo* socket = get_socket_socket_info(socket_obj);
  BVM_NULL_CHECK( socket, return );

  BARD_PUSH_REF( BardString::create(socket->remote_ip) );
}

void Socket__close()
{
  BardObject* socket_obj = BARD_POP_REF();
  BVM_NULL_CHECK( socket_obj, return );
  close_socket(socket_obj);
}


void SocketReader__available()
{
  BardObject* reader_obj = BARD_POP_REF();
  BVM_NULL_CHECK( reader_obj, return );
  SocketInfo* socket = get_reader_socket_info(reader_obj);
  if (socket)
  {
    BARD_PUSH_INT32( socket->available() );
  }
  else
  {
    BARD_PUSH_INT32( 0 );
  }
}

void SocketReader__peek()
{
  BardObject* reader_obj = BARD_POP_REF();
  BVM_NULL_CHECK( reader_obj, return );

  SocketInfo* socket = get_reader_socket_info(reader_obj);
  if (socket)
  {
    BARD_PUSH_CHAR( socket->peek() );
  }
  else
  {
    bard_throw_no_next_value_error();
  }
}

void SocketReader__read()
{
  BardObject* reader_obj = BARD_POP_REF();
  BVM_NULL_CHECK( reader_obj, return );

  SocketInfo* socket = get_reader_socket_info(reader_obj);
  if (socket)
  {
    BARD_PUSH_CHAR( socket->read() );
  }
  else
  {
    bard_throw_no_next_value_error();
  }
}

void SocketWriter__write__Char()
{
  BardChar value = BARD_POP_CHAR();
  BardObject* writer_obj = BARD_POP_REF();
  BVM_NULL_CHECK( writer_obj, return );

  SocketInfo* socket = get_writer_socket_info(writer_obj);
  if ( !(socket && socket->write(value)) )
  {
    bard_throw_socket_error();
  }
}

void ServerSocket__native_init__Int32()
{
  BardInt32 port = BARD_POP_INT32();
  BARD_DISCARD_REF();
  ServerSocketInfo* server_socket = new ServerSocketInfo( port );

  if (server_socket->status() != 1)
  {
    bard_throw_socket_error();
  }

  BARD_PUSH_REF( BardNativeData::create(server_socket, BardNativeDataDeleteResource) );
}

void ServerSocket__is_connected()
{
  BardObject* server_socket_obj = BARD_POP_REF();
  BVM_NULL_CHECK( server_socket_obj, return );

  ServerSocketInfo* server_socket = get_server_socket_info(server_socket_obj);
  if (server_socket)
  {
    BARD_PUSH_LOGICAL( server_socket->status() );
  }
  else
  {
    BARD_PUSH_LOGICAL( 0 );
  }
}

void ServerSocket__get_pending_info()
{
  BardObject* server_socket_obj = BARD_POP_REF();
  BVM_NULL_CHECK( server_socket_obj, return );

  ServerSocketInfo* server_socket = get_server_socket_info(server_socket_obj);
  if (server_socket)
  {
    BARD_PUSH_REF( server_socket->accept_connection() );
  }
  else
  {
    BARD_PUSH_REF( NULL );
  }
}

void ServerSocket__close()
{
  BardObject* server_socket_obj = BARD_POP_REF();
  BVM_NULL_CHECK( server_socket_obj, return );
  close_server_socket( server_socket_obj );
}

//=============================================================================
//  StackTrace
//=============================================================================
#if defined(BARD_XC)
void StackTrace__native_history()
{
  BARD_DISCARD_REF();
  BARD_PUSH_REF( NULL );
}

void StackTrace__describe__Int64()
{
  BARD_POP_INT32();
  BARD_DISCARD_REF();
  BARD_PUSH_REF( NULL );
}
#endif

//-----------------------------------------------------------------------------
//  StdOutReader
//  StdOutWriter
//-----------------------------------------------------------------------------
void StdInReader__native_read_char()
{
  BARD_DISCARD_REF();
  BardChar ch = getc(stdin);
  BARD_PUSH_CHAR( ch );
}

void StdOutWriter__flush()
{
  BARD_DISCARD_REF();
  fflush(stdout);
}

void StdOutWriter__print__Char()
{
  BardChar ch = BARD_POP_CHAR();
  BARD_DISCARD_REF();
  putc( ch, stdout );
}

void StdOutWriter__print__String()
{
  BardString* string = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();

  if ( !string )
  {
    printf( "null" );
  }
  else
  {
    int count = string->count + 1;
    BardChar* data = string->characters - 1;
    while (--count) putc( *(++data), stdout );
  }
}

void StdOutWriter__write__Char()
{
  StdOutWriter__print__Char();
}

void StdOutWriter__write__String()
{
  StdOutWriter__print__String();
}

//-----------------------------------------------------------------------------
//  String
//-----------------------------------------------------------------------------
void StringBuilder__native_copy__String_Array_of_Char_Int32()
{
  BardInt32   dest_offset = BARD_POP_INT32();
  BardArray*  array = (BardArray*) BARD_POP_REF();
  BardString* st = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();

  // dest_offset + src_string.length must be <= dest_array.count
#if defined(BARD_VM)
  if (dest_offset + st->count > array->array_count)
  {
    bvm.throw_exception( bvm.type_out_of_bounds_error );
    return;
  }
#endif

  memcpy( ((BardChar*) array->data)+dest_offset, st->characters, st->count*2 );
}

void StringManager__create_from__Array_of_Char_Int32()
{
  BardInt32  count = BARD_POP_INT32();
  BardArray* array = (BardArray*) BARD_POP_REF();
  BARD_DISCARD_REF();

  if (count == -1) count = array->array_count;

  BARD_PUSH_REF( array );
  BardObject* st = BardString::create( (BardChar*)(array->data), count );
  BARD_DISCARD_REF();
  BARD_PUSH_REF( st );
}

void StringManager__create_from__Char()
{
  BardChar ch = BARD_POP_CHAR();
  BARD_DISCARD_REF();

  BardChar chars[1];
  chars[0] = ch;
  BARD_PUSH_REF( BardString::create( chars, 1 ) );
}

void String__count()
{
  BardString* context = (BardString*) BARD_POP_REF();
  BVM_NULL_CHECK( context, return );
  BARD_PUSH_INT32( context->count );
}

void String__get__Int32()
{
  BardInt32   index = BARD_POP_INT32();
  BardString* context = (BardString*) BARD_POP_REF();
  BVM_NULL_CHECK( context, return );

  BARD_PUSH_CHAR( context->characters[index] );
}

void String__hash_code()
{
  BardString* context = (BardString*) BARD_POP_REF();
  BARD_PUSH_INT32( context->hash_code );
}

void String__opCMP__String()
{
  // method op<>(String).Logical
  // Returns:
  //   'eq' if this.op==(other)
  //   'lt' if strings differ at spot 'i' and this[i] < other[i]
  //   'gt' if strings differ at spot 'i' and this[i] > other[i] 
  //
  BardString* st2 = (BardString*) BARD_POP_REF();
  BardString* st1 = (BardString*) BARD_POP_REF();

  BVM_NULL_CHECK( st1 && st2, return );

  int count1 = st1->count;
  int count2 = st2->count;
  int compare_count = count1;

  if (count2 < compare_count) compare_count = count2;

  BardChar* data1 = (st1->characters) - 1;
  BardChar* data2 = (st2->characters) - 1;

  ++compare_count;
  while (--compare_count)
  {
    if (*(++data1) != *(++data2))
    {
      if (*data1 < *data2)
      {
        BARD_PUSH_INT32( -1 );  // lt
        return;
      }
      else
      {
        BARD_PUSH_INT32( 1 );  // gt
        return;
      }
    }
  }

  // Everything matches so far.  Determine final equality based on string length.

  int result;
  if (count1 == count2) result = 0; // eq
  else if (count1 > count2) result = 1;
  else result = -1;
  BARD_PUSH_INT32( result );
}


void String__opEQ__String()
{
  BardString* st2 = (BardString*) BARD_POP_REF();
  BardString* st1 = (BardString*) BARD_POP_REF();

  BVM_NULL_CHECK( st1, return );

  if ( !st2 )
  {
    BARD_PUSH_LOGICAL( 0 );
    return;
  }

  if (st1 == st2)
  {
    BARD_PUSH_LOGICAL( 1 );
    return;
  }

  if (st1->hash_code != st2->hash_code) 
  {
    BARD_PUSH_LOGICAL( 0 );
    return;
  }

  int count = st1->count;
  if (count != st2->count)
  {
    BARD_PUSH_LOGICAL( 0 );
    return;
  }

  BARD_PUSH_LOGICAL( ((memcmp(st1->characters,st2->characters,count<<1)) ? 0 : 1) );
}

void String__opADD__String()
{
  BardString* b = (BardString*) BARD_POP_REF();
  BardString* a = (BardString*) BARD_POP_REF();

  BVM_NULL_CHECK( a, return );

  BARD_PUSH_REF(a);
  BARD_PUSH_REF(b);

  BardString* result;
  int acount = a->count;
  if ( !b )
  {
    result = BardString::create( acount + 4 );
    memcpy( result->characters, a->characters, acount*2 );
    result->characters[acount]   = 'n';
    result->characters[acount+1] = 'u';
    result->characters[acount+2] = 'l';
    result->characters[acount+3] = 'l';
  }
  else
  {
    int bcount = b->count;
    result = BardString::create( acount + bcount );
    memcpy( result->characters, a->characters, acount*2 );
    memcpy( result->characters+acount, b->characters, bcount*2 );
  }
  result->set_hash_code();

  BARD_DISCARD_REF();
  BARD_DISCARD_REF();

  BARD_PUSH_REF( result );
}

void String__opADD__Char()
{
  BardChar ch = BARD_POP_CHAR();
  BardString* st = (BardString*) BARD_POP_REF();

  BVM_NULL_CHECK( st, return );
  BARD_PUSH_REF( st );

  BardString* result;
  int count = st->count;

  result = BardString::create( count + 1 );
  memcpy( result->characters, st->characters, count*2 );
  result->characters[count] = ch;
  result->set_hash_code();

  BARD_DISCARD_REF();
  BARD_PUSH_REF( result );
}

void String__substring__Int32_Int32()
{
  BardInt32 i2 = BARD_POP_INT32();
  BardInt32 i1 = BARD_POP_INT32();
  BardString* st = (BardString*) BARD_POP_REF();

  BVM_NULL_CHECK( st, return );
  BARD_PUSH_REF( st );

  if (i1 < 0) i1 = 0;
  if (i2 >= st->count) i2 = st->count - 1;

  if (i1 > i2)
  {
    BardString* result = BardString::create("");
    result->set_hash_code();

    BARD_DISCARD_REF();
    BARD_PUSH_REF( result );
    return;
  }

  int count = (i2 - i1) + 1;

  BardString* result = BardString::create(count);
  BardChar* src = (st->characters + i1) - 1;
  BardChar* dest = (result->characters) - 1;

  ++count;
  while (--count) *(++dest) = *(++src);

  result->set_hash_code();
  BARD_DISCARD_REF();
  BARD_PUSH_REF( result );
}

void String__to_Array()
{
  BardString* st = (BardString*) BARD_POP_REF();

  BVM_NULL_CHECK( st, return );
  BARD_PUSH_REF( st );

  BardArray* new_array = BARD_TYPE_ARRAY_OF_CHAR->create( st->count );
  memcpy( new_array->data, st->characters, st->count*2 );

  BARD_DISCARD_REF();
  BARD_PUSH_REF( new_array );
}

//-----------------------------------------------------------------------------
//  System
//-----------------------------------------------------------------------------
void System__force_garbage_collection()
{
  BARD_DISCARD_REF();
  mm.force_gc = true;  // at next opportunity
}

bool catch_control_c = false;
bool control_c_pressed = false;

void on_control_c( int sig_num )
{
  signal( SIGINT, on_control_c );
  if (catch_control_c) control_c_pressed = true;
  else exit(0);
}

void System__catch_control_c__Logical()
{
  BardInt32 setting = BARD_POP_INT32();
  BARD_DISCARD_REF();
  if (setting)
  {
    if ( !catch_control_c )
    {
      catch_control_c = true;
      signal( SIGINT, on_control_c );
    }
  }
  else
  {
    catch_control_c = false;
  }
  control_c_pressed = false;
}

void System__control_c_pressed()
{
  BARD_DISCARD_REF();
  BARD_PUSH_INTEGER( control_c_pressed );
  control_c_pressed = false;
}


#if !defined(ANDROID)
void System__exit_program__Int32_String()
{
  BardObject* mesg = BARD_POP_REF();
  BardInt32   exit_code = BARD_POP_INT32();
  BARD_DISCARD_REF();

  /* System::exit_program(Int32,String) */
  if (mesg)
  {
    bard_error_message = ((BardString*)mesg)->to_new_ascii();
  }
#ifdef BARD_USE_LONGJMP
  longjmp( bard_fatal_jumppoint, exit_code );
#else
  throw exit_code;
#endif
}
#endif //!defined(ANDROID)


//void System__memory_usage()
//{
  // System::memory_usage().SystemMemoryUsage
  //BARD_POP_REF();  // discard singleton

  //mm.track_stats = 1;
  //mm_collect_garbage();
  //mm.track_stats = 0;
  //BARD_PUSH_INTEGER( mm.resource_bytes );
  //BARD_PUSH_INTEGER( mm.system_object_bytes );
  //BARD_PUSH_INTEGER( mm.runtime_bytes );
  //BARD_PUSH_INTEGER( mm.num_objects );
//}

void System__get__String()
{
  BardString* name_string = (BardString*) BARD_POP_REF();
  BARD_DISCARD_REF();

  BVM_NULL_CHECK( name_string, return );

  char name[128];
  name_string->to_ascii( name, 128 );

  char* value = getenv( name );
  BARD_PUSH_REF( BardString::create(value) );
}

#if !defined(ANDROID)
void System__language()
{
  BARD_DISCARD_REF();  // singleton ptr

  if ( ! *bard_language_ref )
  {
    bard_language_ref = (BardObject*) BardString::create((char*)"en");
  }

  BARD_PUSH_REF( *bard_language_ref );
}
#endif  // !ANDROID


void System__os()
{
  BARD_DISCARD_REF();

  if ( ! *bard_os_ref )
  {
#ifdef _WIN32 
    bard_os_ref = (BardObject*) BardString::create((char*)"windows");
#elif TARGET_OS_IPHONE 
    bard_os_ref = (BardObject*) BardString::create((char*)"ios");
#elif __APPLE__ 
    bard_os_ref = (BardObject*) BardString::create((char*)"mac");
#elif ANDROID 
    bard_os_ref = (BardObject*) BardString::create((char*)"android");
#elif defined(LINUX)
    bard_os_ref = (BardObject*) BardString::create((char*)"linux");
#elif defined(UNIX)
    bard_os_ref = (BardObject*) BardString::create((char*)"unix");
#else
# error Must define System__os in bard_stdlib.cpp.
#endif
  }

  BARD_PUSH_REF( (*bard_os_ref) );
}

void System__os_version()
{
  BARD_DISCARD_REF();  // singleton ptr

  if ( ! *bard_os_version_ref )
  {
#ifdef _WIN32 
    bard_os_version_ref = (BardObject*) BardString::create((char*)"unknown");
#elif TARGET_OS_IPHONE
    bard_os_version_ref = (BardObject*) BardString::create((char*)"unknown");
#elif __APPLE__ 
    bard_os_version_ref = (BardObject*) BardString::create((char*)"unknown");
#elif ANDROID 
    bard_os_version_ref = (BardObject*) BardString::create((char*)"unknown");
#elif defined(UNIX)
    bard_os_version_ref = (BardObject*) BardString::create((char*)"unknown");
#else
    bard_os_version_ref = (BardObject*) BardString::create((char*)"unknown");
#endif
  }
  BARD_PUSH_REF( *bard_os_version_ref );
}

#if !defined(ANDROID)
void System__device_type()
{
  BARD_DISCARD_REF();  // singleton ptr

  if ( ! *bard_hw_version_ref )
  {
#ifdef _WIN32 
    bard_hw_version_ref = (BardObject*) BardString::create((char*)"unknown");
#elif TARGET_OS_IPHONE 
    size_t size;
    sysctlbyname("hw.machine",NULL,&size,NULL,0);
    char* machine = (char*) malloc(size+1);  // prolly don't need the +1 on this one
    sysctlbyname("hw.machine",machine,&size,NULL,0);
    bard_hw_version_ref = (BardObject*) BardString::create(machine);
#elif __APPLE__ 
    bard_hw_version_ref = (BardObject*) BardString::create((char*)"unknown");
#elif defined(UNIX)
    bard_hw_version_ref = (BardObject*) BardString::create((char*)"unknown");
#else
    bard_hw_version_ref = (BardObject*) BardString::create((char*)"unknown");
#endif
  }
  BARD_PUSH_REF( *bard_hw_version_ref );
}
#endif // !ANDROID

void System__raw_exe_filepath()
{
  BARD_DISCARD_REF();
#if defined(BARD_VM)
  BARD_PUSH_REF( BardString::create( bvm.raw_exe_filepath ) );
#else
  BARD_PUSH_REF( BardString::create( sxc_raw_exe_filepath ) );
#endif
}

//=============================================================================
//  WeakReference
//=============================================================================
void WeakReferenceManager__create_from__Object()
{
  BardObject* object = BARD_POP_REF();
  BARD_DISCARD_REF();
  BARD_PUSH_REF( BardWeakRef::create(object) );
}

void WeakReference__object__Object()
{
  // Set
  BardObject* object = BARD_POP_REF();
  BardWeakRef* ref = (BardWeakRef*) BARD_POP_REF();
  BVM_NULL_CHECK( ref, return );

  ref->set( object );
}

void WeakReference__object()
{
  // Get
  BardWeakRef* ref = (BardWeakRef*) BARD_POP_REF();
  BVM_NULL_CHECK( ref, return );

  BARD_PUSH_REF( ref->object );
}

