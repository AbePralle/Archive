package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardCodeWriter
{
  public BardCompiler    bc;
  public BardMethod      this_method;
  public BardIntegerList code_buffer     = new BardIntegerList();
  public BardStringList  String_literals = new BardStringList();
  public BardRealList    Real_literals   = new BardRealList();

  // Not using objects here in order to minimize the quantity of temporary objects.
  public BardIntegerList  closed_ids       = new BardIntegerList();
  public BardStringList closed_tags      = new BardStringList();
  public BardIntegerList  closed_addresses = new BardIntegerList();
  public BardIntegerList  open_ids         = new BardIntegerList();
  public BardStringList open_tags        = new BardStringList();
  public BardIntegerList  open_positions   = new BardIntegerList();

  public int previous_control_id;

  public BardCodeWriter( BardCompiler bc )
  {
    this.bc = bc;
  }

  public void reset( BardMethod this_method )
  {
    this.this_method = this_method;
    previous_control_id = 0;
    code_buffer.clear();
    String_literals.clear();
    Real_literals.clear();
    closed_ids.clear();
    closed_tags.clear();
    closed_addresses.clear();
    open_ids.clear();
    open_tags.clear();
    open_positions.clear();
  }

  public void finish()
  {
    if (open_tags.count > 0)
    {
      throw new RuntimeException( "Open control scope at end of method definition: " + open_tags.get(open_tags.count-1) ); 
    }
  }

  public int next_control_id()
  {
    ++previous_control_id;
    return previous_control_id;
  }

  public int   position()
  {
    return code_buffer.count;
  }

  public int[] to_code()
  {
    int count = code_buffer.count;
    int[] data = new int[ count ];

      System.arraycopy( code_buffer.data, 0, data, 0, code_buffer.count );

    return data;
  }

  public void write( int value )
  {
    code_buffer.add( value );
  }

  public void write( int v1, int v2 )
  {
    code_buffer.add( v1 );
    code_buffer.add( v2 );
  }

  public void write_Object_call( String signature )
  {
    BardMethod m = bc.type_Object.find_method( signature );
    if (m == null) throw new BardError( "No such method Object::" + signature );
    write( BardOp.dynamic_call );
    write( 0, m.method_id );
  }

  public void write_default_return( BardType for_type )
  {
    if (for_type == null)
    {
      write( BardOp.return_nil );
    }
    else if (for_type == bc.type_Real)
    {
      write( BardOp.push_literal_Real_0 );
      write( BardOp.return_Real );
    }
    else if (for_type == bc.type_Integer)
    {
      write( BardOp.push_literal_Integer_0 );
      write( BardOp.return_Integer );
    }
    else if (for_type == bc.type_Character || for_type == bc.type_Logical)
    {
      write( BardOp.push_literal_Integer_0 );
      write( BardOp.return_Integer );
    }
    else
    {
      write( BardOp.push_literal_Object_null );
      write( BardOp.return_Object );
    }
  }

  public int index_of_String( String n )
  {
    String_literals.add( n );
    return String_literals.count - 1;
  }

  public int index_of_Real( double n )
  {
    Real_literals.add( n );
    return Real_literals.count - 1;
  }

  public void write_address( int control_id, String tag )
  {
    for (int i=closed_ids.count-1; i>=0; --i)
    {
      if (closed_ids.get(i) == control_id && closed_tags.get(i).equals(tag))
      {
        int dest_ip = closed_addresses.get(i);
        int src_ip = code_buffer.count;
        write( (dest_ip-src_ip)-1 );
        return;
      }
    }

    open_ids.add( control_id );
    open_tags.add( tag );
    open_positions.add( position() );
    write( -1000 );  // placeholder; -1 would be the beginning of the method so -2 to force crash on compile error
  }

  public void define_address( int control_id, String tag )
  {
    int address = position();
    closed_ids.add( control_id );
    closed_tags.add( tag );
    closed_addresses.add( address );

    // Backpatch
    for (int i=open_ids.count-1; i>=0; --i)
    {
      if (open_ids.get(i) == control_id && open_tags.get(i).equals(tag))
      {
        int src_ip = open_positions.get(i);
        code_buffer.set( src_ip, (address-src_ip)-1 );
        open_ids.remove_at( i );
        open_tags.remove_at( i );
        open_positions.remove_at( i );
      }
    }
  }
}

