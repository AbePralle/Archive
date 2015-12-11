package compiler;

import alphavm.BardOp;

public class BardProperty
{
  public String   name;
  public BardType type;
  public BardType type_context;
  public BardCmd  initial_value;

  public int      property_id;
  public int      index;

  public BardProperty( BardType type_context, String name, BardCmd initial_value )
  {
    this.type_context = type_context;
    this.name = name;
    this.initial_value = initial_value;
    property_id = type_context.bc.id_table.get_id( name );
  }
}

