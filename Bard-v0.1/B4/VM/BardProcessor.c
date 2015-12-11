#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <strings.h>

#include "Bard.h"
#include "BardOpcodes.h"

// Opcode is lower 10 bits (0..1023)
#define BARD_OPCODE(op)   ((op)       & 0x000003FF)

// Single argument N is upper 22 bits (signed)
#define BARD_OP_ARG_N(op) (op >> 10)

// Argument pair X (11 bits, 10:20, 0..2047) and Y (11 bits, 21:31, 0..2047) (unsigned)
#define BARD_OP_ARG_A(op) ((op >> 10)  & 0x000007FF)
#define BARD_OP_ARG_B(op) ((op >> 21) & 0x000007FF)

// Argument pair
//   A (11 bits, 10:20, 0..2047)
//   B (11 bits, 21:31, 0..2047)
#define BARD_OP_ARG_A(op) ((op >> 10)  & 0x000007FF)
#define BARD_OP_ARG_B(op) ((op >> 21) & 0x000007FF)

// Argument triple
//   X (7 bits, 10:16,    0..127)
//   Y (7 bits, 17:23,    0..127)
//   Z (8 bits, 24:31, -128..127)
#define BARD_OP_ARG_X(op) ((op >> 10) & 0x0000007F)
#define BARD_OP_ARG_Y(op) ((op >> 17) & 0x0000007F)
#define BARD_OP_ARG_Z(op) (op >> 24)

void BardProcessor_on_unhandled_native_method( BardVM* vm )
{
  BardMethod* m = vm->current_frame->m;

  printf( "Missing native implementation of %s::%s", m->type_context->name, BardMethod_call_signature(m) );
  if (m->return_type) printf( "->%s", m->return_type->name );
  printf("\n");

  exit(1);  // TODO: throw VM error instead
}

void BardProcessor_invoke( BardVM* vm, BardType* type, BardMethod* m )
{
//printf("Initial sp is %d\n", (int)(vm->sp - vm->stack));
  {
    BardStackValue* new_fp = vm->sp + m->parameter_slot_count;
    // fp of new method and sp to restore to after execute() finishes.

    // Special halt frame restores original frame and returns control to us.
    (--vm->current_frame)->m = NULL;
    vm->current_frame->fp    = new_fp;
    vm->current_frame->ip    = vm->code - 1; // code[0] is HALT instruction

    // Set up frame for new method execution.
    (--vm->current_frame)->m = m;
    vm->current_frame->fp    = new_fp;
    vm->current_frame->ip    = m->ip;

    vm->sp -= m->local_slot_count;

    BardProcessor_execute( vm );

    vm->sp += m->standard_return_slot_delta;
  }
//printf("Final sp is %d\n", (int)(vm->sp - vm->stack));
}

void BardProcessor_execute( BardVM* vm )
{
  BardInteger     op;
  BardStackFrame* current_frame = vm->current_frame;
  BardInteger*    ip = current_frame->ip;
  BardStackValue* fp = current_frame->fp;
  BardStackValue* sp = vm->sp;

  int trace = 0;
  for (;;)
  {
if (trace) printf(">>>>>>>>>>> @%d:  %d:%s (%d), sp:%d fp:%d\n",
    (int)((ip+1)-vm->code),
    BARD_OPCODE(ip[1]),
    bard_op_names[BARD_OPCODE(ip[1])],
    BARD_OP_ARG_N(ip[1]), (int)(sp-vm->stack),(int)(fp-vm->stack) );

/*
if(trace)
{
char* filename;
int   line;
if (BardVM_find_line_info_at_ip(vm, current_frame->m, (ip-vm->code)+1, &filename, &line))
{
printf("  @ip %d %s line %d\n", (int)(long)((ip+1)-vm->code), filename, line );
}
}
*/

    switch (BARD_OPCODE(op = *(++ip)))
    {
      case BARD_OP_HALT:
        vm->sp = fp;
        vm->current_frame = ++current_frame;
        return;

      case BARD_OP_NOP:
        continue;

      case BARD_OP_NATIVE_RETURN:
        // The stack is already correct - don't touch it as we return
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_RETURN_NIL:
        sp = fp + 1;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_RETURN_THIS:
        // 'this' object is already in the correct return slot - just pop off a stack frame.
        sp = fp;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_RETURN_OBJECT:
        fp->object = sp->object;
        sp = fp;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_RETURN_REAL:
        fp->real = sp->real;
        sp = fp;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_RETURN_INTEGER:
        fp->integer = sp->integer;
        sp = fp;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_RETURN_COMPOUND_N_SLOTS:
        {
          int slots = BARD_OP_ARG_N(op);
          BardStackValue* new_sp = fp - (slots-1);
          memmove( new_sp, sp, slots * sizeof(BardStackValue) );
          sp = new_sp;
          fp = (++current_frame)->fp;
          ip = current_frame->ip;
        }
        continue;

      case BARD_OP_COMPOUND_RETURN_NIL_FP_ADJUST_N:
        sp = fp + BARD_OP_ARG_N(op);
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_COMPOUND_RETURN_OBJECT_FP_ADJUST_N:
        (fp += BARD_OP_ARG_N(op))->object = sp->object;
        sp = fp;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_COMPOUND_RETURN_REAL_FP_ADJUST_N:
        (fp += BARD_OP_ARG_N(op))->real = sp->real;
        sp = fp;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_COMPOUND_RETURN_INTEGER_FP_ADJUST_N:
        (fp += BARD_OP_ARG_N(op))->integer = sp->integer;
        sp = fp;
        fp = (++current_frame)->fp;
        ip = current_frame->ip;
        continue;

      case BARD_OP_COMPOUND_RETURN_COMPOUND_A_SLOTS_FP_ADJUST_B:
        {
          BardStackValue* src = sp;
          fp += BARD_OP_ARG_B(op);
          memmove( fp, src, BARD_OP_ARG_A(op) * sizeof(BardStackValue) );
          sp = fp;
          fp = (++current_frame)->fp;
          ip = current_frame->ip;
        }
        continue;

      case BARD_OP_THROW_MISSING_RETURN:
        {
          char* filename;
          int   line;
          if (BardVM_find_line_info_at_ip(vm, current_frame->m, (ip-vm->code)+1, &filename, &line))
          {
            printf( "Missing 'return' in method beginning on line %d of %s\n", line, filename );
          }
        }
        return;

      case BARD_OP_THROW_EXCEPTION:
        {
          current_frame->ip = ip;
          vm->current_frame = current_frame;
          vm->sp = sp;

          BardProcessor_throw_exception_on_stack( vm );

          sp = vm->sp;
          current_frame = vm->current_frame;
          fp = current_frame->fp;
          ip = current_frame->ip;
        }
        continue;

      case BARD_OP_JUMP_TO_OFFSET_N:
        ip += BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_JUMP_IF_FALSE_TO_OFFSET_N:
        if ( !(sp++)->integer ) ip += BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_JUMP_IF_TRUE_TO_OFFSET_N:
        if ((sp++)->integer) ip += BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_JUMP_IF_NULL_TO_OFFSET_N:
        if ( !(sp++)->object ) ip += BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_JUMP_IF_NOT_NULL_TO_OFFSET_N:
        if ((sp++)->object) ip += BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_IF_FALSE_PUSH_FALSE_AND_JUMP_TO_OFFSET_N:
        if ( !sp->integer ) ip += BARD_OP_ARG_N(op);
        else                ++sp;
        continue;

      case BARD_OP_IF_TRUE_PUSH_TRUE_AND_JUMP_TO_OFFSET_N:
        if (sp->integer) ip += BARD_OP_ARG_N(op);
        else                ++sp;
        continue;

      case BARD_OP_DUPLICATE_OBJECT:
        --sp;
        sp->object = sp[1].object;
        continue;

      case BARD_OP_POP_DISCARD:
        ++sp;
        continue;

      case BARD_OP_POP_DISCARD_COUNT_N:
        sp += BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_TRACE_ON:
        trace = 1;
        continue;

      case BARD_OP_TRACE_OFF:
        trace = 0;
        continue;

      case BARD_OP_INSTANCE_OF_TYPE_INDEX_N:
        {
          BardType* operand_type = sp->object->type;
          BardType* of_type = vm->types[ BARD_OP_ARG_N(op) ];
          if (operand_type == of_type || BardType_partial_instance_of(operand_type,of_type))
          {
            sp->integer = 1;
          }
          else
          {
            sp->integer = 0;
          }
        }
        continue;

      case BARD_OP_SPECIALIZE_AS_TYPE_INDEX_N:
        if (sp->object)
        {
          BardType* operand_type = sp->object->type;
          BardType* of_type = vm->types[ BARD_OP_ARG_N(op) ];
          if (operand_type != of_type && !BardType_partial_instance_of(operand_type,of_type))
          {
            sp->object = NULL;
          }
        }
        continue;

      case BARD_OP_CONVERT_INTEGER_TO_REAL:
        sp->real = (BardReal) sp->integer;
        continue;

      case BARD_OP_CONVERT_REAL_TO_INTEGER:
        sp->integer = (BardInteger) sp->real;
        continue;

      case BARD_OP_CONVERT_INTEGER_TO_CHARACTER:
        sp->integer &= 65535;
        continue;

      case BARD_OP_CONVERT_INTEGER_TO_BYTE:
        sp->integer &= 255;
        continue;

      case BARD_OP_PUSH_THIS:
        (--sp)->object = fp->object;
if (trace)
{
  printf( "OBJECT TYPE:\n  " );
  if (sp->object) printf( "%s\n", sp->object->type->name );
  else            printf( "(null)\n" );
}
        continue;

      case BARD_OP_PUSH_THIS_COMPOUND_N_SLOTS:
        {
          int slot_count = BARD_OP_ARG_N(op);
          memcpy( sp-=slot_count, fp, slot_count*sizeof(BardStackValue) );
        }
        continue;

      case BARD_OP_PUSH_THIS_REAL:
        (--sp)->real = fp->real;
        continue;

      case BARD_OP_PUSH_THIS_INTEGER:
        (--sp)->integer = fp->integer;
        continue;

      case BARD_OP_PUSH_LITERAL_OBJECT_NULL:
        (--sp)->object = NULL;
        continue;

      case BARD_OP_PUSH_LITERAL_STRING_N:
        (--sp)->object = (BardObject*) vm->literal_strings[ BARD_OP_ARG_N(op) ];
        continue;

      case BARD_OP_PUSH_LITERAL_REAL:
        (--sp)->real = (BardReal)vm->literal_reals[ *(++ip) ];
        continue;

      case BARD_OP_PUSH_LITERAL_INTEGER:
        (--sp)->integer = *(++ip);
        continue;

      case BARD_OP_PUSH_LITERAL_INTEGER_N:
        (--sp)->integer = BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_PUSH_LITERAL_INTEGER_N_AS_REAL:
        (--sp)->real = (BardReal) BARD_OP_ARG_N(op);
        continue;

      case BARD_OP_PUSH_AND_CLEAR_N_SLOTS:
        {
          int count = BARD_OP_ARG_N(op);
          sp -= count;
          bzero( sp, count * sizeof(BardStackValue) );
        }
        continue;

        /*
      case BARD_OP_ADD_LITERAL_LOGICALS_TO_LIST_COUNT_N:
        {
          int count = BARD_OP_ARG_N( op );

          BardArrayList* list = (BardArrayList*) sp->object;
          int required_capacity = list->count + count;
          unsigned char* dest;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          dest = (list->data->byte_data + list->count);
          list->count += count;

          int odd_count = (count & 7);
          count &= ~7;
          while ((count-=8) >= 0)
          {
            int packed = *(++ip);
            dest[0] = (unsigned char) ((packed >> 7) & 1);
            dest[1] = (unsigned char) ((packed >> 6) & 1);
            dest[2] = (unsigned char) ((packed >> 5) & 1);
            dest[3] = (unsigned char) ((packed >> 4) & 1);
            dest[4] = (unsigned char) ((packed >> 3) & 1);
            dest[5] = (unsigned char) ((packed >> 2) & 1);
            dest[6] = (unsigned char) ((packed >> 1) & 1);
            dest[7] = (unsigned char) ((packed) & 1);
            dest += 8;
          }

          if (odd_count)
          {
            int packed = *(++ip);
            int cur_shift = 7;
            while (--odd_count >= 0)
            {
              *(dest++) = (unsigned char) ((packed >> cur_shift) & 1);
              --cur_shift;
            }
          }
        }
        continue;

      case BARD_OP_ADD_STACK_LOGICALS_TO_LIST_COUNT_N:
        {
          BardArrayList* list;
          int count = BARD_OP_ARG_N( op );
          int required_capacity;
          unsigned char*    dest;
          BardStackValue* cur_sp;

          sp += count;
          list = (BardArrayList*) sp->object;
          required_capacity = list->count + count;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          cur_sp = sp;

          dest = (list->data->byte_data + list->count) - 1;
          list->count += count;

          while (--count >= 0)
          {
            *(++dest) = (--cur_sp)->integer;
          }
        }
        continue;

      case BARD_OP_ADD_LITERAL_CHARACTERS_TO_LIST_COUNT_N:
        {
          int count = BARD_OP_ARG_N( op );

          BardArrayList* list = (BardArrayList*) sp->object;
          int required_capacity = list->count + count;
          BardCharacter* dest;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          dest = (list->data->character_data + list->count);
          list->count += count;

          int odd_count = (count & 1);
          count &= ~1;

          while ((count-=2) >= 0)
          {
            int packed = *(++ip);
            dest[0] = (BardCharacter) (packed >> 16);
            dest[1] = (BardCharacter) packed;
            dest += 2;
          }

          if (odd_count)
          {
            *dest = (BardCharacter) ((*(++ip)) >> 16);
          }
        }
        continue;

      case BARD_OP_ADD_STACK_CHARACTERS_TO_LIST_COUNT_N:
        {
          BardArrayList* list;
          int count = BARD_OP_ARG_N( op );
          int required_capacity;
          BardCharacter*    dest;
          BardStackValue* cur_sp;

          sp += count;
          list = (BardArrayList*) sp->object;
          required_capacity = list->count + count;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          cur_sp = sp;

          dest = (list->data->character_data + list->count) - 1;
          list->count += count;

          while (--count >= 0)
          {
            *(++dest) = (--cur_sp)->integer;
          }
        }
        continue;

      case BARD_OP_ADD_LITERAL_INTEGERS_TO_LIST_COUNT_N:
        {
          BardInteger count = BARD_OP_ARG_N( op );

          BardArrayList* list = (BardArrayList*) sp->object;
          int required_capacity = list->count + count;
          BardInteger* dest;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          dest = (list->data->integer_data + list->count) - 1;
          list->count += count;

          while (--count >= 0)
          {
            *(++dest) = *(++ip);
          }
        }
        continue;

      case BARD_OP_ADD_STACK_INTEGERS_TO_LIST_COUNT_N:
        {
          BardArrayList* list;
          int count = BARD_OP_ARG_N( op );
          int required_capacity;
          BardInteger*    dest;
          BardStackValue* cur_sp;

          sp += count;
          list = (BardArrayList*) sp->object;
          required_capacity = list->count + count;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          cur_sp = sp;

          dest = (list->data->integer_data + list->count) - 1;
          list->count += count;

          while (--count >= 0)
          {
            *(++dest) = (--cur_sp)->integer;
          }
        }
        continue;

      case BARD_OP_ADD_LITERAL_REALS_TO_LIST_COUNT_N:
        {
          BardInteger count = BARD_OP_ARG_N( op );

          BardArrayList* list = (BardArrayList*) sp->object;
          int required_capacity = list->count + count;
          BardReal* dest;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          dest = (list->data->real_data + list->count) - 1;
          list->count += count;

          while (--count >= 0)
          {
            *(++dest) = (BardReal) vm->literal_reals[ *(++ip) ];
          }
        }
        continue;

      case BARD_OP_ADD_STACK_REALS_TO_LIST_COUNT_N:
        {
          BardArrayList* list;
          int count = BARD_OP_ARG_N( op );
          int required_capacity;
          BardReal*    dest;
          BardStackValue* cur_sp;

          sp += count;
          list = (BardArrayList*) sp->object;
          required_capacity = list->count + count;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          cur_sp = sp;

          dest = (list->data->real_data + list->count) - 1;
          list->count += count;

          while (--count >= 0)
          {
            *(++dest) = (--cur_sp)->real;
          }
        }
        continue;

      case BARD_OP_ADD_STACK_OBJECTS_TO_LIST_COUNT_N:
        {
          BardArrayList* list;
          int count = BARD_OP_ARG_N( op );
          int required_capacity;
          BardObject**    dest;
          BardStackValue* cur_sp;

          sp += count;
          list = (BardArrayList*) sp->object;
          required_capacity = list->count + count;

          if (list->data->count < required_capacity)
          {
            list->data = BardArray_resized( list->data, required_capacity, list->count );
          }

          cur_sp = sp;

          dest = (list->data->object_data + list->count) - 1;
          list->count += count;

          while (--count >= 0)
          {
            *(++dest) = (--cur_sp)->object;
          }
        }
        continue;
        */

      case BARD_OP_READ_LOCAL_OBJECT_N:
        (--sp)->object = fp[ BARD_OP_ARG_N(op) ].object;
if (trace && sp->object)
{
printf( "  TYPE: %s\n", sp->object->type->name );
}
        continue;

      case BARD_OP_READ_LOCAL_REAL_N:
        (--sp)->real = fp[ BARD_OP_ARG_N(op) ].real;
        continue;

      case BARD_OP_READ_LOCAL_INTEGER_N:
        (--sp)->integer = fp[ BARD_OP_ARG_N(op) ].integer;
        continue;

      case BARD_OP_READ_LOCAL_COMPOUND_SLOT_A_SIZE_B:
        {
          int slot_count = BARD_OP_ARG_B(op);
          memcpy( sp -= slot_count, fp - BARD_OP_ARG_A(op), slot_count * sizeof(BardStackValue) );
        }
        continue;

      case BARD_OP_WRITE_LOCAL_OBJECT_N:
        fp[ BARD_OP_ARG_N(op) ].object = (sp++)->object;
        continue;

      case BARD_OP_WRITE_LOCAL_REAL_N:
        fp[ BARD_OP_ARG_N(op) ].real = (sp++)->real;
        continue;

      case BARD_OP_WRITE_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer = (sp++)->integer;
        continue;

      case BARD_OP_WRITE_LOCAL_COMPOUND_SLOT_A_SIZE_B:
        {
          int slot_count = BARD_OP_ARG_B(op);
          memcpy( fp - BARD_OP_ARG_A(op), sp, slot_count * sizeof(BardStackValue) );
          sp += slot_count;
        }
        continue;

      case BARD_OP_CLEAR_LOCAL_OBJECT_N:
        fp[ BARD_OP_ARG_N(op) ].object = NULL;
        continue;

      case BARD_OP_CLEAR_LOCAL_REAL_N:
        fp[ BARD_OP_ARG_N(op) ].real = 0.0;
        continue;

      case BARD_OP_CLEAR_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer = 0;
        continue;

      case BARD_OP_CLEAR_LOCAL_COMPOUND_SLOT_A_SIZE_B:
        bzero( fp - BARD_OP_ARG_A(op), BARD_OP_ARG_B(op) * sizeof(BardStackValue) );
        continue;

      case BARD_OP_ADD_ASSIGN_LOCAL_REAL_N:
        fp[ BARD_OP_ARG_N(op) ].real += (sp++)->real;
        continue;

      case BARD_OP_ADD_ASSIGN_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer += (sp++)->integer;
        continue;

      case BARD_OP_SUBTRACT_ASSIGN_LOCAL_REAL_N:
        fp[ BARD_OP_ARG_N(op) ].real -= (sp++)->real;
        continue;

      case BARD_OP_SUBTRACT_ASSIGN_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer -= (sp++)->integer;
        continue;

      case BARD_OP_MULTIPLY_ASSIGN_LOCAL_REAL_N:
        fp[ BARD_OP_ARG_N(op) ].real *= (sp++)->real;
        continue;

      case BARD_OP_MULTIPLY_ASSIGN_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer *= (sp++)->integer;
        continue;

      case BARD_OP_DIVIDE_ASSIGN_LOCAL_REAL_N:
        fp[ BARD_OP_ARG_N(op) ].real /= (sp++)->real;
        continue;

      case BARD_OP_DIVIDE_ASSIGN_LOCAL_INTEGER_N:
        {
          int slot = BARD_OP_ARG_N( op );
          fp[slot].integer = (BardInteger) (((BardReal)fp[slot].integer) / (BardReal) (sp++)->integer);
        }
        continue;

      case BARD_OP_MOD_ASSIGN_LOCAL_REAL_N:
        {
          double a = fp[ BARD_OP_ARG_N(op) ].real;
          double b = (sp++)->real;
          double q = a / b;
          fp[ BARD_OP_ARG_N(op) ].real = a - floor(q)*b;
        }
        continue;

      case BARD_OP_MOD_ASSIGN_LOCAL_INTEGER_N:
        {
          int a = fp[ BARD_OP_ARG_N(op) ].integer;
          int b = (++sp)->integer;
          if (a != 0)
          {
            if (b == 1)
            {
              a = 0;
            }
            else if ((a ^ b) < 0)
            {
              int r = a % b;
              a = (r != 0) ? (r+b) : r;
            }
            else
            {
              a %= b;
            }
            fp[ BARD_OP_ARG_N(op) ].integer = a;
          }
        }
        continue;

      case BARD_OP_POWER_ASSIGN_LOCAL_REAL_N:
        {
          double x = fp[ BARD_OP_ARG_N(op) ].real;
          fp[ BARD_OP_ARG_N(op) ].real = (BardReal) pow( x, (sp++)->real );
        }
        continue;

      case BARD_OP_POWER_ASSIGN_LOCAL_INTEGER_N:
        {
          double x = fp[ BARD_OP_ARG_N(op) ].integer;
          fp[ BARD_OP_ARG_N(op) ].integer = (BardInteger) pow( x, (double) (sp++)->integer );
        }
        continue;

      case BARD_OP_BITWISE_AND_ASSIGN_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer &= (sp++)->integer;
        continue;

      case BARD_OP_BITWISE_OR_ASSIGN_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer |= (sp++)->integer;
        continue;

      case BARD_OP_BITWISE_XOR_ASSIGN_LOCAL_INTEGER_N:
        fp[ BARD_OP_ARG_N(op) ].integer ^= (sp++)->integer;
        continue;

      case BARD_OP_ADD_ASSIGN_PROPERTY_REAL_N:
        ++sp;
        *((BardReal*)(((char*)(sp->object)) + BARD_OP_ARG_N(op))) += sp[-1].real;
        continue;

      case BARD_OP_ADD_ASSIGN_PROPERTY_INTEGER_N:
        ++sp;
        *((BardInteger*)(((char*)(sp->object)) + BARD_OP_ARG_N(op))) += sp[-1].integer;
        continue;

      case BARD_OP_SUBTRACT_ASSIGN_PROPERTY_REAL_N:
        ++sp;
        *((BardReal*)(((char*)(sp->object)) + BARD_OP_ARG_N(op))) -= sp[-1].real;
        continue;

      case BARD_OP_SUBTRACT_ASSIGN_PROPERTY_INTEGER_N:
        ++sp;
        *((BardInteger*)(((char*)(sp->object)) + BARD_OP_ARG_N(op))) -= sp[-1].integer;
        continue;

      case BARD_OP_ADD_ASSIGN_THIS_PROPERTY_REAL_N:
        *((BardReal*)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) += (sp++)->real;
        continue;

      case BARD_OP_ADD_ASSIGN_THIS_PROPERTY_INTEGER_N:
        *((BardInteger*)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) += (sp++)->integer;
        continue;

      case BARD_OP_SUBTRACT_ASSIGN_THIS_PROPERTY_REAL_N:
        *((BardReal*)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) -= (sp++)->real;
        continue;

      case BARD_OP_SUBTRACT_ASSIGN_THIS_PROPERTY_INTEGER_N:
        *((BardInteger*)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) -= (sp++)->integer;
        continue;

      case BARD_OP_INCREMENT_LOCAL_REAL_N:
        ++fp[ BARD_OP_ARG_N(op) ].real;
        continue;

      case BARD_OP_INCREMENT_LOCAL_INTEGER_N:
        ++fp[ BARD_OP_ARG_N(op) ].integer;
        continue;

      case BARD_OP_DECREMENT_LOCAL_REAL_N:
        --fp[ BARD_OP_ARG_N(op) ].real;
        continue;

      case BARD_OP_DECREMENT_LOCAL_INTEGER_N:
        --fp[ BARD_OP_ARG_N(op) ].integer;
        continue;

      case BARD_OP_INCREMENT_PROPERTY_REAL_N:
        ++ *((BardReal*)(((char*)((sp++)->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_INCREMENT_PROPERTY_INTEGER_N:
        ++ *((BardInteger*)(((char*)((sp++)->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_INCREMENT_THIS_PROPERTY_REAL_N:
        ++ *((BardReal*)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_INCREMENT_THIS_PROPERTY_INTEGER_N:
        ++ *((BardInteger*)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_DECREMENT_PROPERTY_REAL_N:
        -- *((BardReal*)(((char*)((sp++)->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_DECREMENT_PROPERTY_INTEGER_N:
        -- *((BardInteger*)(((char*)((sp++)->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_DECREMENT_THIS_PROPERTY_REAL_N:
        -- *((BardReal*)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_DECREMENT_THIS_PROPERTY_INTEGER_N:
        -- *((BardInteger*)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_SETTING_OBJECT_OFFSET_N:
        (--sp)->object = *((BardObject**)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))); 
        continue;

      case BARD_OP_READ_SETTING_REAL_OFFSET_N:
        (--sp)->real = *((BardReal*)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))); 
        continue;

      case BARD_OP_READ_SETTING_INTEGER_OFFSET_N:
        (--sp)->integer = *((BardInteger*)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))); 
        continue;

      case BARD_OP_READ_SETTING_CHARACTER_OFFSET_N:
        (--sp)->integer = *((BardCharacter*)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))); 
        continue;

      case BARD_OP_READ_SETTING_BYTE_OFFSET_N:
        (--sp)->integer = *(((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))); 
        continue;

      case BARD_OP_READ_SETTING_COMPOUND_OFFSET_N:
        {
          int   slot_count = ip[2];
          unsigned char* src = ((vm->types[ ip[1] ]->settings_data) + BARD_OP_ARG_N(op)); 
          memcpy( sp -= slot_count, src, slot_count * sizeof(BardStackValue) );
          ip += 2;
        }
        continue;

      case BARD_OP_READ_PROPERTY_OBJECT_OFFSET_N:
        sp->object = *((BardObject**)(((char*)(sp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_PROPERTY_REAL_OFFSET_N:
        sp->real = *((BardReal*)(((char*)(sp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_PROPERTY_INTEGER_OFFSET_N:
        sp->integer = *((BardInteger*)(((char*)(sp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_PROPERTY_CHARACTER_OFFSET_N:
        sp->integer = *((BardCharacter*)(((char*)(sp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_PROPERTY_BYTE_OFFSET_N:
        sp->integer = *(((unsigned char*)(sp->object)) + BARD_OP_ARG_N(op));
        continue;

      case BARD_OP_READ_PROPERTY_COMPOUND_OFFSET_N:
        {
          int   slot_count = *(++ip);
          char* src = ((char*)(sp->object)) + BARD_OP_ARG_N(op);
          memcpy( sp -= (slot_count-1), src, slot_count * sizeof(BardStackValue) );
        }
        continue;

      case BARD_OP_READ_THIS_PROPERTY_OBJECT_OFFSET_N:
        (--sp)->object = *((BardObject**)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_THIS_PROPERTY_REAL_OFFSET_N:
        (--sp)->real = *((BardReal*)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_THIS_PROPERTY_INTEGER_OFFSET_N:
        (--sp)->integer = *((BardInteger*)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_THIS_PROPERTY_CHARACTER_OFFSET_N:
        (--sp)->integer = *((BardCharacter*)(((char*)(fp->object)) + BARD_OP_ARG_N(op)));
        continue;

      case BARD_OP_READ_THIS_PROPERTY_BYTE_OFFSET_N:
        (--sp)->integer = *(((unsigned char*)(fp->object)) + BARD_OP_ARG_N(op));
        continue;

      case BARD_OP_READ_THIS_PROPERTY_COMPOUND_OFFSET_N:
        {
          int   slot_count = *(++ip);
if (trace)printf("  Slot count %d\n", slot_count );
          memcpy( sp -= slot_count, ((char*)(fp->object)) + BARD_OP_ARG_N(op), slot_count * sizeof(BardStackValue) );
        }
        continue;

      case BARD_OP_WRITE_SETTING_OBJECT_OFFSET_N:
        *((BardObject**)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))) = (sp++)->object;
        continue;

      case BARD_OP_WRITE_SETTING_REAL_OFFSET_N:
        *((BardReal*)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))) = (sp++)->real;
        continue;

      case BARD_OP_WRITE_SETTING_INTEGER_OFFSET_N:
        *((BardInteger*)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))) = (sp++)->integer;
        continue;

      case BARD_OP_WRITE_SETTING_CHARACTER_OFFSET_N:
        *((BardCharacter*)((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op))) = (sp++)->integer;
        continue;

      case BARD_OP_WRITE_SETTING_BYTE_OFFSET_N:
        *((vm->types[ *(++ip) ]->settings_data) + BARD_OP_ARG_N(op)) = (unsigned char) (sp++)->integer;
        continue;

      case BARD_OP_WRITE_SETTING_COMPOUND_OFFSET_N:
        {
          int slot_count = ip[2];
          memcpy( ((vm->types[ ip[1] ]->settings_data) + BARD_OP_ARG_N(op)), sp, slot_count*sizeof(BardStackValue) );
          sp += slot_count;
          ip += 2;
        }
        continue;

      case BARD_OP_WRITE_PROPERTY_OBJECT_OFFSET_N:
        *((BardObject**)(((char*)(sp[1].object)) + BARD_OP_ARG_N(op))) = sp->object;
        sp += 2;
        continue;

      case BARD_OP_WRITE_PROPERTY_REAL_OFFSET_N:
        *((BardReal*)(((char*)(sp[1].object)) + BARD_OP_ARG_N(op))) = sp->real;
        sp += 2;
        continue;

      case BARD_OP_WRITE_PROPERTY_INTEGER_OFFSET_N:
        *((BardInteger*)(((char*)(sp[1].object)) + BARD_OP_ARG_N(op))) = sp->integer;
        sp += 2;
        continue;

      case BARD_OP_WRITE_PROPERTY_CHARACTER_OFFSET_N:
        *((BardCharacter*)(((char*)(sp[1].object)) + BARD_OP_ARG_N(op))) = sp->integer;
        sp += 2;
        continue;

      case BARD_OP_WRITE_PROPERTY_BYTE_OFFSET_N:
        *(((char*)(sp[1].object)) + BARD_OP_ARG_N(op)) = (char) sp->integer;
        sp += 2;
        continue;

      case BARD_OP_WRITE_PROPERTY_COMPOUND_OFFSET_N:
        {
          int slot_count = *(++ip);
          memcpy( ((char*)sp[slot_count].object) + BARD_OP_ARG_N(op), sp, slot_count * sizeof(BardStackValue) );
          sp += slot_count + 1;
        }
        continue;

      case BARD_OP_WRITE_THIS_PROPERTY_OBJECT_OFFSET_N:
        *((BardObject**)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) = (sp++)->object;
        continue;

      case BARD_OP_WRITE_THIS_PROPERTY_REAL_OFFSET_N:
        *((BardReal*)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) = (sp++)->real;
        continue;

      case BARD_OP_WRITE_THIS_PROPERTY_INTEGER_OFFSET_N:
        *((BardInteger*)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) = (sp++)->integer;
        continue;

      case BARD_OP_WRITE_THIS_PROPERTY_CHARACTER_OFFSET_N:
        *((BardCharacter*)(((char*)(fp->object)) + BARD_OP_ARG_N(op))) = (BardCharacter) ((sp++)->integer);
        continue;

      case BARD_OP_WRITE_THIS_PROPERTY_BYTE_OFFSET_N:
        *((((char*)(fp->object)) + BARD_OP_ARG_N(op))) = (char) ((sp++)->integer);
        continue;

      case BARD_OP_WRITE_THIS_PROPERTY_COMPOUND_OFFSET_N:
        {
          int slot_count = *(++ip);
if (trace)printf("  Slot count %d\n", slot_count );
          memcpy( ((char*)(fp->object)) + BARD_OP_ARG_N(op), sp, slot_count * sizeof(BardStackValue) );
          sp += slot_count;
        }
        continue;

      case BARD_OP_READ_COMPOUND_OBJECT_OFFSET_A_DELTA_SP_B:
        {
          BardObject* result = sp[BARD_OP_ARG_A(op)].object;
          sp += BARD_OP_ARG_B( op );
          sp->object = result;
        }
        continue;

      case BARD_OP_READ_COMPOUND_REAL_OFFSET_A_DELTA_SP_B:
        {
          BardReal result = sp[BARD_OP_ARG_A(op)].real;
          sp += BARD_OP_ARG_B( op );
          sp->real = result;
        }
        continue;

      case BARD_OP_READ_COMPOUND_INTEGER_OFFSET_A_DELTA_SP_B:
        {
          BardInteger result = sp[BARD_OP_ARG_A(op)].integer;
          sp += BARD_OP_ARG_B( op );
          sp->integer = result;
        }
        continue;

      case BARD_OP_READ_COMPOUND_SUBSET_OFFSET_A_DELTA_SP_B:
        {
          BardStackValue* src = &sp[ BARD_OP_ARG_A(op) ];
          sp += BARD_OP_ARG_B( op );
          memmove( sp, src, *(++ip) * sizeof(BardStackValue) );
        }
        continue;

      case BARD_OP_READ_THIS_COMPOUND_OBJECT_OFFSET_N:
        (--sp)->object = fp[ BARD_OP_ARG_N(op) ].object;
        continue;

      case BARD_OP_READ_THIS_COMPOUND_REAL_OFFSET_N:
        (--sp)->real = fp[ BARD_OP_ARG_N(op) ].real;
        continue;

      case BARD_OP_READ_THIS_COMPOUND_INTEGER_OFFSET_N:
        (--sp)->integer = fp[ BARD_OP_ARG_N(op) ].integer;
        continue;

      case BARD_OP_READ_THIS_COMPOUND_SUBSET_OFFSET_A_SLOT_COUNT_B:
        {
          int slot_count = BARD_OP_ARG_B( op );
          memcpy( sp -= slot_count, fp + BARD_OP_ARG_A(op), slot_count * sizeof(BardStackValue) );
        }
        continue;

      case BARD_OP_CREATE_OBJECT_OF_TYPE_INDEX_N:
//printf( "CREATING OBJECT OF TYPE %s\n", vm->types[BARD_OP_ARG_N(op)]->name );
        {
          BardType* of_type = vm->types[ BARD_OP_ARG_N(op) ];
          (--sp)->object = BardMM_create_object( &of_type->vm->mm, of_type, of_type->object_size );

          // Static call to m_init_defaults
          {
            BardMethod* m = of_type->m_init_defaults;
  if(trace)printf( "Static call to %s::%s\n", m->type_context->name, BardMethod_call_signature(m) );

            fp = sp;

            // Save local copy of ip back in call stack
            current_frame->ip = ip;

            // Reset ip to new method
            ip = m->ip;

            // Create new stack frame info
            (--current_frame)->fp = fp;
            current_frame->m  = m;
            current_frame->ip = ip;
          }
        }
        continue;

      case BARD_OP_READ_SINGLETON_OF_TYPE_INDEX_N:
        (--sp)->object = vm->singletons[ BARD_OP_ARG_N(op) ];
        continue;

      case BARD_OP_READ_SINGLETON_COMPOUND_OF_TYPE_INDEX_N:
        {
          int type_index = BARD_OP_ARG_N( op );
          BardType* type = vm->types[ type_index ];
          int slot_count = type->stack_slots;
          memcpy( sp -= slot_count, ((char*)vm->singletons[type_index]) + type->data_offset, slot_count * sizeof(BardStackValue) );
        }
        continue;

      case BARD_OP_NATIVE_CALL_INDEX_N:
        {
          BardMethod* m = vm->methods[ BARD_OP_ARG_N(op) ];

          current_frame->ip = ip;
          vm->current_frame = current_frame;
          vm->sp = sp;

          m->native_method( vm );

          sp = vm->sp;
          current_frame = vm->current_frame;
          fp = current_frame->fp;
          ip = current_frame->ip;
        }
        continue;

      case BARD_OP_DYNAMIC_CALL_WITH_N_PARAMETERS:
        {
          BardMethod* m;

          // Use sp + #params to determine new frame pointer and method info
          fp = sp + BARD_OP_ARG_N(op);  // sp + parameter count
          m = fp->object->type->methods[ *(++ip) ];
          sp -= m->local_slot_count;
if(trace)
{
  printf( "Dynamic call to %s::%s (method #%d)", m->type_context->name, BardMethod_call_signature(m), *ip );
  if (m->return_type) printf("->%s",m->return_type->name);
  printf("\n");
  printf( "Context type: %s\n", fp->object->type->name );
}

          // Save local copy of ip back in call stack
          current_frame->ip = ip;

          // Reset ip to new method
          ip = m->ip;

          // Create new stack frame info
          (--current_frame)->fp = fp;
          current_frame->m  = m;
          current_frame->ip = ip;
        }
        continue;

      case BARD_OP_ASPECT_CALL_WITH_N_PARAMETERS:
        {
          BardMethod*    m;
          BardType*      type;
          BardType*      desired_aspect_type;
          BardType**     aspect_type_ptr;
          BardMethod***  aspect_call_table_ptr;

          // Use sp + #params to determine new frame pointer and method info
          fp = sp + BARD_OP_ARG_N(op);  // sp + parameter count

          type = fp->object->type;
          desired_aspect_type = vm->types[ *(++ip) ];
          aspect_type_ptr = type->aspect_types;
          aspect_call_table_ptr = type->aspect_call_tables;
          while (*aspect_type_ptr != desired_aspect_type)
          {
            ++aspect_type_ptr;
            ++aspect_call_table_ptr;
          }

//if(trace)printf( "fp:%d\n", (int)(fp-vm->stack));
//if(trace)printf( "object:%d\n", (int)(fp->object) );
          m = (*aspect_call_table_ptr)[ *(++ip) ];
          sp -= m->local_slot_count;
if(trace)
{
  printf( "Aspect call to %s::%s (method #%d)", m->type_context->name, BardMethod_call_signature(m), *ip );
  if (m->return_type) printf("->%s",m->return_type->name);
  printf("\n");
}

          // Save local copy of ip back in call stack
          current_frame->ip = ip;

          // Reset ip to new method
          ip = m->ip;

          // Create new stack frame info
          (--current_frame)->fp = fp;
          current_frame->m  = m;
          current_frame->ip = ip;
        }
        continue;

      case BARD_OP_STATIC_CALL_TO_METHOD_INDEX_N:
        {
          BardMethod* m = vm->methods[ BARD_OP_ARG_N(op) ];
if(trace)printf( "Static call to %s::%s\n", m->type_context->name, BardMethod_call_signature(m) );

          // Use sp + #params to determine new frame pointer and method info
          fp = sp + m->parameter_slot_count;
          sp -= m->local_slot_count;

          // Save local copy of ip back in call stack
          current_frame->ip = ip;

          // Reset ip to new method
          ip = m->ip;

          // Create new stack frame info
          (--current_frame)->fp = fp;
          current_frame->m  = m;
          current_frame->ip = ip;
        }
        continue;

      case BARD_OP_EQ_REAL:
        ++sp;
        sp->integer = (sp->real == sp[-1].real);
        continue;

      case BARD_OP_EQ_INTEGER:
        ++sp;
        sp->integer = (sp->integer == sp[-1].integer);
        continue;

      case BARD_OP_IS_OBJECT:
        ++sp;
        sp->integer = (sp->object == sp[-1].object);
        continue;

      case BARD_OP_NE_REAL:
        ++sp;
        sp->integer = (sp->real != sp[-1].real);
        continue;

      case BARD_OP_NE_INTEGER:
        ++sp;
        sp->integer = (sp->integer != sp[-1].integer);
        continue;

      case BARD_OP_IS_NOT_OBJECT:
        ++sp;
        sp->integer = (sp->object != sp[-1].object);
        continue;

      case BARD_OP_LT_REAL:
        ++sp;
        sp->integer = (sp->real < sp[-1].real);
        continue;

      case BARD_OP_LT_INTEGER:
        ++sp;
        sp->integer = (sp->integer < sp[-1].integer);
        continue;

      case BARD_OP_LE_REAL:
        ++sp;
        sp->integer = (sp->real <= sp[-1].real);
        continue;

      case BARD_OP_LE_INTEGER:
        ++sp;
        sp->integer = (sp->integer <= sp[-1].integer);
        continue;

      case BARD_OP_GT_REAL:
        ++sp;
        sp->integer = (sp->real > sp[-1].real);
        continue;

      case BARD_OP_GT_INTEGER:
        ++sp;
        sp->integer = (sp->integer > sp[-1].integer);
        continue;

      case BARD_OP_GE_REAL:
        ++sp;
        sp->integer = (sp->real >= sp[-1].real);
        continue;

      case BARD_OP_GE_INTEGER:
        ++sp;
        sp->integer = (sp->integer >= sp[-1].integer);
        continue;

      case BARD_OP_ADD_REAL:
        ++sp;
        sp->real += sp[-1].real;
        continue;

      case BARD_OP_ADD_INTEGER:
        ++sp;
        sp->integer += sp[-1].integer;
        continue;

      case BARD_OP_SUBTRACT_REAL:
        ++sp;
        sp->real -= sp[-1].real;
        continue;

      case BARD_OP_SUBTRACT_INTEGER:
        ++sp;
        sp->integer -= sp[-1].integer;
        continue;

      case BARD_OP_MULTIPLY_REAL:
        ++sp;
        sp->real *= sp[-1].real;
        continue;

      case BARD_OP_MULTIPLY_INTEGER:
        ++sp;
        sp->integer *= sp[-1].integer;
        continue;

      case BARD_OP_DIVIDE_REAL:
        ++sp;
        sp->real /= sp[-1].real;
        continue;

      case BARD_OP_DIVIDE_INTEGER:
        {
          // Only used in '/=' ops with various integer sizes
          ++sp;
          sp->integer = (BardInteger)(((BardReal)sp->integer) / (BardReal)sp[-1].integer);
        }
        continue;

      case BARD_OP_MOD_REAL:
        {
          double a = (++sp)->real;
          double b = sp[-1].real;
          double q = a / b;
          sp->real = a - floor(q)*b;
        }
        continue;

      case BARD_OP_MOD_INTEGER:
        {
          int a = (++sp)->integer;
          int b = sp[-1].integer;
          if (a != 0 && b != 0)
          {
            if (b == 1)
            {
              sp->integer = 0;
            }
            else if ((a ^ b) < 0)
            {
              int r = a % b;
              sp->integer = (r != 0) ? (r+b) : r;
            }
            else
            {
              sp->integer = a % b;
            }
          }
        }
        continue;

      case BARD_OP_POWER_REAL:
        ++sp;
        sp->real = pow(sp->real, sp[-1].real);
        continue;

      case BARD_OP_POWER_INTEGER:
        ++sp;
        sp->integer = pow(sp->integer, sp[-1].integer);
        continue;

      case BARD_OP_NEGATE_REAL:
        sp->real = -sp->real;
        continue;

      case BARD_OP_NEGATE_INTEGER:
        sp->integer = -sp->integer;
        continue;

      case BARD_OP_NEGATE_LOGICAL:
        sp->integer = 1 - sp->integer;
        continue;

      case BARD_OP_LOGICALIZE_OBJECT:
        sp->integer = (sp->object != NULL);
        continue;

      case BARD_OP_LOGICALIZE_REAL:
        sp->integer = (sp->real != 0.0);
        continue;

      case BARD_OP_LOGICALIZE_INTEGER:
        sp->integer = (sp->integer != 0);
        continue;

      case BARD_OP_BITWISE_AND_INTEGER:
        ++sp;
        sp->integer &= sp[-1].integer;
        continue;

      case BARD_OP_BITWISE_OR_INTEGER:
        ++sp;
        sp->integer |= sp[-1].integer;
        continue;

      case BARD_OP_BITWISE_XOR_INTEGER:
        ++sp;
        sp->integer ^= sp[-1].integer;
        continue;

      case BARD_OP_BITWISE_NOT_INTEGER:
        sp->integer = ~(sp->integer);
        continue;

      case BARD_OP_CREATE_ARRAY_TYPE_N:
        {
          int array_type_index = BARD_OP_ARG_N( op );
          BardType* array_type = vm->types[ array_type_index ];
          int element_type_index = *(++ip);
          BardType* element_type = vm->types[ element_type_index ];
          int count = sp->integer;
          sp->object = (BardObject*) BardArray_create_of_type( vm, array_type, element_type, count >= 0 ? count : 0 );
        }
        continue;

      case BARD_OP_ARRAY_COUNT:
        sp->integer = ((BardArray*) sp->object)->count;
        continue;

      case BARD_OP_READ_ARRAY_ELEMENT_OBJECT:
        {
          int index = (sp++)->integer;
          sp->object = ((BardArray*)sp->object)->object_data[ index ];
        }
        continue;

      case BARD_OP_READ_ARRAY_ELEMENT_REAL:
        {
          int index = (sp++)->integer;
          sp->real = ((BardArray*)sp->object)->real_data[ index ];
        }
        continue;

      case BARD_OP_READ_ARRAY_ELEMENT_INTEGER:
        {
          int index = (sp++)->integer;
          sp->integer = ((BardArray*)sp->object)->integer_data[ index ];
        }
        continue;

      case BARD_OP_READ_ARRAY_ELEMENT_CHARACTER:
        {
          int index = (sp++)->integer;
          sp->integer = ((BardArray*)sp->object)->character_data[ index ];
        }
        continue;

      case BARD_OP_READ_ARRAY_ELEMENT_BYTE:
        {
          int index = (sp++)->integer;
          sp->integer = ((BardArray*)sp->object)->byte_data[ index ];
        }
        continue;

      case BARD_OP_READ_ARRAY_ELEMENT_COMPOUND_N_SLOTS:
        {
          int slot_count = BARD_OP_ARG_N( op );
          int index = sp->integer;
          BardStackValue* dest_sp = sp - (slot_count - 2);
          memcpy( dest_sp, (((BardArray*)sp[1].object)->slot_data) + index * slot_count, slot_count * sizeof(BardStackValue) );
          sp = dest_sp;
        }
        continue;

      case BARD_OP_WRITE_ARRAY_ELEMENT_OBJECT:
        {
          int index = sp[1].integer;
          ((BardArray*)sp[2].object)->object_data[ index ] = sp[0].object;
          sp += 2;
        }
        continue;

      case BARD_OP_WRITE_ARRAY_ELEMENT_REAL:
        {
          int index = sp[1].integer;
          ((BardArray*)sp[2].object)->real_data[ index ] = sp[0].real;
          sp += 2;
        }
        continue;

      case BARD_OP_WRITE_ARRAY_ELEMENT_INTEGER:
        {
          int index = sp[1].integer;
          ((BardArray*)sp[2].object)->integer_data[ index ] = sp[0].integer;
          sp += 2;
        }
        continue;

      case BARD_OP_WRITE_ARRAY_ELEMENT_CHARACTER:
        {
          int index = sp[1].integer;
          ((BardArray*)sp[2].object)->character_data[ index ] = (BardCharacter) sp[0].integer;
          sp += 2;
        }
        continue;

      case BARD_OP_WRITE_ARRAY_ELEMENT_BYTE:
        {
          int index = sp[1].integer;
          ((BardArray*)sp[2].object)->byte_data[ index ] = (BardByte) sp[0].integer;
          sp += 2;
        }
        continue;

      case BARD_OP_WRITE_ARRAY_ELEMENT_COMPOUND_N_SLOTS:
        {
          int slot_count = BARD_OP_ARG_N( op );
          int index = sp[ slot_count ].integer;
          memcpy( (((BardArray*)sp[slot_count+1].object)->slot_data) + index * slot_count, sp, slot_count * sizeof(BardStackValue) );
          sp += slot_count + 1;
        }
        continue;

      case BARD_OP_SHL_INTEGER:
        ++sp;
        sp->integer <<= sp[-1].integer;
        continue;

      case BARD_OP_SHR_INTEGER:
        {
          // unsigned int val = (unsigned int)sp->integer;
          // sp->integer = val >> sp[-1].integer;
          BardInt64 a = (++sp)->integer;
          BardInt64 b = sp[-1].integer;
          if(b == 0) continue;

          a = (a >> 1) & 0x7fffFFFF;
          if (--b > 0) a >>= b;
          sp->integer = a;
        }
        continue;

      case BARD_OP_SHRX_INTEGER:
        sp->integer >>= sp[-1].integer;
        continue;

      case BARD_OP_LOGICAL_NOT:
        sp->integer = !sp->integer;
        continue;

      case BARD_OP_LOGICAL_XOR:
        ++sp;
        sp->integer = (sp->integer + sp[-1].integer) & 1;
        continue;

      case BARD_OP_RESOLVE_POP_DISCARD_COMPOUND_TYPE_N:
        {
          BardType* type = vm->types[ BARD_OP_ARG_N(op) ];
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_POP_DISCARD_COUNT_N, type->stack_slots );
        }
        continue;

      case BARD_OP_RESOLVE_READ_PROPERTY_INDEX_N:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = sp->object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;
          if (BardType_is_reference(type))     opcode = BARD_OP_READ_PROPERTY_OBJECT_OFFSET_N;
          else if (type == vm->type_Real)      opcode = BARD_OP_READ_PROPERTY_REAL_OFFSET_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_READ_PROPERTY_INTEGER_OFFSET_N;
          else if (type == vm->type_Character) opcode = BARD_OP_READ_PROPERTY_CHARACTER_OFFSET_N;
          else if (type == vm->type_Byte)      opcode = BARD_OP_READ_PROPERTY_BYTE_OFFSET_N;
          else if (type == vm->type_Logical)   opcode = BARD_OP_READ_PROPERTY_BYTE_OFFSET_N;
          else if (BardType_is_compound(type)) opcode = BARD_OP_READ_PROPERTY_COMPOUND_OFFSET_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_READ_PROPERTY_INDEX_N\n", type->name );
            return;
          }

          if (BardType_is_compound(type)) ip[1] = type->stack_slots;
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_READ_SETTING_INDEX_N:
        {
          // Rewrite this opcode using the setting type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardType*     type_context = vm->types[ ip[1] ];
          BardProperty* setting = type_context->settings + index;
          BardType*     type = setting->type;
          int           opcode = 0;
          if (BardType_is_reference(type))     opcode = BARD_OP_READ_SETTING_OBJECT_OFFSET_N;
          else if (type == vm->type_Real)      opcode = BARD_OP_READ_SETTING_REAL_OFFSET_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_READ_SETTING_INTEGER_OFFSET_N;
          else if (type == vm->type_Character) opcode = BARD_OP_READ_SETTING_CHARACTER_OFFSET_N;
          else if (type == vm->type_Byte)      opcode = BARD_OP_READ_SETTING_BYTE_OFFSET_N;
          else if (type == vm->type_Logical)   opcode = BARD_OP_READ_SETTING_BYTE_OFFSET_N;
          else if (BardType_is_compound(type)) opcode = BARD_OP_READ_SETTING_COMPOUND_OFFSET_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_READ_SETTING_INDEX_N\n", type->name );
            return;
          }

          if (BardType_is_compound(type)) ip[2] = type->stack_slots;
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, setting->offset );
        }
        continue;

      case BARD_OP_RESOLVE_READ_THIS_PROPERTY_INDEX_N:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = fp->object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;
          if (BardType_is_reference(type))     opcode = BARD_OP_READ_THIS_PROPERTY_OBJECT_OFFSET_N;
          else if (type == vm->type_Real)      opcode = BARD_OP_READ_THIS_PROPERTY_REAL_OFFSET_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_READ_THIS_PROPERTY_INTEGER_OFFSET_N;
          else if (type == vm->type_Character) opcode = BARD_OP_READ_THIS_PROPERTY_CHARACTER_OFFSET_N;
          else if (type == vm->type_Byte)      opcode = BARD_OP_READ_THIS_PROPERTY_BYTE_OFFSET_N;
          else if (type == vm->type_Logical)   opcode = BARD_OP_READ_THIS_PROPERTY_BYTE_OFFSET_N;
          else if (BardType_is_compound(type)) opcode = BARD_OP_READ_THIS_PROPERTY_COMPOUND_OFFSET_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_READ_THIS_PROPERTY_INDEX_N\n", type->name );
            return;
          }

          if (BardType_is_compound(type)) ip[1] = type->stack_slots;

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_WRITE_SETTING_INDEX_N:
        {
          // Rewrite this opcode using the setting type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardType*     type_context = vm->types[ip[1]];
          BardProperty* setting = type_context->settings + index;
          BardType*     type = setting->type;
          int           opcode = 0;

          if (BardType_is_reference(type))     opcode = BARD_OP_WRITE_SETTING_OBJECT_OFFSET_N;
          else if (type == vm->type_Real)      opcode = BARD_OP_WRITE_SETTING_REAL_OFFSET_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_WRITE_SETTING_INTEGER_OFFSET_N;
          else if (type == vm->type_Character) opcode = BARD_OP_WRITE_SETTING_CHARACTER_OFFSET_N;
          else if (type == vm->type_Byte)      opcode = BARD_OP_WRITE_SETTING_BYTE_OFFSET_N;
          else if (type == vm->type_Logical)   opcode = BARD_OP_WRITE_SETTING_BYTE_OFFSET_N;
          else
          {
            // NOTE: Since we have to skip past the value already on the stack, this opcode
            // will not work for compounds.
            printf( "Unhandled type %s in BARD_OP_RESOLVE_WRITE_SETTING_INDEX_N\n", type->name );
            return;
          }

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, setting->offset );
        }
        continue;

      case BARD_OP_RESOLVE_WRITE_SETTING_COMPOUND_INDEX_N:
        {
          // Rewrite this opcode using the setting type and byte offset, the latter
          // not known at compile time.
          BardType* type_context = vm->types[ ip[1] ];
          int index = BARD_OP_ARG_N( op );
          BardProperty* setting = type_context->settings + index;
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_WRITE_SETTING_COMPOUND_OFFSET_N, setting->offset );
        }
        continue;

      case BARD_OP_RESOLVE_WRITE_PROPERTY_INDEX_N:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = sp[1].object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;

          if (BardType_is_reference(type))     opcode = BARD_OP_WRITE_PROPERTY_OBJECT_OFFSET_N;
          else if (type == vm->type_Real)      opcode = BARD_OP_WRITE_PROPERTY_REAL_OFFSET_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_WRITE_PROPERTY_INTEGER_OFFSET_N;
          else if (type == vm->type_Character) opcode = BARD_OP_WRITE_PROPERTY_CHARACTER_OFFSET_N;
          else if (type == vm->type_Byte)      opcode = BARD_OP_WRITE_PROPERTY_BYTE_OFFSET_N;
          else if (type == vm->type_Logical)   opcode = BARD_OP_WRITE_PROPERTY_BYTE_OFFSET_N;
          else
          {
            // NOTE: Since we have to skip past the value already on the stack, this opcode
            // will not work for compounds.
            printf( "Unhandled type %s in BARD_OP_WRITE_PROPERTY_INDEX_N\n", type->name );
            return;
          }

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_WRITE_PROPERTY_COMPOUND_INDEX_N:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.  ip[1] is the number of slots in the compound
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = sp[ip[1]].object->type->properties + index;
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_WRITE_PROPERTY_COMPOUND_OFFSET_N, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_WRITE_THIS_PROPERTY_INDEX_N:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = fp->object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;
          if (BardType_is_reference(type))     opcode = BARD_OP_WRITE_THIS_PROPERTY_OBJECT_OFFSET_N;
          else if (type == vm->type_Real)      opcode = BARD_OP_WRITE_THIS_PROPERTY_REAL_OFFSET_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_WRITE_THIS_PROPERTY_INTEGER_OFFSET_N;
          else if (type == vm->type_Character) opcode = BARD_OP_WRITE_THIS_PROPERTY_CHARACTER_OFFSET_N;
          else if (type == vm->type_Byte)      opcode = BARD_OP_WRITE_THIS_PROPERTY_BYTE_OFFSET_N;
          else if (type == vm->type_Logical)   opcode = BARD_OP_WRITE_THIS_PROPERTY_BYTE_OFFSET_N;
          else if (BardType_is_compound(type)) opcode = BARD_OP_WRITE_THIS_PROPERTY_COMPOUND_OFFSET_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_WRITE_THIS_PROPERTY_INDEX_N\n", type->name );
            return;
          }

          if (BardType_is_compound(type)) ip[1] = type->stack_slots;

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_INCREMENT_PROPERTY:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = fp->object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;
          if      (type == vm->type_Real)      opcode = BARD_OP_INCREMENT_PROPERTY_REAL_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_INCREMENT_PROPERTY_INTEGER_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_INCREMENT_PROPERTY\n", type->name );
            return;
          }

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_INCREMENT_THIS_PROPERTY:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = fp->object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;
          if      (type == vm->type_Real)      opcode = BARD_OP_INCREMENT_THIS_PROPERTY_REAL_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_INCREMENT_THIS_PROPERTY_INTEGER_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_INCREMENT_THIS_PROPERTY\n", type->name );
            return;
          }

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_DECREMENT_PROPERTY:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = fp->object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;
          if      (type == vm->type_Real)      opcode = BARD_OP_DECREMENT_PROPERTY_REAL_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_DECREMENT_PROPERTY_INTEGER_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_DECREMENT_PROPERTY\n", type->name );
            return;
          }

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

      case BARD_OP_RESOLVE_DECREMENT_THIS_PROPERTY:
        {
          // Rewrite this opcode using the property type and byte offset, the latter
          // not known at compile time.
          int index = BARD_OP_ARG_N( op );
          BardProperty* property = fp->object->type->properties + index;
          BardType*     type = property->type;
          int           opcode = 0;
          if      (type == vm->type_Real)      opcode = BARD_OP_DECREMENT_THIS_PROPERTY_REAL_N;
          else if (type == vm->type_Integer)   opcode = BARD_OP_DECREMENT_THIS_PROPERTY_INTEGER_N;
          else
          {
            printf( "Unhandled type %s in BARD_OP_RESOLVE_DECREMENT_THIS_PROPERTY\n", type->name );
            return;
          }

          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( opcode, property->offset );
        }
        continue;

        case BARD_OP_RESOLVE_ADD_ASSIGN_PROPERTY_REAL_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_ADD_ASSIGN_PROPERTY_REAL_N, 
              sp[1].object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

        case BARD_OP_RESOLVE_ADD_ASSIGN_PROPERTY_INTEGER_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_ADD_ASSIGN_PROPERTY_INTEGER_N, 
              sp[1].object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

        case BARD_OP_RESOLVE_SUBTRACT_ASSIGN_PROPERTY_REAL_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_SUBTRACT_ASSIGN_PROPERTY_REAL_N, 
              sp[1].object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

        case BARD_OP_RESOLVE_SUBTRACT_ASSIGN_PROPERTY_INTEGER_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_SUBTRACT_ASSIGN_PROPERTY_INTEGER_N, 
              sp[1].object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

        case BARD_OP_RESOLVE_ADD_ASSIGN_THIS_PROPERTY_REAL_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_ADD_ASSIGN_THIS_PROPERTY_REAL_N, 
              fp->object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

        case BARD_OP_RESOLVE_ADD_ASSIGN_THIS_PROPERTY_INTEGER_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_ADD_ASSIGN_THIS_PROPERTY_INTEGER_N, 
              fp->object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

        case BARD_OP_RESOLVE_SUBTRACT_ASSIGN_THIS_PROPERTY_REAL_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_SUBTRACT_ASSIGN_THIS_PROPERTY_REAL_N, 
              fp->object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

        case BARD_OP_RESOLVE_SUBTRACT_ASSIGN_THIS_PROPERTY_INTEGER_INDEX_N:
          *(ip--) = BARD_CREATE_OP_WITH_ARG_N( BARD_OP_SUBTRACT_ASSIGN_THIS_PROPERTY_INTEGER_N, 
              fp->object->type->properties[BARD_OP_ARG_N(op)].offset );
          continue;

      default:
        printf( "Unhandled opcode: %d\n", BARD_OPCODE(op) );
        return;
    }
  }
}

void BardProcessor_throw_exception_on_stack( BardVM* vm )
{
  BardStackFrame* cur_frame = vm->current_frame;
  BardMethod* m = cur_frame->m;
  BardObject*     exception_object = vm->sp->object;
  BardType*       exception_type = exception_object->type;
  BardInteger*    ip = cur_frame->ip;

  while (m)
  {
    int count = m->exception_handler_count;
    if (count)
    {
      BardExceptionHandlerInfo* cur_handler = m->exception_handlers + count;
      BardExceptionHandlerCatchInfo* best_catch = NULL;
      while (--count >= 0)
      {
        --cur_handler;
//printf("%u  %u  %u\n", ((unsigned int)cur_handler->ip_start)%1000, ((unsigned int)ip)%1000, ((unsigned int)cur_handler->ip_limit)%1000 );
        if (ip >= cur_handler->ip_start && ip < cur_handler->ip_limit)
        {
          int catch_count = cur_handler->catch_count;
          BardExceptionHandlerCatchInfo* cur_catch = cur_handler->catches;
          while (--catch_count >= 0)
          {
            BardType* catch_type = cur_catch->catch_type;
            if (catch_type == exception_type || BardType_partial_instance_of(exception_type,catch_type))
            {
//printf("type %s is instanceOf %s\n",exception_type->name,catch_type->name);
//printf("Best\n");
              best_catch = cur_catch;
              break;
            }
            ++cur_catch;
          }
        }
        else
        {
          if (best_catch) break;
        }
      }

      if (best_catch)
      {
        cur_frame->ip = best_catch->handler_ip;
//printf(" -> ip %u\n", ((unsigned int)(cur_frame->ip-vm->code))%1000 );
        cur_frame->fp[ best_catch->local_slot_index ].object = exception_object;
        vm->current_frame = cur_frame;
        return;
      }

      vm->sp = cur_frame->fp;
    }
    m = (++cur_frame)->m;
    ip = cur_frame->ip;
  }
  vm->current_frame = cur_frame;

  printf( "Uncaught exception.\n" );
}

