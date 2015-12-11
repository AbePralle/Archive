package compiler;

import alphavm.BardOp;

public class BardCmdBlock extends BardCmdStatement
{
  public BardStatementList body;

  public BardCmdBlock()
  {
    body = new BardStatementList();
  }

  public BardCmdBlock( BardStatementList statements )
  {
    body = statements;
  }

  public BardCmd duplicate()
  {
    return new BardCmdBlock( body.duplicate() ).set_source_info( this );
  }

  public BardCmd resolve( BardMethod method_context )
  {
    method_context.open_scope();
    body.resolve( method_context );
    method_context.close_scope();
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    int control_id = writer.next_control_id();

    body.compile( writer );
    writer.define_address( control_id, "end" );
  }
}

