package compiler;

import alphavm.BardOp;

public class BardFileInfo
{
  public BardCompiler bc;
  public String filepath;

  public BardFileInfo( BardCompiler bc, String filepath )
  {
    this.bc = bc;
    this.filepath = filepath;
  }
}
