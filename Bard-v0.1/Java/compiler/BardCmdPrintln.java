package compiler;

import alphavm.BardOp;

public class BardCmdPrintln extends BardCmdStatement
{
  public BardCmd expression;

  public BardCmd duplicate() 
  {
    return new BardCmdPrintln().init( expression ).set_source_info( this );
  }

  public BardCmdPrintln init( BardCmd expression )
  {
    this.expression = expression;
    return this;
  }

  public BardCmd resolve( BardMethod m )
  {
    if (expression != null)
    {
      expression = expression.resolve( m );
      expression = expression.require_value().cast_to( info.bc.type_Variant );
    }
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    if (expression == null)
    {
      writer.write( BardOp.println_nil );
      return;
    }

    expression.compile(writer);
    writer.write( BardOp.println_Variant );
  }
}

