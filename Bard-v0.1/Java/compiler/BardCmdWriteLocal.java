package compiler;

import alphavm.BardOp;

public class BardCmdWriteLocal extends BardCmd
{
  public BardLocal local_info;
  public BardCmd   new_value;

  public BardCmdWriteLocal( BardLocal local_info, BardCmd new_value )
  {
    this.local_info = local_info;
    this.new_value  = new_value;
  }

  public BardCmd duplicate() { return new BardCmdWriteLocal( local_info, duplicate(new_value) ).set_source_info( this ); }

  public BardType type()
  {
    return null;
  }

  public String toString()
  {
    return local_info.name + " = " + new_value;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    if (new_value == null)
    {
      switch (local_info.type.variant_type)
      {
        case BardType.variant_Real:     new_value = new BardCmdLiteral.LiteralReal(0.0); break;
        case BardType.variant_Integer:  new_value = new BardCmdLiteral.LiteralInteger(0); break;
        case BardType.variant_Character:     new_value = new BardCmdLiteral.LiteralCharacter(0); break;
        case BardType.variant_Logical:  new_value = new BardCmdLiteral.LiteralLogical(false); break;
        default:                        new_value = new BardCmdLiteral.LiteralNull(); break;
      }
      new_value.set_source_info( this );
    }

    new_value = new_value.resolve( method_context ).cast_to( local_info.type );
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    BardCompiler   bc = writer.bc;
    BardType local_type = local_info.type;
    int      index      = local_info.index;

    new_value.compile( writer );

    if (local_type == bc.type_Real)
    {
      writer.write( BardOp.write_local_Real, index );
    }
    else if (local_type == bc.type_Integer || local_type == bc.type_Character || local_type == bc.type_Logical)
    {
      writer.write( BardOp.write_local_Integer, index );
    }
    else if (local_type == bc.type_Variant)
    {
      writer.write( BardOp.write_local_Variant, index );
    }
    else
    {
      writer.write( BardOp.write_local_Object, index );
    }
  }
}

