package compiler;

import alphavm.BardOp;

public class BardCmdAdjust extends BardCmdUnaryOp
{
  public int delta;

  public BardCmdAdjust( BardCmd operand, int delta )
  {
    this.operand = operand;
    this.delta = delta;
  }

  public BardCmd duplicate()
  {
    return new BardCmdAdjust( operand.duplicate(), delta ).set_source_info(this);
  }

  public BardType type() { return null; }

  public BardCmd resolve( BardMethod method_context )
  {
    return operand.resolve_adjustment( method_context, delta );
  }

  abstract static public class AdjustLocal extends BardCmd
  {
    public BardLocal local_info;

    public BardCmd init( BardLocal local_info )
    {
      this.local_info = local_info;
      return this;
    }

    public BardType type() { return null; }

    public BardCmd resolve( BardMethod method_context ) { return this; }
  }

  static public class IncrementLocal extends AdjustLocal
  {
    public BardCmd duplicate()
    {
      return new IncrementLocal().init( local_info ).set_source_info(this);
    }

    public void compile( BardCodeWriter writer )
    {
      BardCompiler   bc = writer.bc;
      BardType local_type = local_info.type;

      int opcode = 0;
      if (local_type == bc.type_Real)
      {
        opcode = BardOp.increment_local_Real;
      }
      else if (local_type == bc.type_Integer || local_type == bc.type_Character)
      {
        opcode = BardOp.increment_local_Integer;
      }
      else
      {
        throw error( "'++' cannot be applied to type " + local_type + "." );
      }

      writer.write( opcode, local_info.index );
    }
  }

  static public class DecrementLocal extends AdjustLocal
  {
    public BardCmd duplicate()
    {
      return new DecrementLocal().init( local_info ).set_source_info(this);
    }

    public void compile( BardCodeWriter writer )
    {
      BardCompiler   bc = writer.bc;
      BardType local_type = local_info.type;

      int opcode = 0;
      if (local_type == bc.type_Real)
      {
        opcode = BardOp.decrement_local_Real;
      }
      else if (local_type == bc.type_Integer || local_type == bc.type_Character)
      {
        opcode = BardOp.decrement_local_Integer;
      }
      else
      {
        throw error( "'--' cannot be applied to type " + local_type + "." );
      }

      writer.write( opcode, local_info.index );
    }
  }
}
