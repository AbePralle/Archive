package compiler;

import alphavm.BardOp;

public class BardCmdCmp extends BardCmdBinaryOp
{
  public BardCmd duplicate() { return new BardCmdCmp().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
  
  public BardType type() { return info.bc.type_Logical; }

  public boolean promote_Character_to_String() { return false; };

  static public class EQ extends BardCmdCmp
  {
    public BardCmd duplicate() { return new EQ().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return "=="; }
    public int opcode_Object()  { return BardOp.eq_Object; }
    //public int opcode_String()  { return BardOp.eq_String; }
    public int opcode_Real()  { return BardOp.eq_Real; }
    public int opcode_Integer()   { return BardOp.eq_Integer; }
    public int opcode_Character()   { return BardOp.eq_Integer; }
    public int opcode_Logical() { return BardOp.eq_Integer; }

    public BardCmd resolve_for_types( BardMethod method_context, BardType lhs_type, BardType rhs_type )
    {
      BardCompiler bc = lhs_type.bc;
      boolean have_String = (lhs_type.instance_of(bc.type_String)) || (rhs_type.instance_of(bc.type_String));
      boolean have_Variant = (lhs_type == bc.type_Variant) || (rhs_type == bc.type_Variant);
      if (have_String)
      {
        if (have_Variant)
        {
          lhs = lhs.cast_to( bc.type_String );
          rhs = rhs.cast_to( bc.type_String );
          BardCmd result = resolve( method_context );
          return result;
        }
        else
        {
          lhs = lhs.cast_to( bc.type_String );
          rhs = rhs.cast_to( bc.type_String );
          BardCmd cmd = new BardCmdContextAccess( lhs, "compare_to", new BardArgs(rhs) ).set_source_info(this);
          cmd = new BardCmdCmp.EQ().init( cmd, new BardCmdLiteral.LiteralInteger(0).set_source_info(this) ).set_source_info(this);
          return cmd.resolve( method_context );
        }
      }

      return null;
    }
  }

  static public class NE extends BardCmdCmp
  {
    public BardCmd duplicate() { return new NE().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return "!="; }
    public int opcode_Object()  { return BardOp.ne_Object; }
    //public int opcode_String()  { return BardOp.ne_String; }
    public int opcode_Real()  { return BardOp.ne_Real; }
    public int opcode_Integer()   { return BardOp.ne_Integer; }
    public int opcode_Character()   { return BardOp.ne_Integer; }
    public int opcode_Logical() { return BardOp.ne_Integer; }

    public BardCmd resolve_for_types( BardMethod method_context, BardType lhs_type, BardType rhs_type )
    {
      BardCompiler bc = lhs_type.bc;
      boolean have_String = (lhs_type.instance_of(bc.type_String)) || (rhs_type.instance_of(bc.type_String));
      boolean have_Variant = (lhs_type == bc.type_Variant) || (rhs_type == bc.type_Variant);
      if (have_String)
      {
        if (have_Variant)
        {
          lhs = lhs.cast_to( bc.type_String );
          rhs = rhs.cast_to( bc.type_String );
          BardCmd result = resolve( method_context );
          return result;
        }
        else
        {
          lhs = lhs.cast_to( bc.type_String );
          rhs = rhs.cast_to( bc.type_String );
          BardCmd cmd = new BardCmdContextAccess( lhs, "compare_to", new BardArgs(rhs) ).set_source_info(this);
          cmd = new BardCmdCmp.NE().init( cmd, new BardCmdLiteral.LiteralInteger(0).set_source_info(this) ).set_source_info(this);
          return cmd.resolve( method_context );
        }
      }

      return null;
    }
  }

  static public class LT extends BardCmdCmp
  {
    public BardCmd duplicate() { return new LT().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return "<"; }
    //public int opcode_String()  { return BardOp.lt_String; }
    public int opcode_Real()  { return BardOp.lt_Real; }
    public int opcode_Integer()   { return BardOp.lt_Integer; }
    public int opcode_Character()      { return BardOp.lt_Integer; }
    public int opcode_Logical() { return -1; }
  }

  static public class GT extends BardCmdCmp
  {
    public BardCmd duplicate() { return new GT().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return ">"; }
    //public int opcode_String()  { return BardOp.gt_String; }
    public int opcode_Real()  { return BardOp.gt_Real; }
    public int opcode_Integer()   { return BardOp.gt_Integer; }
    public int opcode_Character()      { return BardOp.gt_Integer; }
    public int opcode_Logical() { return -1; }
  }

  static public class LE extends BardCmdCmp
  {
    public BardCmd duplicate() { return new LE().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return "<="; }
    //public int opcode_String()  { return BardOp.le_String; }
    public int opcode_Real()  { return BardOp.le_Real; }
    public int opcode_Integer()   { return BardOp.le_Integer; }
    public int opcode_Character()      { return BardOp.le_Integer; }
    public int opcode_Logical() { return -1; }
  }

  static public class GE extends BardCmdCmp
  {
    public BardCmd duplicate() { return new GE().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return ">="; }
    //public int opcode_String()  { return BardOp.ge_String; }
    public int opcode_Real()  { return BardOp.ge_Real; }
    public int opcode_Integer()   { return BardOp.ge_Integer; }
    public int opcode_Character()      { return BardOp.ge_Integer; }
    public int opcode_Logical() { return -1; }
  }

  static public class And extends BardCmdCmp
  {
    public BardCmd duplicate() { return new And().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return "and"; }

    public void compile( BardCodeWriter writer )
    {
      BardCompiler bc = writer.bc;

      int control_id = writer.next_control_id();

      lhs.require_Logical().compile( writer );
      writer.write( BardOp.push_false_and_jump_if_false );
      writer.write_address( control_id, "end" );

      rhs.require_Logical().compile( writer );
      writer.define_address( control_id, "end" );
    }
  }

  static public class Or extends BardCmdCmp
  {
    public BardCmd duplicate() { return new Or().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return "or"; }

    public void compile( BardCodeWriter writer )
    {
      BardCompiler bc = writer.bc;

      int control_id = writer.next_control_id();

      lhs.require_Logical().compile( writer );
      writer.write( BardOp.push_true_and_jump_if_true );
      writer.write_address( control_id, "end" );

      rhs.require_Logical().compile( writer );
      writer.define_address( control_id, "end" );
    }
  }

  static public class Is extends BardCmdCmp
  {
    public BardCmd duplicate() { return new Is().init( lhs.duplicate(), rhs.duplicate() ).set_source_info( this ); }
    public String name()        { return "is"; }
    public int opcode_Variant() { return BardOp.is_Variant; }
    public int opcode_Object()  { return BardOp.is_Object; }
    //public int opcode_String()  { return BardOp.eq_String; }
    public int opcode_Real()  { return BardOp.eq_Real; }
    public int opcode_Integer()   { return BardOp.eq_Integer; }
    public int opcode_Character()   { return BardOp.eq_Integer; }
    public int opcode_Logical() { return BardOp.eq_Integer; }
  }

}

