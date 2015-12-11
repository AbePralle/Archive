package compiler;

import alphavm.BardOp;

public class BardCmdReadProperty extends BardCmd
{
  public BardCmd      context;
  public BardProperty property;

  public BardCmdReadProperty( BardCmd context, BardProperty property )
  {
    this.context  = context;
    this.property = property;
  }

  public BardCmd duplicate() { return new BardCmdReadProperty( duplicate(context), property ).set_source_info( this ); }

  public String toString()
  {
    if (context != null)
    {
      return context.toString() + "." + property.name;
    }
    else
    {
      return property.name;
    }
  }

  public BardType type()
  {
    return property.type;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    if (context != null) context = context.resolve( method_context );
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    if (context != null)
    {
      context.compile( writer );
    }
    else
    {
      writer.write( BardOp.push_Object_this );
    }

    BardType type = property.type;
    int opcode = -1;
    //if (type.is_String())       opcode = BardOp.read_property_String;
    if (type.is_Real())           opcode = BardOp.read_property_Real;
    else if (type.is_Integer())   opcode = BardOp.read_property_Integer;
    else if (type.is_Character()) opcode = BardOp.read_property_Character;
    else if (type.is_Logical())   opcode = BardOp.read_property_Logical;
    else if (type.is_Variant())   opcode = BardOp.read_property_Variant;
    else                          opcode = BardOp.read_property_Object;

    if (opcode == -1)
    {
      throw new RuntimeException( "Unhandled property type in BardCmdReadProperty::resolve()." );
    }

    writer.write( opcode, property.index );
  }

  public BardCmd resolve_assignment( BardMethod method_context, BardCmd new_value )
  {
    throw new RuntimeException( "TODO: here" );
  }
}

