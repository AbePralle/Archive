package compiler;

import alphavm.BardOp;

public class BardCmdLoop extends BardCmdStatement
{
  public BardCmd           count_expression;
  public BardStatementList body = new BardStatementList();
  public int               unique_id;

  public BardCmdLoop init( BardCmd count_expression )
  {
    this.count_expression = count_expression;
    return this;
  }

  public BardCmd duplicate() 
  { 
    BardCmdLoop result = new BardCmdLoop().init( duplicate(count_expression) );
    result.set_source_info(this);
    result.body = body.duplicate();
    result.unique_id = unique_id;
    return result;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    if (count_expression != null)
    {
      count_expression = count_expression.resolve( method_context );
    }
    body.resolve( method_context );
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    int control_id = writer.next_control_id();

    if (count_expression != null)
    {
      count_expression.compile( writer );
      writer.write( BardOp.jump );
      writer.write_address( control_id, "modifier" );
    }

    writer.define_address( control_id, "body" );
    body.compile( writer );

    if (count_expression != null)
    {
      writer.define_address( control_id, "modifier" );
      writer.write( BardOp.finite_loop );
      writer.write_address( control_id, "body" );
    }
    else
    {
      writer.write( BardOp.jump );
      writer.write_address( control_id, "body" );
    }
  }
}

