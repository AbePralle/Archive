package compiler;

import alphavm.BardOp;

public class BardCmdThrow extends BardCmdStatement
{
  public BardCmd operand;

  public BardCmdThrow init( BardCmd operand )
  {
    this.operand = operand;
    return this;
  }

  public BardCmd duplicate() { return new BardCmdThrow().init( duplicate(operand) ).set_source_info(this); }

  public BardCmd resolve( BardMethod method_context )
  {
    BardCompiler bc = method_context.type_context.bc;
    operand = operand.resolve( method_context ).require_value();
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    operand.compile( writer );
    writer.write( BardOp.throw_Exception );
  }
}

