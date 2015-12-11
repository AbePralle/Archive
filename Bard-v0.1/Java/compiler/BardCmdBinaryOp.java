package compiler;

import alphavm.BardOp;

public class BardCmdBinaryOp extends BardCmd
{
  public BardCmd lhs, rhs;

  public BardCmdBinaryOp()
  {
  }

  public BardCmd duplicate()
  {
    return new BardCmdBinaryOp().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this);
  }

  public String toString()
  {
    return lhs + " " + name() + " " + rhs;
  }

  public BardCmdBinaryOp init( BardCmd lhs, BardCmd rhs )
  {
    this.lhs = lhs;
    this.rhs = rhs;
    return this;
  }

  public BardType type()
  {
    return common_type( lhs.type(), rhs.type() );
  }

  public BardCmd resolve( BardMethod method_context )
  {
    lhs = lhs.resolve( method_context );
    rhs = rhs.resolve( method_context );

    BardCompiler bc = info.bc;
    BardType lhs_type = lhs.type(); 
    BardType rhs_type = rhs.type();

    BardCmd result = resolve_for_types( method_context, lhs_type, rhs_type );
    if (result != null) return result;

    BardType common_type = common_type( lhs_type, rhs_type );
    lhs = lhs.cast_to( common_type );
    rhs = rhs.cast_to( common_type );

    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    BardCompiler bc = writer.bc;

    lhs.compile( writer );
    rhs.compile( writer );

    BardType common_type = lhs.type();

    int opcode = -1;
    if (common_type == bc.type_Variant)        opcode = opcode_Variant();
    else if (common_type == bc.type_Real)      opcode = opcode_Real();
    else if (common_type == bc.type_Integer)   opcode = opcode_Integer();
    else if (common_type == bc.type_Character) opcode = opcode_Character();
    else if (common_type == bc.type_Logical)   opcode = opcode_Logical();
    else                                       opcode = opcode_Object();

    if (opcode == -1)
    {
      throw error( "Illegal operands of type " + common_type.name + " for '" + name() + "' operation." );
    }
    else
    {
      writer.write( opcode );
    }

  }

  public BardCmd resolve_for_types( BardMethod method_context, BardType lhs_type, BardType rhs_type )
  {
    return null;
  }

  static public class Add extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new Add().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }

    public BardCmd resolve_for_types( BardMethod method_context, BardType lhs_type, BardType rhs_type )
    {
      BardCompiler bc = lhs_type.bc;
      boolean have_String = (lhs_type.instance_of(bc.type_String)) || (rhs_type.instance_of(bc.type_String));
      have_String |= (lhs_type == bc.type_Character) || (rhs_type == bc.type_Character);
      boolean have_Variant = (lhs_type == bc.type_Variant) || (rhs_type == bc.type_Variant);
      if (have_String || have_Variant)
      {
        BardCmd cmd = new BardCmdAccess( "Character[]", new BardArgs() ).set_source_info(this);
        cmd = new BardCmdContextAccess( cmd, "print", new BardArgs(lhs) ).set_source_info(this);
        cmd = new BardCmdContextAccess( cmd, "print", new BardArgs(rhs) ).set_source_info(this);
        return cmd.resolve( method_context );
      }

      return null;
    }

    public String name()        { return "+"; }
    //public int opcode_String()  { return BardOp.add_String; }
    public int opcode_Real()    { return BardOp.add_Real; }
    public int opcode_Integer() { return BardOp.add_Integer; }
    public int opcode_Logical() { return BardOp.bitwise_or_Integer; }
  }

  static public class Subtract extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new Subtract().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "-"; }
    public int opcode_Real()  { return BardOp.subtract_Real; }
    public int opcode_Integer()   { return BardOp.subtract_Integer; }
  }

  static public class Multiply extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new Multiply().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "*"; }
    public int opcode_Real()  { return BardOp.multiply_Real; }
    public int opcode_Integer()   { return BardOp.multiply_Integer; }
    public int opcode_Logical() { return BardOp.bitwise_and_Integer; }

    public BardCmd resolve_for_types( BardMethod method_context, BardType lhs_type, BardType rhs_type )
    {
      BardCompiler bc = info.bc;

      if (lhs_type == bc.type_Character) lhs_type = bc.type_String;
      if (rhs_type == bc.type_Character) rhs_type = bc.type_String;
      if (lhs_type == bc.type_Integer && rhs_type == bc.type_String)
      {
        return new MultiplyStringAndInteger().init( rhs.cast_to(bc.type_String), lhs ).set_source_info(this).resolve( method_context );
      }
      else if (lhs_type == bc.type_String && rhs_type == bc.type_Integer)
      {
        return new MultiplyStringAndInteger().init( lhs.cast_to(bc.type_String), rhs ).set_source_info(this).resolve( method_context );
      }
      else
      {
        return null;
      }
    }
  }

  static public class MultiplyStringAndInteger extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new MultiplyStringAndInteger().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "*"; }

    public BardCmd resolve( BardMethod method_context )
    {
      lhs = lhs.resolve( method_context );
      rhs = rhs.resolve( method_context );
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      throw new RuntimeException( "TODO" );
      //lhs.compile( writer );
      //rhs.compile( writer );
      //writer.write( BardOp.multiply_String_and_Integer );
    }
  }

  static public class Divide extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new Divide().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "/"; }
    public int opcode_Real()  { return BardOp.divide_Real; }

    public BardType common_type( BardType t1, BardType t2 )
    {
      return info.bc.type_Real;
    }
  }

  static public class Mod extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new Mod().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "%"; }
    public int opcode_Real()  { return BardOp.mod_Real; }
    public int opcode_Integer()   { return BardOp.mod_Integer; }
  }

  static public class Power extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new Power().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "^"; }
    public int opcode_Real()  { return BardOp.power_Real; }
    public int opcode_Integer()   { return BardOp.power_Integer; }
  }

  static public class BitwiseAnd extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new BitwiseAnd().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "&"; }
    public int opcode_Integer()   { return BardOp.bitwise_and_Integer; }
  }

  static public class BitwiseOr extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new BitwiseOr().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "|"; }
    public int opcode_Integer()   { return BardOp.bitwise_or_Integer; }
  }

  static public class BitwiseXor extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new BitwiseXor().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return "~"; }
    public int opcode_Integer()   { return BardOp.bitwise_xor_Integer; }
  }

  static public class SHL extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new SHL().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return ":<<:"; }
    public int opcode_Integer()   { return BardOp.shl_Integer; }
  }

  static public class SHR extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new SHR().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return ":>>:"; }
    public int opcode_Integer()   { return BardOp.shr_Integer; }
  }

  static public class SHRX extends BardCmdBinaryOp
  {
    public BardCmd duplicate() { return new SHRX().init( lhs.duplicate(), rhs.duplicate() ).set_source_info(this); }
    public String name()        { return ":>>>:"; }
    public int opcode_Integer()   { return BardOp.shrx_Integer; }
  }
}

