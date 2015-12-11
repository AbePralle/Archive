package compiler;

import alphavm.BardOp;

public class BardCmdReadLocal extends BardCmd
{
  public BardLocal local_info;

  public BardCmdReadLocal( BardLocal local_info )
  {
    this.local_info = local_info;
  }

  public BardCmd duplicate() { return new BardCmdReadLocal(local_info).set_source_info( this ); }

  public String toString()
  {
    return local_info.name;
  }

  public BardType type()
  {
    return local_info.type;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    BardCompiler   bc = writer.bc;
    BardType local_type = local_info.type;
    int      index      = local_info.index;

    if (local_type == bc.type_Real)
    {
      writer.write( BardOp.read_local_Real, index );
    }
    else if (local_type == bc.type_Integer || local_type == bc.type_Character || local_type == bc.type_Logical)
    {
      writer.write( BardOp.read_local_Integer, index );
    }
    else if (local_type == bc.type_Variant)
    {
      writer.write( BardOp.read_local_Variant, index );
    }
    else
    {
      writer.write( BardOp.read_local_Object, index );
    }
  }
}

