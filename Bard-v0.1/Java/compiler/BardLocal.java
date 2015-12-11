package compiler;

import alphavm.BardOp;

public class BardLocal
{
  public String   name;
  public BardType type;
  public BardCmd  initial_value;

  public int      index;
  public int      scope_level;

  public BardLocal( String name )
  {
    this.name = name;
  }

  public BardLocal( String name, BardType type )
  {
    this.name = name;
    this.type = type;
  }
}

