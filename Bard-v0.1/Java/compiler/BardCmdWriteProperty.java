package compiler;

import alphavm.BardOp;

public class BardCmdWriteProperty extends BardCmd
{
  public BardCmd      context;
  public BardProperty property;
  public BardCmd      new_value;

  public BardCmdWriteProperty( BardCmd context, BardProperty property, BardCmd new_value )
  {
    this.context   = context;
    this.property  = property;
    this.new_value = new_value;
  }

  public BardCmd duplicate() { return new BardCmdWriteProperty( context.duplicate(), property, new_value.duplicate() ).set_source_info(this); }

  public BardType type()
  {
    return null;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    if (context != null) context = context.resolve( method_context );

    new_value = new_value.resolve( method_context ).cast_to(  property.type );
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

    BardType property_type = property.type;
    new_value.compile( writer );

    int opcode = -1;
    //if (property_type.is_String())       opcode = BardOp.write_property_String;
    if (property_type.is_Real())           opcode = BardOp.write_property_Real;
    else if (property_type.is_Integer())   opcode = BardOp.write_property_Integer;
    else if (property_type.is_Character()) opcode = BardOp.write_property_Character;
    else if (property_type.is_Logical())   opcode = BardOp.write_property_Logical;
    else if (property_type.is_Variant())   opcode = BardOp.write_property_Variant;
    else                                   opcode = BardOp.write_property_Object;

    if (opcode == -1)
    {
      throw new RuntimeException( "Unhandled property type in BardCmdWriteProperty::resolve()." );
    }

    writer.write( opcode, property.index );
  }
}

