package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardIDTable
{
  BardIntTable       id_table  = new BardIntTable();
  ArrayList<String>  name_list = new ArrayList<String>();

  public BardIDTable()
  {
  }

  public int get_id( String name )
  {
    int result = id_table.get( name );
    if (result >= 0) return result;
    id_table.set( name, name_list.size() );
    name_list.add( name );
    return name_list.size() - 1;
  }

  public int get_id( StringBuilder name )
  {
    int result = id_table.get( name );
    if (result >= 0) return result;

    String name_string = name.toString();
    id_table.set( name_string, name_list.size() );
    name_list.add( name_string );
    return name_list.size() - 1;
  }

  public String get_name( int id )
  {
    return name_list.get( id );
  }

  public String consolidate( String st )
  {
    return get_name( get_id(st) );
  }

  public String consolidate( StringBuilder st )
  {
    return get_name( get_id(st) );
  }
}

