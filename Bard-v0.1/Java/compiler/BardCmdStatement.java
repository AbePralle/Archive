package compiler;

import alphavm.BardOp;

abstract public class BardCmdStatement extends BardCmd
{
  public BardType type()
  {
    return null;
  }

  static public class Tron extends BardCmdStatement
  {
    public BardCmd resolve( BardMethod method_context ) { return this; }

    public void compile( BardCodeWriter writer )
    {
      //BardCmdIf.tron = true;
      writer.write( BardOp.tron );
    }
  }

  static public class Troff extends BardCmdStatement
  {
    public BardCmd resolve( BardMethod method_context ) { return this; }

    public void compile( BardCodeWriter writer )
    {
      //BardCmdIf.tron = false;
      writer.write( BardOp.troff );
    }
  }
}

