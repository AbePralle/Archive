package compiler;

import alphavm.BardOp;

public class BardCmdExpression extends BardCmd
{
  public BardCmd operand;

  public BardCmdExpression( BardCmd operand ) { this.operand = operand; }

  public BardCmd duplicate() { return new BardCmdExpression( duplicate(operand) ).set_source_info(this); }

  public BardType type() { return operand.type(); }
}

