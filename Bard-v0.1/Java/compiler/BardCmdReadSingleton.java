package compiler;

import alphavm.BardOp;

public class BardCmdReadSingleton extends BardCmd
{
  public BardType of_type;

  public BardCmdReadSingleton( BardType of_type )
  {
    this.of_type = of_type;
  }

  public BardCmd duplicate() { return new BardCmdReadSingleton( of_type ).set_source_info(this); }

  public BardType type() { return of_type; }

  public BardCmd resolve( BardMethod method_context ) { return this; }

  public void compile( BardCodeWriter writer )
  {
    writer.write( BardOp.read_singleton, of_type.index );
  }
}
