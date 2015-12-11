package compiler;

import alphavm.BardOp;

public class BardCmdReturn extends BardCmdStatement
{
  public BardCmd operand;

  public BardCmdReturn init( BardCmd operand )
  {
    this.operand = operand;
    return this;
  }

  public BardCmd duplicate() { return new BardCmdReturn().init( duplicate(operand) ).set_source_info(this); }

  public BardCmd resolve( BardMethod method_context )
  {
    BardCompiler bc = method_context.type_context.bc;

    if (operand != null)
    {
      if (method_context.return_type == null) throw error( "No return type is defined." );
      operand = operand.resolve( method_context ).require_value().cast_to( method_context.return_type );
    }
    else
    {
      if (method_context.return_type != null) throw error( "Return value expected." );
    }

    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    if (operand == null)
    {
      writer.write( BardOp.return_nil );
      return;
    }

    BardCompiler   bc = writer.bc;
    BardType return_type = writer.this_method.return_type;

    operand.compile( writer );

    if (return_type == bc.type_Real) writer.write( BardOp.return_Real );
    else if (return_type == bc.type_Integer || return_type == bc.type_Character || return_type == bc.type_Logical) 
    {
                                            writer.write( BardOp.return_Integer );
    }
    else if (return_type == bc.type_Variant) writer.write( BardOp.return_Variant );
    else                                     writer.write( BardOp.return_Object );
  }
}

