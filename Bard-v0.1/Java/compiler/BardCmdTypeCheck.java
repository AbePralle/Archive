package compiler;

import alphavm.BardOp;

abstract public class BardCmdTypeCheck extends BardCmdUnaryOp
{
  public BardType reference_type;

  public BardCmdTypeCheck init( BardCmd operand, BardType reference_type )
  {
    this.operand = operand;
    this.reference_type = reference_type;
    return this;
  }

  public BardType type() { return info.bc.type_Logical; }

  public BardCmd resolve( BardMethod method_context )
  {
    operand = operand.resolve( method_context ).require_value();
    return this;
  }

  static public class InstanceOf extends BardCmdTypeCheck
  {
    public BardCmd duplicate() { return new InstanceOf().init( operand.duplicate(), reference_type ).set_source_info( this ); }
    public String name()       { return "instanceOf"; }

    public void compile( BardCodeWriter writer )
    {
      operand.compile( writer );

      BardType operand_type = operand.type();
      BardCompiler bc = info.bc;
      if (operand_type == bc.type_Variant)
      {
        writer.write( BardOp.Variant_instanceOf );
      }
      else if (operand_type.instance_of(bc.type_Object))
      {
        writer.write( BardOp.Fixed_instanceOf );
      }
      writer.write( reference_type.index );
    }
  }
}

