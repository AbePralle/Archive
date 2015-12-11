#ifndef BARD_TYPES_H
#define BARD_TYPES_H

#ifndef BARD_STACK_SIZE
#  define BARD_STACK_SIZE 4096
//#  define BARD_STACK_SIZE 100
#endif

#ifndef BARD_LOG
#  define BARD_LOG(mesg) printf(mesg); printf("\n")
#endif

#ifndef BARD_ALLOCATE
#  define BARD_ALLOCATE(bytes) malloc(bytes)
#endif

#ifndef BARD_FREE
#  define BARD_FREE(ptr) free(ptr)
#endif

#ifndef BARD_BYTES_BEFORE_GC
#  define BARD_BYTES_BEFORE_GC (512*1024)
#endif

#ifndef NULL
#  define NULL 0
#endif

#if defined(_WIN32)
  typedef __int32          BardInteger;
  typedef unsigned __int16 BardCharacter;
  typedef unsigned char    BardByte;
  typedef double           BardReal;
  typedef BardInteger      BardLogical;
  typedef float            BardReal32;
  typedef __int64          BardInt64;
#else
  typedef int              BardInteger;
  typedef unsigned short   BardCharacter;
  typedef unsigned char    BardByte;
  typedef double           BardReal;
  typedef BardInteger      BardLogical;
  typedef float            BardReal32;
  typedef long long        BardInt64;
#endif

struct BardVM;

typedef void (*BardNativeMethod)(struct BardVM* vm);

#define BARD_ATTRIBUTE_TYPE_MASK  3
#define BARD_ATTRIBUTE_CLASS      0
#define BARD_ATTRIBUTE_ASPECT     1
#define BARD_ATTRIBUTE_PRIMITIVE  2
#define BARD_ATTRIBUTE_COMPOUND   3

#define BARD_ATTRIBUTE_NATIVE     4
#define BARD_ATTRIBUTE_SINGLETON  16

#define BardType_is_class( type )     ((type->attributes & BARD_ATTRIBUTE_TYPE_MASK) == BARD_ATTRIBUTE_CLASS)
#define BardType_is_aspect( type )    ((type->attributes & BARD_ATTRIBUTE_TYPE_MASK) == BARD_ATTRIBUTE_ASPECT)
#define BardType_is_reference( type ) ((type->attributes & BARD_ATTRIBUTE_TYPE_MASK) <= BARD_ATTRIBUTE_ASPECT)
#define BardType_is_primitive( type ) ((type->attributes & BARD_ATTRIBUTE_TYPE_MASK) == BARD_ATTRIBUTE_PRIMITIVE)
#define BardType_is_compound( type )  ((type->attributes & BARD_ATTRIBUTE_TYPE_MASK) == BARD_ATTRIBUTE_COMPOUND)
#define BardType_is_native( type )    (type->attributes & BARD_ATTRIBUTE_NATIVE)
#define BardType_is_singleton( type ) (type->attributes & BARD_ATTRIBUTE_SINGLETON)

#define BardMethod_is_native( m )    (m->attributes & BARD_ATTRIBUTE_NATIVE)

#define BardVM_push_object(vm,obj)         (--vm->sp)->object = (BardObject*) obj
#define BardVM_push_c_string(vm,st)    BardVM_push_object( vm, BardString_create_with_c_string(vm,st) )
#define BardVM_push_c_string_with_length(vm,st,len)    BardVM_push_object( vm, BardString_create_with_c_string(vm,st,len) )
#define BardVM_push_real( vm, value )      (--vm->sp)->real = value
#define BardVM_push_integer( vm, value )   (--vm->sp)->integer = value
#define BardVM_push_character( vm, value ) (--vm->sp)->integer = value
#define BardVM_push_logical( vm, value )   (--vm->sp)->integer = value

#define BardVM_pop_discard( vm )              ++vm->sp
#define BardVM_pop_object( vm )     ((vm->sp++)->object)
#define BardVM_pop_string( vm )     ((BardString*)((vm->sp++)->object))
#define BardVM_pop_list( vm )       (BardArrayList*) ((vm->sp++)->object)
#define BardVM_pop_real( vm )       ((vm->sp++)->real)
#define BardVM_pop_integer( vm )    ((vm->sp++)->integer)
#define BardVM_pop_character( vm )  ((BardCharacter)((vm->sp++)->integer))
#define BardVM_pop_logical( vm )    ((BardInteger)((vm->sp++)->integer))

#define BardVM_peek_object( vm )  (vm->sp->object)
#define BardVM_peek_string( vm )  ((BardString*)(vm->sp->object))
#define BardVM_peek_list( vm )    ((BardArrayList*) vm->sp->object)
#define BardVM_peek_real( vm )    (vm->sp->real)
#define BardVM_peek_integer( vm ) (vm->sp->integer)
#define BardVM_peek_logical( vm ) (vm->sp->integer)

#define BardList_data(list)  (*((BardArray**)(((char*)list) + list->type->vm->BardList_data_offset)))
#define BardList_count(list) (*((BardInteger*)(((char*)list) + list->type->vm->BardList_count_offset)))
#define BardList_capacity(list) (BardList_data(list)->count)

#define BARD_CREATE_OP_WITH_ARG_N(op,n) ((n << 10) | op)

#endif
