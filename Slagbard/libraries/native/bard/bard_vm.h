//=============================================================================
//  bard_vm.h
//
//  Bard Virtual Machine Header
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
//    2010.11.26 / Abe Pralle - v3.2 revamp
//    2008.12.17 / Abe Pralle - Created
//=============================================================================
#ifndef BARD_VM_H
#define BARD_VM_H

#include "bard.h"

typedef int BardOpcode;

//=============================================================================
//  StringBuilder
//=============================================================================
struct StringBuilder : ArrayList<char>
{
  void print( const char* st )
  {
    while (*st) add(*(st++));
  }

  void print( char ch ) { add(ch); }

  void print( int n )
  {
    if (n < 0)
    {
      print('-');
      print(-n);
    }
    else
    {
      if (n > 9)
      {
        print(n/10);
      }
      add('0'+(n%10));
    }
  }

  char* to_new_ascii()
  {
    char* buffer = new char[count+1];
    memcpy( buffer, data, count );
    buffer[count] = 0;
    return buffer;
  }
};

//=============================================================================
//  Bard Structures
//=============================================================================
struct BardTypeInfo;

struct BardLocalVarInfo
{
  BardTypeInfo* type;
  int offset;

  BardLocalVarInfo( BardTypeInfo* type ) : type(type), offset(0)
  {
  }
};

struct BardParameterList
{
  AllocList<BardLocalVarInfo*> parameters;
  int num_ref_params;
  int num_data_params;

  BardParameterList( int n ) : num_ref_params(0), num_data_params(0)
  {
    parameters.ensure_capacity(n);
  }

  inline int count() { return parameters.count; }
  inline BardLocalVarInfo*& operator[](int index) { return parameters[index]; }
  inline BardLocalVarInfo*& get(int index) { return parameters[index]; }
};

struct BardPropertyInfo
{
  BardTypeInfo* type;
  char*         name;
  int           offset;

  BardPropertyInfo( BardTypeInfo* type ) : type(type), name(NULL), offset(0)
  {
  }
};

struct BardSourcePos
{
  int code_offset;
  int source_pos;
    // If pos <= 0 then |pos| is filename index, else is line #

  BardSourcePos() : code_offset(0), source_pos(0)
  {
  }

  BardSourcePos( int code_offset, int source_pos ) 
    : code_offset(code_offset), source_pos(source_pos)
  {
  }
};

struct BardCatchInfo
{
  BardTypeInfo* type_caught;
  BardInt32 begin_offset;
  BardInt32 end_offset;
  BardInt32 handler;

  BardCatchInfo( BardTypeInfo* type_caught, BardInt32 begin_offset, BardInt32 end_offset,
      BardInt32 handler ) : type_caught(type_caught), begin_offset(begin_offset),
      end_offset(end_offset), handler(handler)
  {
  }
};

struct BardMethodInfo
{
  char* name;
  char* signature;
  int   index;
  int   qualifiers;
  int   num_local_refs;
  int   local_data_size;
  BardTypeInfo*              type_context;
  BardTypeInfo*              return_type;
  BardParameterList*         parameters;
  AllocList<BardCatchInfo*>    catches;
  AllocList<BardLocalVarInfo*> local_vars;

  BardOpcode*    bytecode;
  int            bytecode_offset;
  int            bytecode_limit;
  int            source_pos_offset;
  BardNativeFn   native_handler;

  BardMethodInfo() : signature(0), num_local_refs(0), local_data_size(0), 
      native_handler(NULL)
  {
  }

  ~BardMethodInfo()
  {
    if (signature)
    {
      delete signature;
      signature = NULL;
    }
  }

  void create_signature();
};

struct BardGenericObject;

struct BardTypeInfo : BardTypeInfoBase
{
  BardTypeInfo* element_type; // arrays only

  AllocList<BardPropertyInfo*> properties;
  ArrayList<BardMethodInfo*>   methods;
  ArrayList<BardMethodInfo*>   dispatch_table;

  int num_reference_properties;
  int stack_slots;  // # slots on the stack (pointer size for objects)

  BardMethodInfo* method_init_object;
  BardMethodInfo* method_clean_up;

  BardTypeInfo() : element_type(NULL), num_reference_properties(0)
  {
    method_init_object = NULL;
    method_clean_up = NULL;
  }

  bool is_real();
  bool is_integer();
  bool is_int32_64();
  bool is_logical();

  BardMethodInfo* find_method( const char* sig );
  BardMethodInfo* must_find_method( const char* sig );
  BardPropertyInfo* find_property( const char* name, BardTypeInfo* required_type=NULL );
  BardPropertyInfo* must_find_property( const char* name, BardTypeInfo* required_type=NULL );

  BardObject* singleton();

  BardObject* create();
  BardArray*  create( int array_count );
};

struct BardGenericObject : BardObject
{
  BardInt64  data[1];  // Int64 forces this member to have 8-byte alignment.

  int property_offset( const char* name );
  void* property_address( const char* name );
  void* property_address( int offset );
  void set_ref( const char* id, BardObject* object );
  void set_ref( int offset, BardObject* object );
  BardObject* get_ref( const char* name );
  BardObject* get_ref( int offset );
};


void bard_assert( bool value, const char* st );
void bard_assert( bool value, const char* st1, const char* st2, const char* st3 );
void bard_throw_exception_of_type( BardTypeInfo* type );


//=============================================================================
// BardVM
//=============================================================================
#define BARDOP_HALT                        0
#define BARDOP_NOP                         1
#define BARDOP_BREAKPOINT                  2
#define BARDOP_MISSING_RETURN_ERROR        3
#define BARDOP_RETURN_NIL                  4
#define BARDOP_RETURN_REF                  5
#define BARDOP_RETURN_8                    6
#define BARDOP_RETURN_X                    7
#define BARDOP_DUPLICATE_REF               8
#define BARDOP_DUPLICATE_8                 9
#define BARDOP_POP_REF                    10
#define BARDOP_POP_8                      11
#define BARDOP_POP_X                      12
#define BARDOP_JUMP                       13
#define BARDOP_JUMP_IF_TRUE               14
#define BARDOP_JUMP_IF_FALSE              15
#define BARDOP_JUMP_IF_REF                16
#define BARDOP_JUMP_IF_NULL_REF           17
#define BARDOP_THROW                      18
#define BARDOP_LITERAL_STRING             19
#define BARDOP_LITERAL_NULL               20
#define BARDOP_LITERAL_8                  21
#define BARDOP_LITERAL_4                  22
#define BARDOP_LITERAL_REAL32             23
#define BARDOP_LITERAL_INTEGER_1          24
#define BARDOP_LITERAL_INTEGER_0          25
#define BARDOP_LITERAL_INTEGER_NEG1       26
#define BARDOP_LITERAL_REAL_1             27
#define BARDOP_LITERAL_REAL_0             28
#define BARDOP_LITERAL_REAL_NEG1          29
#define BARDOP_READ_THIS_REF              30
#define BARDOP_READ_SINGLETON_REF         31
#define BARDOP_READ_PROPERTY_REF          32
#define BARDOP_READ_PROPERTY_1U           33
#define BARDOP_READ_PROPERTY_2U           34
#define BARDOP_READ_PROPERTY_4            35
#define BARDOP_READ_PROPERTY_REAL32       36
#define BARDOP_READ_PROPERTY_8            37
#define BARDOP_READ_PROPERTY_X            38
#define BARDOP_READ_THIS_PROPERTY_REF     39
#define BARDOP_READ_THIS_PROPERTY_4       40
#define BARDOP_READ_THIS_PROPERTY_REAL32  41
#define BARDOP_READ_THIS_PROPERTY_8       42
#define BARDOP_READ_THIS_PROPERTY_X       43
#define BARDOP_READ_LOCAL_REF             44
#define BARDOP_READ_LOCAL_8               45
#define BARDOP_READ_LOCAL_X               46
#define BARDOP_READ_COMPOUND_8            47
#define BARDOP_READ_COMPOUND_X            48
#define BARDOP_WRITE_SINGLETON_REF        49
#define BARDOP_WRITE_PROPERTY_REF         50
#define BARDOP_WRITE_PROPERTY_1           51
#define BARDOP_WRITE_PROPERTY_2           52
#define BARDOP_WRITE_PROPERTY_4           53
#define BARDOP_WRITE_PROPERTY_REAL32      54
#define BARDOP_WRITE_PROPERTY_8           55
#define BARDOP_WRITE_PROPERTY_X           56
#define BARDOP_WRITE_THIS_PROPERTY_REF    57
#define BARDOP_WRITE_THIS_PROPERTY_4      58
#define BARDOP_WRITE_THIS_PROPERTY_REAL32 59
#define BARDOP_WRITE_THIS_PROPERTY_8      60
#define BARDOP_WRITE_THIS_PROPERTY_X      61
#define BARDOP_WRITE_LOCAL_REF            62
#define BARDOP_WRITE_LOCAL_8              63
#define BARDOP_WRITE_LOCAL_X              64
#define BARDOP_FAUX_STATIC_CALL           65
#define BARDOP_STATIC_CALL                66
#define BARDOP_DYNAMIC_CALL               67
#define BARDOP_NATIVE_CALL                68
#define BARDOP_NEW_OBJECT                 69
#define BARDOP_NEW_OBJECT_NO_INIT         70
#define BARDOP_NEW_ARRAY                  71
#define BARDOP_ARRAY_READ_REF             72
#define BARDOP_ARRAY_READ_1U              73
#define BARDOP_ARRAY_READ_2U              74
#define BARDOP_ARRAY_READ_4               75
#define BARDOP_ARRAY_READ_REAL32          76
#define BARDOP_ARRAY_READ_8               77
#define BARDOP_ARRAY_READ_X               78
#define BARDOP_ARRAY_WRITE_REF            79
#define BARDOP_ARRAY_WRITE_1              80
#define BARDOP_ARRAY_WRITE_2              81
#define BARDOP_ARRAY_WRITE_4              82
#define BARDOP_ARRAY_WRITE_REAL32         83
#define BARDOP_ARRAY_WRITE_8              84
#define BARDOP_ARRAY_WRITE_X              85
#define BARDOP_ARRAY_DUPLICATE            86
#define BARDOP_TYPECHECK                  87
#define BARDOP_AS_REF                     88
#define BARDOP_CAST_REAL_TO_INTEGER       89
#define BARDOP_CAST_INTEGER_TO_REAL       90
#define BARDOP_CAST_REAL_TO_LOGICAL       91
#define BARDOP_CAST_INTEGER_TO_LOGICAL    92
#define BARDOP_CAST_INTEGER_TO_I32        93
#define BARDOP_CAST_INTEGER_TO_CHAR       94
#define BARDOP_CAST_INTEGER_TO_BYTE       95
#define BARDOP_CMP_INSTANCE_OF            96
#define BARDOP_CMP_EQ_REF                 97
#define BARDOP_CMP_NE_REF                 98
#define BARDOP_CMP_EQ_REF_NULL            99
#define BARDOP_CMP_NE_REF_NULL            100
#define BARDOP_CMP_EQ_X                   101
#define BARDOP_CMP_NE_X                   102
#define BARDOP_CMP_EQ_INTEGER             103
#define BARDOP_CMP_NE_INTEGER            104
#define BARDOP_CMP_GT_INTEGER            105
#define BARDOP_CMP_GE_INTEGER            106
#define BARDOP_CMP_LT_INTEGER            107
#define BARDOP_CMP_LE_INTEGER            108
#define BARDOP_CMP_EQ_REAL               109
#define BARDOP_CMP_NE_REAL               110
#define BARDOP_CMP_GT_REAL               111
#define BARDOP_CMP_GE_REAL               112
#define BARDOP_CMP_LT_REAL               113
#define BARDOP_CMP_LE_REAL               114
#define BARDOP_NOT_INTEGER               115
#define BARDOP_NOT_LOGICAL               116
#define BARDOP_NEGATE_INTEGER            117
#define BARDOP_NEGATE_REAL               118
#define BARDOP_AND_LOGICAL               119
#define BARDOP_OR_LOGICAL                120
#define BARDOP_XOR_LOGICAL               121
#define BARDOP_ADD_INTEGER               122
#define BARDOP_SUB_INTEGER               123
#define BARDOP_MUL_INTEGER               124
#define BARDOP_DIV_INTEGER               125
#define BARDOP_MOD_INTEGER               126
#define BARDOP_EXP_INTEGER               127
#define BARDOP_AND_INTEGER               128
#define BARDOP_OR_INTEGER                129
#define BARDOP_XOR_INT64                 130
#define BARDOP_SHL_INT64                 131
#define BARDOP_SHR_INT64                 132
#define BARDOP_SHRX_INT64                133
#define BARDOP_XOR_INT32                 134
#define BARDOP_SHL_INT32                 135
#define BARDOP_SHR_INT32                 136
#define BARDOP_SHRX_INT32                137
#define BARDOP_ADD_REAL                  138
#define BARDOP_SUB_REAL                  139
#define BARDOP_MUL_REAL                  140
#define BARDOP_DIV_REAL                  141
#define BARDOP_MOD_REAL                  142
#define BARDOP_EXP_REAL                  143
#define BARDOP_THIS_ADD_ASSIGN_I32       144
#define BARDOP_THIS_SUB_ASSIGN_I32       145
#define BARDOP_THIS_MUL_ASSIGN_I32       146
#define BARDOP_THIS_INCREMENT_I32        147
#define BARDOP_THIS_DECREMENT_I32        148
#define BARDOP_THIS_ADD_ASSIGN_R64       149
#define BARDOP_THIS_SUB_ASSIGN_R64       150
#define BARDOP_THIS_MUL_ASSIGN_R64       151
#define BARDOP_THIS_INCREMENT_R64        152
#define BARDOP_THIS_DECREMENT_R64        153
#define BARDOP_LOCAL_ADD_ASSIGN_INTEGER  154
#define BARDOP_LOCAL_SUB_ASSIGN_INTEGER  155
#define BARDOP_LOCAL_MUL_ASSIGN_INTEGER  156
#define BARDOP_LOCAL_INCREMENT_INTEGER   157
#define BARDOP_LOCAL_DECREMENT_INTEGER   158
#define BARDOP_LOCAL_ADD_ASSIGN_REAL     159
#define BARDOP_LOCAL_SUB_ASSIGN_REAL     160
#define BARDOP_LOCAL_MUL_ASSIGN_REAL     161
#define BARDOP_LOCAL_INCREMENT_REAL      162
#define BARDOP_LOCAL_DECREMENT_REAL      163

#define BARD_PUSH_REF( obj ) bvm.regs.stack.refs[-1] = (obj); --bvm.regs.stack.refs
#define BARD_PEEK_REF() (*(bvm.regs.stack.refs))
#define BARD_POP_REF() *(bvm.regs.stack.refs++)
#define BARD_DISCARD_REF() bvm.regs.stack.refs++
#define BARD_DUPLICATE_REF() bvm.regs.stack.refs[-1] = *bvm.regs.stack.refs; --bvm.regs.stack.refs

#define BARD_POP_INTEGER()   *(bvm.regs.stack.data++)
#define BARD_PUSH_INTEGER(i) *(--bvm.regs.stack.data) = i
#define BARD_PEEK_INTEGER() *(bvm.regs.stack.data)

#define BARD_POP_REAL()   *(BardReal64*)(bvm.regs.stack.data++)
#define BARD_PUSH_REAL(r) *(BardReal64*)(--bvm.regs.stack.data) = r
#define BARD_PEEK_REAL() *(BardReal64*)(bvm.regs.stack.data)

#define BARD_PUSH(type,value) *((type*)(bvm.regs.stack.data -= sizeof(type)/8)) = (value)
#define BARD_PEEK(type) *((type*)bvm.regs.stack.data)
#define BARD_POP(type) ((type*)(bvm.regs.stack.data += sizeof(type)/8))[-1]


#define BVM_DEREFERENCE(context,offset,property_type) *((property_type*)(((char*)context)+offset))

#define BARD_FIND_TYPE( var_name, type_name ) \
  BardTypeInfo* var_name; \
  { \
    static int type_index = -1; \
    if (type_index == -1) type_index = bvm.must_find_type(type_name)->index; \
    var_name = bvm.types[type_index]; \
  }

#define BARD_GET_REF(var_name,obj,name) \
  BARD_GET(BardObject*,var_name,obj,name)

#define BARD_SET_REF(obj,name,value) \
  {\
    static int property_offset = -1; \
    if (property_offset == -1) property_offset = (obj)->type->must_find_property(name)->offset; \
    ((BardGenericObject*)(obj))->set_ref( property_offset, value ); \
  }

#define BARD_GET(vartype,var_name,obj,name) \
  vartype var_name; \
  {\
    static int property_offset = -1; \
    if (property_offset == -1) property_offset = (obj)->type->must_find_property(name)->offset; \
    var_name = *((vartype*)(((BardGenericObject*)(obj))->property_address( property_offset ))); \
  }

#define BARD_SET(vartype,obj,name,value) \
  {\
    static int property_offset = -1; \
    if (property_offset == -1) property_offset = (obj)->type->must_find_property(name)->offset; \
    *((vartype*)(((BardGenericObject*)(obj))->property_address(property_offset))) = value; \
  }

#define BARD_CALL( type, sig ) \
  {\
    static int method_index = -1; \
    if (method_index == -1) method_index = type->must_find_method(sig)->index; \
    bvm.call( bvm.methods[method_index] ); \
  }

#define BARDCODE_OPERAND_I32() *(bvm.regs.ip++)
#define BARDCODE_OPERAND_I64() bvm.literal_table[*(bvm.regs.ip++)]
#define BARDCODE_OPERAND_ADDR() bvm.address_table[*(bvm.regs.ip++)]

#define BVM_ASSERT(obj,err_type,cmd) if (!(obj)) { bvm.throw_exception(err_type); cmd; }

#define BVM_NULL_CHECK(obj,cmd) if (!(obj)) { bvm.throw_exception(bvm.type_null_reference_error); cmd; }

#define BVM_THROW_TYPE(err_type,cmd) { bvm.throw_exception(err_type); cmd; }

#define BARDCODE_READ_PROPERTY(property_type) \
  { \
    BardObject* context = BARD_POP_REF( ); \
    BVM_ASSERT( context, bvm.type_null_reference_error, continue ); \
    BARD_PUSH_INTEGER( BVM_DEREFERENCE(context,BARDCODE_OPERAND_I32(),property_type) ); \
  }

#define BARDCODE_WRITE_PROPERTY(property_type) \
  { \
    BardInt64   new_value = BARD_POP_INTEGER(); \
    BardObject* context = BARD_POP_REF( ); \
    BVM_ASSERT( context, bvm.type_null_reference_error, continue ); \
    BVM_DEREFERENCE(context,BARDCODE_OPERAND_I32(),property_type) = (property_type) new_value; \
  }

#define BARDCODE_WRITE_THIS_PROPERTY(property_type) \
    BVM_DEREFERENCE(bvm.regs.frame.refs[-1],BARDCODE_OPERAND_I32(),property_type) = (property_type) BARD_POP_INTEGER()

#define BARD_TYPE_STRING bvm.type_string
#define BARD_TYPE_WEAK_REFERENCE bvm.type_weak_reference
#define BARD_TYPE_ARRAY_OF_CHAR bvm.type_array_of_char

#define BARD_REF_STACK_PTR bvm.regs.stack.refs
#define BARD_REF_STACK_LIMIT bvm.ref_stack_limit
#define BARD_SINGLETONS bvm.singletons
#define BARD_SINGLETONS_COUNT bvm.singletons.count


struct BardStackPointers
{
  BardInt64*     data;
  BardObject**   refs;
};

/*
struct BardCatchInfo
{
  BardOpcode*    begin_addr;
  BardOpcode*    end_addr;
  BardTypeInfo* type_caught;
  BardOpcode*    handler;
};
*/

struct BardCallFrame
{
  BardMethodInfo*     called_method;
  BardOpcode*         return_address;
  BardStackPointers   prior_frame;
};

struct BardRegisters
{
  BardStackPointers stack;
  BardStackPointers frame;
  BardOpcode*       ip;
  BardCallFrame*    call_frame;
};

struct BardNativeFnHook
{
  const char* class_name;
  const char* signature;
  BardNativeFn fn;

  BardNativeFnHook() : class_name(NULL), signature(NULL), fn(NULL) { }

  BardNativeFnHook( const char* _class_name, const char* _signature, BardNativeFn _fn )
    : class_name(_class_name), signature(_signature), fn(_fn) { }
};

struct BardVM
{
  BardRegisters  regs;

  BardObject**   min_ref_stack;
  BardInt64*     min_data_stack;
  BardCallFrame* min_call_stack;

  BardObject**   ref_stack;
  BardInt64*     data_stack;
  BardCallFrame* call_stack;

  BardObject**   ref_stack_limit;
  BardInt64*     data_stack_limit;
  BardCallFrame* call_stack_limit;

  ArrayList<BardOpcode>    code;
  ArrayList<BardInt64>     literal_table;  // 64-bit literals
  ArrayList<int>           address_offsets;
  ArrayList<void*>         address_table;
  ArrayList<BardSourcePos> line_table;
  ArrayList<BardObject*>   singletons;

  ArrayList<BardNativeFnHook> native_method_hooks;

  AllocList<char*> filenames;
  AllocList<char*> identifiers;
  AllocList<BardTypeInfo*> types;
  AllocList<BardParameterList*> parameter_table;
  AllocList<BardMethodInfo*>    methods;
  ArrayList<BardObject*>        strings;
  BardTypeInfo*  main_class;

  BardTypeInfo* type_object;
  BardTypeInfo* type_int64;
  BardTypeInfo* type_int32;
  BardTypeInfo* type_char;
  BardTypeInfo* type_byte;
  BardTypeInfo* type_real64;
  BardTypeInfo* type_real32;
  BardTypeInfo* type_logical;
  BardTypeInfo* type_null;
  BardTypeInfo* type_string;
  BardTypeInfo* type_requires_cleanup;
  BardTypeInfo* type_native_data;
  BardTypeInfo* type_system;
  BardTypeInfo* type_weak_reference;
  BardTypeInfo* type_array_of_char;
  BardTypeInfo* type_array_of_int64;
  BardTypeInfo* type_missing_return_error;
  BardTypeInfo* type_type_cast_error;
  BardTypeInfo* type_out_of_bounds_error;
  BardTypeInfo* type_divide_by_zero_error;
  BardTypeInfo* type_null_reference_error;
  BardTypeInfo* type_stack_limit_error;
  BardTypeInfo* type_file_error;
  BardTypeInfo* type_file_not_found_error;
  BardTypeInfo* type_no_next_value_error;
  BardTypeInfo* type_socket_error;
  BardObject*   string_invalid_directory;
  BardTypeInfo* type_invalid_operand_error;

  char* raw_exe_filepath;

  bool        initialized;
  BardOpcode* exception_ip;

  BardVM() : ref_stack(NULL), data_stack(NULL), call_stack(NULL)
  {
    initialized = false;
    init();
    raw_exe_filepath = (char*) "unavailable";
  }

  ~BardVM();

  void init()
  {
    if (initialized) return;
    initialized = true;
    exception_ip = NULL;
    register_stdlib();
  }

  void shut_down();

  BardObject*    create_type( const char* name );
  BardTypeInfo*  find_type( const char* name );
  BardTypeInfo*  must_find_type( const char* name );

  void register_native_method( const char* class_name, const char* sig, BardNativeFn fn );
  void register_stdlib();
  void apply_hooks();

  void set_command_line_args( char* args[], int count );
  void prep_types_and_methods();
  void configure();
  void create_singletons();
  void call( BardMethodInfo* method );
  void invoke( BardMethodInfo* method );
  void execute();

  void throw_exception_on_stack();
  void throw_exception( BardTypeInfo* type, const char* mesg=NULL );
  void throw_stack_limit_error();
};

extern BardVM bvm;


//=============================================================================
//  Loader
//=============================================================================
void BardVM_unhandled_native_method();

//-----------------------------------------------------------------------------
//  BardReader
//-----------------------------------------------------------------------------
struct BardReader
{
  const unsigned char* data;
  int   pos;
  int   remaining;
  int   total_size;
  bool  free_data;

  BardReader() : data(NULL), free_data(false) { }
  ~BardReader();

  void init( const char* filename );
  void init( const unsigned char* data, int size );
  int  read();
  int  read16();
  int  read32();
  BardInt64 read64();
  int  readX();
  int  read_utf8();
  char* read_new_ascii();
};

struct BardCodeLabel
{
  const char* name;
  int id;
  int n;
  int address_index;

  BardCodeLabel() : name(NULL), id(0), n(0) { }
  BardCodeLabel( const char* name, int id, int n, int address_index=0 ) 
    : name(name), id(id), n(n), address_index(address_index) { }

  bool equals( BardCodeLabel other )
  {
    return (name == other.name && id == other.id && n == other.n);
  }
};

struct BardOpInfo
{
  int op;
  int code_offset;

  BardOpInfo() : op(0), code_offset(0)
  {
  }

  BardOpInfo( int op, int code_offset ) : op(op), code_offset(code_offset)
  {
  }
};

struct BardLoader
{
  BardReader reader;
  BardMethodInfo* this_method;
  int cur_line;
  int next_structure_id;
  int next_auto_id;
  ArrayList<BardCodeLabel> resolved_labels;
  ArrayList<BardCodeLabel> unresolved_labels;
  ArrayList<BardOpInfo>    op_history;

  BardLoader() : this_method(NULL), cur_line(0)
  {
  }

  void load( const char* filename );
  void load( const char* data, int count );

  void load();
  void load_version();
  void load_filenames();
  void load_identifiers();
  void load_type_info();
  void load_parameter_info();
  void load_method_info();
  void load_type_defs();
  void load_string_table();
  void load_method_defs();

  void load_method_body( BardMethodInfo* m );

  BardTypeInfo*     load_type();
  BardMethodInfo*   load_method();
  char*             load_id();
  BardLocalVarInfo* load_local();
  BardPropertyInfo* load_this_property();

  void must_consume_header( const char* header_id );

  void write_op( int op );
  void write_i32( int i32 );
  void write_op_i32( int op, BardInt32 value );
  void write_op_i64( int op, BardInt64 value );
  int  history( int num_back=1 );
  void undo_op( int count=1 );

  void load_statement_list();
  BardTypeInfo* load_statement();
  BardTypeInfo* load_expression();
  BardTypeInfo* load_ref_expression();
  void          load_logical_expression();
  void          load_int32_expression();
  BardTypeInfo* load_compound_expression();
  int  get_next_structure_id();
  void define_label( const char* name, int id, int n ); 
  bool define_transient_label( const char* name, int id, int n, int* index_ptr=NULL ); 
  void write_label_address( const char* name, int id, int n ); 
  void close_label_id( int id );
  void write_jump_if_true( const char* name, int id, int n );
  void write_jump_if_false( const char* name, int id, int n );
};

void bard_set_raw_exe_filepath( char* filepath );
void bard_set_command_line_args( char** argv, int argc );
void bard_throw_file_error();
void bard_throw_file_error( char* filename );
void bard_throw_file_not_found_error( char* filename );
void bard_throw_no_next_value_error();
void bard_throw_socket_error();
void bard_throw_invalid_operand_error();

#endif // BARD_VM_H

