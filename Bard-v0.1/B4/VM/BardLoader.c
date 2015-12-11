#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "Bard.h"


int BardLoader_load_bc_from_reader( BardVM* vm, BardReader* reader )
{
  BARD_TRY(vm)
    if ( !BardReader_consume(reader,"BARDCODE") )
    {
      BardVM_throw_error( vm, 1, "Not a BARDCODE file." );
    }

    {
      int bc_version = BardReader_read_integer( reader );
      if (bc_version != BARD_CURRENT_BC_VERSION)
      {
        char st[80];
        sprintf( st, "Unsupported BC version: %d", bc_version );
        BardVM_throw_error( vm, 1, st );
      }
    }

    // Load code chunk
    {
      int i;
      vm->code_size = BardLoader_load_count( vm, reader );

      vm->code = (BardInteger*) BARD_ALLOCATE( sizeof(BardInteger) * vm->code_size );
      for (i=0; i<vm->code_size; ++i)
      {
        vm->code[i] = BardReader_read_integer( reader );
      }

      if (vm->code[0] != 0) BardVM_throw_error( vm, 1, "code[0] must be HALT." );
    }

    // Read identifiers
    {
      int i;
      vm->identifier_count = BardReader_read_integer( reader );
      vm->identifiers = (char**) BARD_ALLOCATE( sizeof(char*) * vm->identifier_count );
      for (i=0; i<vm->identifier_count; ++i)
      {
        char* id = BardReader_read_id( reader );
        vm->identifiers[i] = id;
      }
    }

    // Read type declarations
    {
      int i;
      vm->type_count = BardReader_read_integer( reader );
      vm->types = (BardType**) BARD_ALLOCATE( sizeof(BardType*) * vm->type_count );

      // Allocate placeholder objects for every type so that "forward references" to types
      // work during loading.
      for (i=0; i<vm->type_count; ++i)
      {
        vm->types[i] = BardType_create( vm );
      }

      for (i=0; i<vm->type_count; ++i)
      {
        BardType* type = vm->types[i];
        char*     name = BardLoader_load_indexed_id( vm, reader );

        type = BardType_init( type, name );

        // Read attribute flags
        type->attributes = BardReader_read_integer( reader );

        // Load attribute tags
        type->tag_count = BardLoader_load_count( vm, reader );

        if (type->tag_count > 0)
        {
          int t;
          type->tags = (char**) BARD_ALLOCATE( sizeof(char*) * type->tag_count );
          for (t=0; t<type->tag_count; ++t)
          {
            type->tags[t] = BardLoader_load_indexed_id( vm, reader );
          }
        }

        // Load base types
        type->base_type_count = BardLoader_load_count( vm, reader );
        if (type->base_type_count > 0)
        {
          int b;
          type->base_types = (BardType**) BARD_ALLOCATE( sizeof(BardType*) * type->base_type_count );
          for (b=0; b<type->base_type_count; ++b)
          {
            type->base_types[b] = BardLoader_load_indexed_type( vm, reader );
          }
        }

        // Load settings list
        type->settings_count = BardLoader_load_count( vm, reader );
        if (type->settings_count > 0)
        {
          int s;
          type->settings = (BardProperty*) BARD_ALLOCATE( sizeof(BardProperty) * type->settings_count );
          for (s=0; s<type->settings_count; ++s)
          {
            BardProperty* property = type->settings + s;
            property->name = BardLoader_load_indexed_id( vm, reader );
            property->type = BardLoader_load_indexed_type( vm, reader );
          }
        }

        // Load property list
        type->property_count = BardLoader_load_count( vm, reader );
        if (type->property_count > 0)
        {
          int p;
          type->properties = (BardProperty*) BARD_ALLOCATE( sizeof(BardProperty) * type->property_count );
          for (p=0; p<type->property_count; ++p)
          {
            BardProperty* property = type->properties + p;
            property->name = BardLoader_load_indexed_id( vm, reader );
            property->type = BardLoader_load_indexed_type( vm, reader );
          }
        }
      }
    }

    vm->main_class = BardLoader_load_indexed_type( vm, reader );

    BardVM_set_sizes_and_offsets( vm );

    // Read method definitions
    {
      int i;
      vm->method_count = BardReader_read_integer( reader );
      vm->methods = (BardMethod**) BARD_ALLOCATE( sizeof(BardMethod*) * vm->method_count );
      for (i=0; i<vm->method_count; ++i)
      {
        BardType* type_context;
        BardType* return_type;
        char*     name;
        int       parameter_count;
        int       local_count;

        type_context = BardLoader_load_indexed_type( vm, reader );

        name = BardLoader_load_indexed_id( vm, reader );

        parameter_count = BardLoader_load_count( vm, reader );
        return_type = BardLoader_load_indexed_type( vm, reader );
        local_count = BardLoader_load_count( vm, reader );

        BardMethod* m = BardMethod_create( i, type_context, name, parameter_count, return_type, local_count );
        vm->methods[i] = m;

        {
          int p;
          for (p=0; p<local_count; ++p)
          {
            BardType* local_type = BardLoader_load_indexed_type( vm, reader );
            m->local_types[p] = local_type;

            //if (p > 0) printf(",");
            //printf("%s",local_type->name);
          }
        }

        m->ip_start = BardReader_read_integer(reader);
        m->ip_limit = BardReader_read_integer(reader);
        m->ip = vm->code + m->ip_start - 1;
        m->line_info_index = BardReader_read_integer( reader );

        // Load method attributes

        // Flags
        m->attributes = BardReader_read_integer( reader );

        // Load attribute tags
        m->tag_count = BardLoader_load_count( vm, reader );

        if (m->tag_count > 0)
        {
          int t;
          m->tags = (char**) BARD_ALLOCATE( sizeof(char*) * m->tag_count );
          for (t=0; t<m->tag_count; ++t)
          {
            m->tags[t] = BardLoader_load_indexed_id( vm, reader );
          }
        }

        // Load exception handler table
        m->exception_handler_count = BardLoader_load_count( vm, reader );
        if (m->exception_handler_count)
        {
          int e;
          int size = sizeof(BardExceptionHandlerInfo) * m->exception_handler_count;
          m->exception_handlers = (BardExceptionHandlerInfo*) BARD_ALLOCATE( size );
          for (e=0; e<m->exception_handler_count; ++e)
          {
            BardExceptionHandlerInfo* info = m->exception_handlers + e;
            info->ip_start = vm->code + BardLoader_load_count( vm, reader );
            info->ip_limit = vm->code + BardLoader_load_count( vm, reader );
            info->catch_count = BardLoader_load_count( vm, reader );
            if (info->catch_count)
            {
              int c;
              info->catches = (BardExceptionHandlerCatchInfo*) BARD_ALLOCATE( sizeof(BardExceptionHandlerCatchInfo) * info->catch_count );
              for (c=0; c<info->catch_count; ++c)
              {
                BardExceptionHandlerCatchInfo* cur_catch = info->catches + c;
                cur_catch->catch_type = BardLoader_load_indexed_type( vm, reader );

                cur_catch->handler_ip = vm->code + (BardLoader_load_count( vm, reader ) - 1);
//printf( "Loaded handler ip %d\n", (int)(cur_catch->handler_ip - vm->code) );
                cur_catch->local_slot_index = BardReader_read_integer( reader );
              }
            }
            else
            {
              info->catches = NULL;
            }
          }
        }
      }
    }

    // Read per-type method tables
    {
      int i;
      int type_count = BardReader_read_integer( reader );
      if (type_count != vm->type_count)
      {
        BardVM_throw_error( vm, 1, "Mismatched type count for type declarations vs. type definitions." );
      }

      for (i=0; i<type_count; ++i)
      {
        BardType* type = vm->types[i];

        // Load method list
        type->method_count = BardLoader_load_count( vm, reader );
        if (type->method_count > 0)
        {
          int m;
          type->methods = (BardMethod**) BARD_ALLOCATE( sizeof(BardMethod*) * type->method_count );
          for (m=0; m<type->method_count; ++m)
          {
            type->methods[m] = BardLoader_load_indexed_method( vm, reader );
          }
        }

        // Load aspect call tables
        {
          type->aspect_count = BardLoader_load_count( vm, reader );
          if (type->aspect_count > 0)
          {
            int a;

            type->aspect_types = (BardType**) BARD_ALLOCATE( sizeof(BardType*) * type->aspect_count );
            type->aspect_call_tables = (BardMethod***) BARD_ALLOCATE( sizeof(BardMethod**) * type->aspect_count );

            for (a=0; a<type->aspect_count; ++a)
            {
              int m_count, i;
              type->aspect_types[a] = BardLoader_load_indexed_type( vm, reader );
              m_count = BardLoader_load_count( vm, reader );
              type->aspect_call_tables[a] = (BardMethod**) BARD_ALLOCATE( sizeof(BardMethod*) * m_count );

              for (i=0; i<m_count; ++i)
              {
                type->aspect_call_tables[a][i] = BardLoader_load_indexed_method( vm, reader );
              }
            }
          }
        }
      }
    }

    // Load Real literals
    {
      int i;
      vm->literal_real_count = BardLoader_load_count( vm, reader );

      if (vm->literal_real_count)
      {
        vm->literal_reals = (BardReal*)BARD_ALLOCATE( sizeof(BardReal) * vm->literal_real_count);

        for(i = 0; i < vm->literal_real_count; ++i)
        {
          BardInteger upper32_of_real = (BardInteger)BardReader_read_integer( reader );
          BardInteger lower32_of_real = (BardInteger)BardReader_read_integer( reader );
          BardInt64 real_as_int64 = ((BardInt64)upper32_of_real << 32) | (lower32_of_real & 0xFFFFFFFF);

          BardReal* real = (BardReal*)&real_as_int64;
          vm->literal_reals[i] = *real;
        }
      }
    }

    // Load filenames used
    {
      int i;
      vm->filename_count = BardReader_read_integer( reader );
      vm->filenames = (char**) BARD_ALLOCATE( sizeof(char*) * vm->filename_count );
      for (i=0; i<vm->filename_count; ++i)
      {
        char* id = BardReader_read_id( reader );
        vm->filenames[i] = id;
      }
    }

    // Load line info
    {
      vm->line_info_count = BardLoader_load_count( vm, reader );
      if (vm->line_info_count)
      {
        int i;
        int count = vm->line_info_count;
        vm->line_info = (unsigned char*) BARD_ALLOCATE( vm->line_info_count );
        for (i=0; i<count; ++i)
        {
          vm->line_info[i] = BardReader_read_byte( reader );
        }
      }
    }


    BardVM_organize( vm );

    // Load string literals
    {
      int i;
      vm->literal_string_count = BardLoader_load_count( vm, reader );

      if (vm->literal_string_count)
      {
        vm->literal_strings = (BardObject**) BARD_ALLOCATE( sizeof(BardObject*) * vm->literal_string_count );

        for (i=0; i<vm->literal_string_count; ++i)
        {
          int string_count = BardLoader_load_count( vm, reader );
          BardString* obj = BardString_create_with_size( vm, string_count );
          obj->count = string_count;
          BardCharacter* dest = obj->characters - 1;
          while (--string_count >= 0)
          {
            *(++dest) = BardReader_read_integer( reader );
          }
          vm->literal_strings[i] = (BardObject*) BardString_set_hash_code( obj );
        }
      }
    }

    BardVM_create_singletons( vm );

    return 1;

  BARD_CATCH_ERROR
    printf( "ERROR: %s\n", vm->error_message );
    return 0;

  BARD_END_TRY(vm)
}

int BardLoader_load_count( BardVM* vm, BardReader* reader )
{
  int result = BardReader_read_integer( reader );
  if (result < 0) BardVM_throw_error( vm, 1, "Negative element count." );
  return result;
}

char* BardLoader_load_indexed_id( BardVM* vm, BardReader* reader )
{
  int id_index = BardReader_read_integer( reader );
  if (id_index < 0 || id_index >= vm->identifier_count)
  {
    BardVM_throw_error( vm, 1, "ID index out of bounds." );
  }

  return (char*) vm->identifiers[ id_index ];
}

BardType* BardLoader_load_indexed_type( BardVM* vm, BardReader* reader )
{
  int type_index = BardReader_read_integer( reader );
  if (type_index == -1) return NULL;

  if (type_index < 0 || type_index >= vm->type_count)
  {
    BardVM_throw_error( vm, 1, "Type index out of bounds." );
  }

  return (BardType*) vm->types[ type_index ];
}

BardMethod* BardLoader_load_indexed_method( struct BardVM* vm, BardReader* reader )
{
  int method_index = BardReader_read_integer( reader );
  if (method_index == -1) return NULL;

  if (method_index < 0 || method_index >= vm->method_count)
  {
    BardVM_throw_error( vm, 1, "Method index out of bounds." );
  }

  return (BardMethod*) vm->methods[ method_index ];
}

