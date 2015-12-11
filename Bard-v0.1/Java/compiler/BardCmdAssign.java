package compiler;

import alphavm.BardOp;

public class BardCmdAssign extends BardCmdStatement
{
  public BardCmd target;
  public BardCmd new_value;

  public BardCmdAssign( BardCmd target, BardCmd new_value )
  {
    this.target = target;
    this.new_value = new_value;
  }

  public BardCmd duplicate()
  {
    return new BardCmdAssign( target.duplicate(), new_value.duplicate() ).set_source_info(this);
  }

  public BardCmd resolve( BardMethod method_context )
  {
    return target.resolve_assignment( method_context, new_value );
  }
}

