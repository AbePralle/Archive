package compiler;

import alphavm.BardOp;

public class BardCmdThis extends BardCmd
{
  public BardType type;

  public BardCmdThis( BardType type )
  {
    this.type = type;
  }

  public BardCmd duplicate() { return new BardCmdThis(type).set_source_info(this); }
  
  public BardType type() { return type; }

  public BardCmd resolve( BardMethod method_context )
  {
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    writer.write( BardOp.push_Object_this );
  }
}

