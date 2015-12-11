package compiler;

import alphavm.BardOp;

public class BardCmdCreateObject extends BardCmd
{
  public BardType of_type;

  public BardCmdCreateObject( BardType of_type )
  {
    this.of_type = of_type;
  }

  public BardCmd duplicate() { return new BardCmdCreateObject( of_type ).set_source_info(this); }

  public String toString()
  {
    return "new " + of_type + "()";
  }

  public BardType type() { return of_type; }

  public BardCmd resolve( BardMethod method_context )
  {
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    writer.write( BardOp.create_object, of_type.index );
  }
}

