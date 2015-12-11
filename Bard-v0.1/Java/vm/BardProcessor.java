package vm;

import java.lang.reflect.*;

public class BardProcessor
{
  public BardVM vm;

  public int[]        code;

  public int          ip;
  public int          fp;
  public int[]        fp_stack;
  public int[]        ip_stack;
  public BardMethod   this_method;
  public BardMethod   halt_method;
  public BardMethod[] method_stack;
  public int          call_sp;

  public int          sp;

  public double[]     Real_stack;
  public int[]        Integer_stack;
  public BardObject[] Object_stack;
  public Object[]     Native_stack;

  public int call_stack_size;
  public int stack_size;

  public StringBuilder buffer = new StringBuilder();

  public boolean fatal_error;
  public String  error_message;

  public boolean tron;

  public BardProcessor( BardVM vm, int call_stack_size )
  {
    this.vm = vm;
    this.call_stack_size = call_stack_size;

    ip_stack = new int[ call_stack_size ];
    fp_stack = new int[ call_stack_size ];
    method_stack = new BardMethod[ call_stack_size ];
    call_sp  = call_stack_size; 

    stack_size = call_stack_size * 4;
    Real_stack = new double[ stack_size ];
    Integer_stack = new int[ stack_size ];
    Object_stack = new BardObject[ stack_size ];
    Native_stack = new Object[ stack_size ];
    sp = stack_size;
    fp = sp;

    halt_method = new BardMethod( vm.type_Global, "halt" );
    halt_method.code = new int[]{ BardOp.halt };

    //result_printer = new BardStandardLibrary.println_1( vm );
  }

  public void push_Object( BardObject obj )
  {
    Object_stack[--sp] = obj;
  }

  public void push_String( String value )
  {
    Object_stack[--sp] = vm.create_String_object( value );
  }

  public void push_Real( double value )
  {
    Real_stack[--sp] = value;
  }

  public void push_Integer( int value )
  {
    Integer_stack[--sp] = value;
  }

  public void push_Character( int value )
  {
    Integer_stack[--sp] = value;
  }

  public void push_Logical( boolean value )
  {
    Integer_stack[--sp] = value ? 1 : 0;
  }

  public void push_this()
  {
    Object_stack[--sp] = Object_stack[fp];
  }

  public void pop_discard() { ++sp; }

  public BardObject pop_Object() { return Object_stack[sp++]; }
  public String     pop_String() { return Object_stack[sp++].toString(); }
  public double     pop_Real() { return Real_stack[sp++]; }
  public int        pop_Integer() { return Integer_stack[sp++]; }
  public int        pop_Character() { return Integer_stack[sp++]; }
  public boolean    pop_Logical() { return Integer_stack[sp++] != 0; }

  public BardObject peek_Object() { return Object_stack[sp]; }
  public String     peek_String() { return (String) Native_stack[sp]; }
  public double     peek_Real() { return Real_stack[sp]; }
  public int        peek_Integer() { return Integer_stack[sp]; }
  public int        peek_Character() { return Integer_stack[sp]; }
  public boolean    peek_Logical() { return Integer_stack[sp] != 0; }

  /*
  public String read_String_property( BardObject obj, String name )
  {
    // TODO
    if (obj != null)
    {
      BardProperty p_info = obj.type.find_property( name );
      if (p_info != null)
      {
        return obj.
      }
    }
    throw new RuntimeException( "No such property: " + name );
  }
    */


  public void invoke( String signature, int arg_count )
  {
    BardObject obj = Object_stack[ sp + arg_count ];
    BardMethod m = obj.type.find_method( signature );
    if (m == null) throw new BardError( "No such method " + obj.type.name + "::" + signature + "." );
    invoke( m );
  }

  public void invoke( BardMethod m )
  {
    if (m.native_method != null)
    {
      m.native_method.execute( this );
    }
    else
    {
      int new_fp = sp + m.parameter_count;

      // Save original stack frame
      method_stack[ --call_sp ] = this_method;
      fp_stack[ call_sp ] = fp;
      ip_stack[ call_sp ] = ip;

      // Special halt method
      method_stack[ --call_sp ] = halt_method;
      fp_stack[ call_sp ] = new_fp;
      ip_stack[ call_sp ] = -1;

      // Setup for new method execution
      fp = new_fp;
      sp -= m.local_count;
      this_method = m;
      code = m.code;
      ip = m.initial_ip;

      execute();
    }
  }

  public void pop_stack_frame()
  {
    sp = fp;
    fp = fp_stack[ call_sp ];
    ip = ip_stack[ call_sp ];
    this_method = method_stack[ call_sp++ ];
    if (this_method != null) code = this_method.code;
  }

  public void execute()
  {
    try
    {
    BardVM vm = this.vm;

    int[] code = this.code;
    int   ip   = this.ip;
    int   sp   = this.sp;
    int   fp   = this.fp;

    for (;;)
    {
//tron = true;
if (tron) System.out.println( ">> " + (ip+1) + ": " + describe_op( code[ip+1] ) );
//System.out.println( "  FP: " + fp );
      switch (code[++ip])
      {
        case BardOp.halt:
//System.out.println ("Setting sp to " + fp );
          this.sp = fp;
          this.fp = fp_stack[ call_sp ];
          this.ip = ip_stack[ call_sp ];
          this_method = method_stack[ call_sp++ ];
          if (this_method != null) this.code = this_method.code;
          return;

        case BardOp.jump:
          ip += code[ip+1] + 1;
          continue;

        case BardOp.jump_if_false:
          {
            int target_ip = code[++ip];
            if (Integer_stack[sp++] == 0) ip += target_ip;
          }
          continue;

        case BardOp.jump_if_true:
          {
            int target_ip = code[++ip];
            if (Integer_stack[sp++] != 0) ip += target_ip;
          }
          continue;

        case BardOp.push_false_and_jump_if_false:
          {
            int target_ip = code[++ip];
            if (Integer_stack[sp] == 0) ip += target_ip;
            else                        ++sp;
          }
          continue;

        case BardOp.push_true_and_jump_if_true:
          {
            int target_ip = code[++ip];
            if (Integer_stack[sp] != 0) ip += target_ip;
            else                        ++sp;
          }
          continue;

        case BardOp.jump_if_null:
          {
            int target_ip = code[++ip];
            if (Object_stack[sp] == null)
            {
              ip += target_ip;
              ++sp;
            }
          }
          continue;

        case BardOp.jump_if_not_null:
          {
            int target_ip = code[++ip];
            if (Object_stack[sp] != null) ip += target_ip;
            else                          ++sp;
          }
          continue;

        case BardOp.pop_discard:
          ++sp;
          continue;

        case BardOp.return_nil:
          sp = fp + 1;
          fp = fp_stack[ call_sp ];
          ip = ip_stack[ call_sp ];
          this_method = method_stack[ call_sp++ ];
          code = this_method.code;
          continue;

        case BardOp.return_Object:
          Object_stack[fp] = Object_stack[sp];
          sp = fp;
          fp = fp_stack[ call_sp ];
          ip = ip_stack[ call_sp ];
          this_method = method_stack[ call_sp++ ];
          code = this_method.code;
          continue;

        case BardOp.return_Real:
          Real_stack[fp] = Real_stack[sp];
          sp = fp;
          fp = fp_stack[ call_sp ];
          ip = ip_stack[ call_sp ];
          this_method = method_stack[ call_sp++ ];
          code = this_method.code;
          continue;

        case BardOp.return_Integer:
          Integer_stack[fp] = Integer_stack[sp];
          sp = fp;
          fp = fp_stack[ call_sp ];
          ip = ip_stack[ call_sp ];
          this_method = method_stack[ call_sp++ ];
          code = this_method.code;
          continue;

        case BardOp.return_Native:
          Native_stack[fp] = Native_stack[sp];
          sp = fp;
          fp = fp_stack[ call_sp ];
          ip = ip_stack[ call_sp ];
          this_method = method_stack[ call_sp++ ];
          code = this_method.code;
          continue;

        case BardOp.throw_Exception:
          fatal_error = true;
          error_message = Object_stack[sp].toString();
          System.out.println( error_message );
          this.sp = fp;
          this.fp = fp_stack[ call_sp ];
          this.ip = ip_stack[ call_sp ];
          this.code = code;
          while (call_sp != call_stack_size)
          {
            pop_stack_frame();
          }
          return;

        case BardOp.println_nil:
          System.out.println();
          continue;

        case BardOp.finite_loop:
          {
            int target_ip = code[++ip];
            if (--Integer_stack[sp] >= 0)
            {
              ip += target_ip;
              continue;
            }
            else
            {
              --sp;
              continue;
            }
          }

        case BardOp.unbox_Real:
          Real_stack[sp] = ((BardValue.RealValue)Object_stack[sp]).value;
          continue;

        case BardOp.unbox_Integer:
          Integer_stack[sp] = ((BardValue.IntegerValue)Object_stack[sp]).value;
          continue;

        case BardOp.unbox_Character:
          throw new RuntimeException( "TODO" );

        case BardOp.unbox_Logical:
          throw new RuntimeException( "TODO" );

        case BardOp.box_Real:
          throw new RuntimeException( "TODO" );
          //continue;

        case BardOp.box_Integer:
          throw new RuntimeException( "TODO" );
          //continue;

        case BardOp.box_Character:
          throw new RuntimeException( "TODO" );
          //continue;

        case BardOp.box_Logical:
          throw new RuntimeException( "TODO" );
          //continue;

        case BardOp.type_check:
          {
            BardType   of_type = vm.type_list.get( code[++ip] );
            BardObject obj = Object_stack[sp];
            //System.out.println( "Typecheck: " + obj + " instanceOf " + of_type + "?" );
            if (obj != null && !obj.type.instance_of(of_type)) Object_stack[sp] = null;
          }
          continue;

        case BardOp.Fixed_instanceOf:
          {
            BardType   of_type = vm.type_list.get( code[++ip] );
            BardObject obj = Object_stack[sp];
            Integer_stack[sp] = (obj != null && obj.type.instance_of(of_type)) ? 1 : 0;
          }
          continue;

        case BardOp.cast_Integer_to_Real:
          Real_stack[sp] = Integer_stack[sp];
//if (tron) System.out.println( "  stack at " + sp );
          continue;

        case BardOp.cast_Real_to_Integer:
          Integer_stack[sp] = (int)(long)Real_stack[sp];
          continue;

        case BardOp.cast_Character_to_Integer:
          {
            int ch = Integer_stack[sp];
            if (ch >= '0' && ch <= '9') Integer_stack[sp] = (ch - '0');
            else if (ch >= 'a' && ch <= 'z') Integer_stack[sp] = (ch - 'a') + 10;
            else if (ch >= 'A' && ch <= 'Z') Integer_stack[sp] = (ch - 'A') + 10;
            else                             Integer_stack[sp] = 0;
          }
          continue;

        case BardOp.push_Object_this:
          Object_stack[--sp] = Object_stack[fp];
          continue;

        case BardOp.push_literal_Object_null:
          Object_stack[--sp] = null;
          continue;

        case BardOp.push_literal_String:
          Object_stack[--sp] = this_method.String_literals[ code[++ip] ];
          continue;

        case BardOp.push_literal_Native_null:
          Native_stack[--sp] = null;
          continue;

        case BardOp.push_literal_Real:
          Real_stack[--sp] = this_method.Real_literals[ code[++ip] ];
          continue;

        case BardOp.push_literal_Real_negative_1:
          Real_stack[--sp] = -1.0;
          continue;

        case BardOp.push_literal_Real_0:
          Real_stack[--sp] = 0.0;
          continue;

        case BardOp.push_literal_Real_1:
          Real_stack[--sp] = 1.0;
          continue;

        case BardOp.push_literal_Integer:
          Integer_stack[--sp] = code[++ip];
          continue;

        case BardOp.push_literal_Integer_negative_1:
          Integer_stack[--sp] = -1;
          continue;

        case BardOp.push_literal_Integer_0:
          Integer_stack[--sp] = 0;
          continue;

        case BardOp.push_literal_Integer_1:
          Integer_stack[--sp] = 1;
          continue;

        case BardOp.negate_Real:
          Real_stack[sp] = -Real_stack[sp];
          continue;

        case BardOp.negate_Integer:
          Integer_stack[sp] = -Integer_stack[sp];
          continue;

        case BardOp.negate_Logical:
          Integer_stack[sp] = 1 - Integer_stack[sp];
          continue;

        case BardOp.logicalize_Object:
          Integer_stack[sp] = (Object_stack[sp] != null) ? 1 : 0;
          continue;

        case BardOp.logicalize_Real:
          Integer_stack[sp] = (Real_stack[sp] != 0) ? 1 : 0;
          continue;

        case BardOp.logicalize_Integer:
          if (Integer_stack[sp] != 0) Integer_stack[sp] = 1;
          continue;

        case BardOp.eq_Object:
          ++sp;
          Integer_stack[sp] = (Object_stack[sp] == Object_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.eq_Real:
          ++sp;
          Integer_stack[sp] = (Real_stack[sp] == Real_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.eq_Integer:
          ++sp;
          Integer_stack[sp] = (Integer_stack[sp] == Integer_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.ne_Object:
          ++sp;
          Integer_stack[sp] = (Object_stack[sp] != Object_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.ne_Real:
          ++sp;
          Integer_stack[sp] = (Real_stack[sp] != Real_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.ne_Integer:
          ++sp;
          Integer_stack[sp] = (Integer_stack[sp] != Integer_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.le_Real:
          ++sp;
          Integer_stack[sp] = (Real_stack[sp] <= Real_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.le_Integer:
          ++sp;
          Integer_stack[sp] = (Integer_stack[sp] <= Integer_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.ge_Real:
          ++sp;
          Integer_stack[sp] = (Real_stack[sp] >= Real_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.ge_Integer:
          ++sp;
          Integer_stack[sp] = (Integer_stack[sp] >= Integer_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.lt_Real:
          ++sp;
          Integer_stack[sp] = (Real_stack[sp] < Real_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.lt_Integer:
          ++sp;
          Integer_stack[sp] = (Integer_stack[sp] < Integer_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.gt_Real:
          ++sp;
          Integer_stack[sp] = (Real_stack[sp] > Real_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.gt_Integer:
          ++sp;
          Integer_stack[sp] = (Integer_stack[sp] > Integer_stack[sp-1]) ? 1 : 0;
          continue;

        case BardOp.is_Object:
          ++sp;
          Integer_stack[sp] = (Object_stack[sp] == Object_stack[sp-1]) ? 1 : 0;
if (tron) System.out.println( Integer_stack[sp] );
          continue;

        case BardOp.add_Real:
          ++sp;
          Real_stack[sp] += Real_stack[sp-1];
          continue;

        case BardOp.add_Integer:
          ++sp;
          Integer_stack[sp] += Integer_stack[sp-1];
          continue;

        case BardOp.subtract_Real:
          ++sp;
          Real_stack[sp] -= Real_stack[sp-1];
          continue;

        case BardOp.subtract_Integer:
          ++sp;
          Integer_stack[sp] -= Integer_stack[sp-1];
          continue;

          /*
        case BardOp.multiply_String_and_Integer:
          {
            int count = Integer_stack[ sp++ ];
            String st = (String) Native_stack[sp];
            buffer.setLength( 0 );
            for (int i=0; i<count; ++i)
            {
              buffer.append( st );
            }
            Native_stack[sp] = buffer.toString();
          }
          continue;
          */

        case BardOp.multiply_Real:
          ++sp;
          Real_stack[sp] *= Real_stack[sp-1];
          continue;

        case BardOp.multiply_Integer:
          ++sp;
          Integer_stack[sp] *= Integer_stack[sp-1];
          continue;

        case BardOp.divide_Real:
          ++sp;
          Real_stack[sp] /= Real_stack[sp-1];
          continue;

        case BardOp.mod_Real:
          {
            double b = Real_stack[sp++];
            double a = Real_stack[sp];
            double q = a / b;
            Real_stack[sp] = a - Math.floor(q) * b;
          }
          continue;

        case BardOp.mod_Integer:
          {
            int b = Integer_stack[sp++];
            int a = Integer_stack[sp];
            if (a != 0)
            {
              if (b == 1)
              {
                Integer_stack[sp] = 0;
              }
              else if ((a ^ b) < 0)
              {
                int r = a % b;
                Integer_stack[sp] = (r != 0) ? (r+b) : r;
              }
              else
              {
                Integer_stack[sp] = a % b;
              }
            }
          }
          continue;

        case BardOp.power_Real:
          {
            double b = Real_stack[sp++];
            Real_stack[sp] = Math.pow( Real_stack[sp], b );
          }
          continue;

        case BardOp.power_Integer:
          {
            double b = Integer_stack[sp++];
            Integer_stack[sp] = (int)(long)Math.pow( (double) Integer_stack[sp], b );
          }
          continue;

        case BardOp.bitwise_and_Integer:
          ++sp;
          Integer_stack[sp] &= Integer_stack[sp-1];
          continue;

        case BardOp.bitwise_or_Integer:
          ++sp;
          Integer_stack[sp] |= Integer_stack[sp-1];
          continue;

        case BardOp.bitwise_xor_Integer:
          ++sp;
          Integer_stack[sp] ^= Integer_stack[sp-1];
          continue;

        case BardOp.shl_Integer:
          ++sp;
          Integer_stack[sp] <<= Integer_stack[sp-1];
          continue;

        case BardOp.shr_Integer:
          ++sp;
          Integer_stack[sp] >>>= Integer_stack[sp-1];
          continue;

        case BardOp.shrx_Integer:
          ++sp;
          Integer_stack[sp] >>= Integer_stack[sp-1];
          continue;

        case BardOp.logical_not:
          Integer_stack[sp] = 1 - Integer_stack[sp];
          continue;

        case BardOp.read_local_Object:
          Object_stack[--sp] = Object_stack[ fp + code[++ip] ];
          continue;

        case BardOp.read_local_Real:
          Real_stack[--sp] = Real_stack[ fp + code[++ip] ];
//if (tron) System.out.println( "Reading " + Real_stack[sp] + ", on stack at " + sp );
          continue;

        case BardOp.read_local_Integer:
          Integer_stack[--sp] = Integer_stack[ fp + code[++ip] ];
//if (tron) System.out.println( "Reading " + Integer_stack[sp] + ", on stack at " + sp );
          continue;

        case BardOp.read_local_Native:
          Native_stack[--sp] = Native_stack[ fp + code[++ip] ];
          continue;


        case BardOp.write_local_Object:
          Object_stack[ fp + code[++ip] ] = Object_stack[sp++];
          continue;

        case BardOp.write_local_Real:
          Real_stack[ fp + code[++ip] ] = Real_stack[sp++];
          continue;

        case BardOp.write_local_Integer:
          Integer_stack[ fp + code[++ip] ] = Integer_stack[sp++];
//if (tron) System.out.println( "  Writing " + Integer_stack[sp] + ", stack now at " + sp );
          continue;

        case BardOp.write_local_Native:
          Native_stack[ fp + code[++ip] ] = Native_stack[sp++];
          continue;

        case BardOp.increment_local_Object:
          throw new RuntimeException( "TODO" );

        case BardOp.increment_local_Real:
          ++Real_stack[ fp + code[++ip] ];
          continue;

        case BardOp.increment_local_Integer:
          ++Integer_stack[ fp + code[++ip] ];
          continue;

        case BardOp.decrement_local_Object:
          throw new RuntimeException( "TODO" );

        case BardOp.decrement_local_Real:
          --Real_stack[ fp + code[++ip] ];
          continue;

        case BardOp.decrement_local_Integer:
          --Integer_stack[ fp + code[++ip] ];
          continue;

        case BardOp.read_property_Object:
          {
            BardObject context = Object_stack[ sp ];
            Object_stack[sp] = context.read_Object( code[++ip] );
          }
          continue;

        case BardOp.read_property_Real:
          {
            BardObject context = Object_stack[ sp ];
            Real_stack[sp] = context.read_Real( code[++ip] );
          }
          continue;

        case BardOp.read_property_Integer:
          {
            BardObject context = Object_stack[ sp ];
            Integer_stack[sp] = context.read_Integer( code[++ip] );
          }
          continue;

        case BardOp.read_property_Character:
          {
            BardObject context = Object_stack[ sp ];
            Integer_stack[sp] = context.read_Character( code[++ip] );
          }
          continue;

        case BardOp.read_property_Logical:
          {
            BardObject context = Object_stack[ sp ];
            Integer_stack[sp] = context.read_Logical( code[++ip] );
          }
          continue;

        case BardOp.read_property_Native:
          {
            BardObject context = Object_stack[ sp ];
            Native_stack[sp] = context.read_Native( code[++ip] );
          }
          continue;

        case BardOp.write_property_Object:
          {
            BardObject context = Object_stack[ sp+1 ];
            context.write_Object( code[++ip], Object_stack[sp] );
            sp += 2;
          }
          continue;

        case BardOp.write_property_Real:
          {
            BardObject context = Object_stack[ sp+1 ];
            context.write_Real( code[++ip], Real_stack[sp] );
            sp += 2;
          }
          continue;

        case BardOp.write_property_Integer:
          {
            BardObject context = Object_stack[ sp+1 ];
            context.write_Integer( code[++ip], Integer_stack[sp] );
            sp += 2;
          }
          continue;

        case BardOp.write_property_Character:
          {
            BardObject context = Object_stack[ sp+1 ];
            context.write_Character( code[++ip], Integer_stack[sp] );
            sp += 2;
          }
          continue;

        case BardOp.write_property_Logical:
          {
            BardObject context = Object_stack[ sp+1 ];
            context.write_Logical( code[++ip], Integer_stack[sp] );
            sp += 2;
          }
          continue;

        case BardOp.write_property_Native:
          {
            BardObject context = Object_stack[ sp+1 ];
            context.write_Native( code[++ip], Native_stack[sp] );
            sp += 2;
          }
          continue;

        case BardOp.create_object:
          {
            BardType of_type = vm.type_list.get( code[++ip] );
            Object_stack[ --sp ] = of_type.create_object();

            // init_object
            BardMethod m = of_type.m_init_object;

            method_stack[ --call_sp ] = this_method;
            fp_stack[ call_sp ] = fp;
            ip_stack[ call_sp ] = ip;

            fp = sp;
            sp -= m.local_count;
            code = m.code;
            this_method = m;
            ip = m.initial_ip;
          }
          continue;

        case BardOp.read_singleton:
          Object_stack[ --sp ] = vm.singletons.get( code[++ip] );
          continue;

        case BardOp.dynamic_call:
          {
            int param_count = code[++ip];
//System.out.println( "Calling " + vm.id_table.get_name(code[ip+1]) );
            BardType context_type = Object_stack[sp+param_count].type;

            int method_id = code[++ip];
            BardMethod m = null;
            BardMethod[] methods = context_type.methods.data;
            int count = context_type.methods.count;
            for (int i=0; i<count; ++i)
            {
              BardMethod m2 = methods[i];
              if (m2.method_id == method_id)
              {
                m = m2;
                break;
              }
            }
            //BardMethod m = context_type.methods.find( code[++ip] );

if (tron) 
{
  if (m == null) 
  {
    for (int i=0; i<context_type.methods.count; ++i)
    {
      BardMethod other_m = context_type.methods.get(i);
      System.out.print( "  " + other_m.signature );
      if (other_m.return_type != null) System.out.print( "." + other_m.return_type.name );
      System.out.println();
    }
    throw new RuntimeException( "No match for " + context_type + "::" + vm.id_table.get_name(method_id) );
  }
  System.out.print( "Calling " + context_type + "::" + m.signature );
  if (m.return_type != null)
  {
    System.out.print( "." );
    System.out.print( m.return_type.name );
  }
  System.out.println( " sp:" + sp + " native:" + (m.native_method!=null));
}
//if (m == null) for (BardMethod m2 : context_type.methods) System.out.println( m2.signature );
            if (m.native_method != null)
            {
              this.code = code;
              this.ip   = ip;
              this.sp   = sp;
              this.fp   = fp;

              m.native_method.execute( this );

              if (sp + m.expected_stack_delta != this.sp)
              {
                throw error( "Stack corruption detected in native method " + m.type_context.name + "::" + m.signature + "." );
              }

              code = this.code;
              ip   = this.ip;
              sp   = this.sp;
              fp   = this.fp;
            }
            else
            {
//System.out.println( "Calling " + Object_stack[sp+param_count].type.name + "::" + m.signature );
              method_stack[ --call_sp ] = this_method;
              fp_stack[ call_sp ] = fp;
              ip_stack[ call_sp ] = ip;

              fp = sp + param_count;
              sp -= m.local_count;
              code = m.code;
              this_method = m;
//System.out.println( m.type_context.name + "::" + m.signature );
              ip = m.initial_ip;
            }
          }
          continue;

          /*
        case BardOp.List_create:
          Object_stack[--sp] = ((BardList)vm.type_List.create_object()).init( Integer_stack[code[++ip]] );
          continue;

        case BardOp.List_count:
          {
            BardList list = (BardList) Object_stack[sp];
            Integer_stack[sp] = list.count;
          }
          continue;

        case BardOp.List_get:
          {
            // Value to Variant
            int index = Integer_stack[ sp ];
            BardList   list = (BardList) Object_stack[++sp];
            BardObject obj = list.data[ index ];
            sp = Variant_to_stack( sp, obj );
          }
          continue;

        case BardOp.List_set:
          {
            // Variant to Value
            BardObject obj = Object_stack[ sp ];
            int index_sp = sp + ((obj != null) ? BardType.variant_stack_slots[obj.type.variant_type] : 1);

            int index = Integer_stack[ index_sp ];
            BardList list = (BardList) Object_stack[index_sp+1];

            list.data[index] = stack_to_Variant( sp, obj, list.data[index] );
            sp = index_sp + 2;
          }
          continue;

        case BardOp.List_add:
          {
            // Variant to Value
            BardObject obj = Object_stack[ sp ];
            int next_sp = sp + ((obj != null) ? BardType.variant_stack_slots[obj.type.variant_type] : 1);
            obj = stack_to_Variant( sp, obj, null );

            sp = next_sp;
            BardList list = (BardList) Object_stack[sp];
            list.add( obj );
            // leave list on stack
          }
          continue;

        case BardOp.List_insert:
          {
            int before_index = Integer_stack[sp];

            // Variant to Value
            BardObject obj = Object_stack[ ++sp ];
            int next_sp = sp + ((obj != null) ? BardType.variant_stack_slots[obj.type.variant_type] : 1);
            obj = stack_to_Variant( sp, obj, null );
            sp = next_sp;

            BardList list = (BardList) Object_stack[sp];
            list.insert( obj, before_index );
            // leave list on stack
          }
          continue;

        case BardOp.List_clear:
          {
            BardList list = (BardList) Object_stack[sp];
            list.clear();
            // leave list on stack
          }
          continue;

        case BardOp.List_remove_at:
          {
            int index = Integer_stack[ sp ];
            BardList list = (BardList) Object_stack[++sp];
            BardObject result = list.remove_at( index );
            sp = Variant_to_stack( sp, result );
          }
          continue;

        case BardOp.Table_create:
          Object_stack[--sp] = vm.type_Table.create_object();
          continue;

        case BardOp.Table_count:
          {
            BardTable table = (BardTable) Object_stack[sp];
            Integer_stack[sp] = table.count;
            continue;
          }

        case BardOp.Table_set:
          {
            BardObject value = Object_stack[ sp ];
            int key_sp = sp + 1;

            BardObject key = Object_stack[ key_sp ];
            int table_sp = key_sp + 1;

            BardTable table = (BardTable) Object_stack[ table_sp ];

            key = stack_to_Variant( key_sp, key, table.test_key );
            table.test_key = key;

            int index = table.find( key );
            boolean entry_exists;
            BardObject existing_value;
            if (index >= 0)
            {
              entry_exists = true;
              existing_value = table.values[ index ];
            }
            else
            {
              entry_exists = false;
              existing_value = null;
            }

            if (value != null) value = stack_to_Variant( sp, value, existing_value );

            if (value != existing_value) table.set( key, value );

            if ( !entry_exists ) table.test_key = null;  // test key object has been stored with the table

            sp = table_sp;
          }
          continue;

        case BardOp.Table_get_key:
          {
            BardObject key = Object_stack[ sp ];
            int table_sp = sp + 1;

            BardTable  table = (BardTable) Object_stack[table_sp];

            key = stack_to_Variant( sp, key, table.test_key );
            if (key != null)
            {
              table.test_key = key;

              BardObject value = table.get( key );
              if (value != null)
              {
                sp = Variant_to_stack( table_sp, value );
              }
              else
              {
                sp = table_sp;
                Object_stack[ sp ] = vm.NullObjectVariant_singleton;
              }
            }
            else
            {
              sp = table_sp;
              Object_stack[ sp ] = vm.NullObjectVariant_singleton;
            }
          }
          continue;
          */

        case BardOp.tron:
          tron = true;
          continue;

        case BardOp.troff:
          tron = false;
          continue;

        case BardOp.print_Object:
          {
            BardObject obj = Object_stack[ sp++ ];
            if (obj == null)
            {
              System.out.print( "null" );
            }
            else
            {
              System.out.print( "TODO: print_Object" );
            }
          }
          continue;

        case BardOp.print_String:
          {
            BardString st = (BardString) Object_stack[sp++];
            if (st == null)
            {
              System.out.print( "null" );
            }
            else
            {
              int len = st.count;
              char[] data = st.data;
              for (int i=0; i<len; ++i)
              {
                System.out.print( data[i] );
              }
            }
          }
          continue;

        case BardOp.print_Real:
          System.out.print( BardValue.format( Real_stack[sp++], 4 ) );
          continue;

        case BardOp.print_Integer:
          System.out.print( Integer_stack[sp++] );
          continue;

        case BardOp.print_Character:
          System.out.print( (char) Integer_stack[sp++] );
          continue;

        case BardOp.print_Logical:
          System.out.print( (Integer_stack[sp++] != 0) ? "true" : "false" );
          continue;

        case BardOp.throw_missing_return:
          System.out.println( "Missing return value in " + this_method.type_context.name + "::" + this_method.signature );
          this.sp = fp;
          this.fp = fp_stack[ call_sp ];
          this.ip = ip_stack[ call_sp ];
          this.code = code;
          while (call_sp != call_stack_size)
          {
            pop_stack_frame();
          }
          return;

        case BardOp.bitwise_not_Integer:
          Integer_stack[sp] = ~Integer_stack[sp];
          continue;

        default:
          throw new RuntimeException( "Unhandled opcode: " + describe_op(code[ip]) );
      }
    }
    }
    catch (RuntimeException err)
    {
      System.out.println( "Null reference exception in " + this_method.type_context.name + "::" + this_method.signature );
      throw err;
    }
  }

  public RuntimeException error( String message )
  {
    return new RuntimeException( message );
  }

  Class class_BardOp = new BardOp().getClass();

  public String describe_op( int op )
  {
    try
    {
      Field[] fields = class_BardOp.getFields();
      for (Field f : fields)
      {
        if (f.getInt(null) == op) return op + " " + f.getName();
      }
      return ""+op;
    }
    catch (Exception err)
    {
      return ""+op;
    }
  }
}

