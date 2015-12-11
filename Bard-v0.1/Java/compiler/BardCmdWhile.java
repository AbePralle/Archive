package compiler;

import alphavm.BardOp;

public class BardCmdWhile extends BardCmdStatement
{
  public BardCmd           condition;
  public BardStatementList body = new BardStatementList();

  public BardCmdWhile init( BardCmd condition )
  {
    this.condition = condition;
    return this;
  }

  public BardCmd duplicate()
  {
    BardCmdWhile result = new BardCmdWhile().init( condition.duplicate() );
    result.set_source_info( this );
    result.body = body.duplicate();
    return result;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    condition = condition.resolve( method_context );
    condition.require_Logical();
    body.resolve( method_context );
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    int control_id = writer.next_control_id();

    writer.write( BardOp.jump );
    writer.write_address( control_id, "condition" );

    writer.define_address( control_id, "body" );
    body.compile( writer );

    writer.define_address( control_id, "condition" );
    condition.compile( writer );
    writer.write( BardOp.jump_if_true );
    writer.write_address( control_id, "body" );

    writer.define_address( control_id, "end" );
  }
}

