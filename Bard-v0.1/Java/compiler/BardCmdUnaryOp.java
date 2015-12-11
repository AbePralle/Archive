package compiler;

import alphavm.BardOp;

abstract public class BardCmdUnaryOp extends BardCmd
{
  public BardCmd operand;

  public BardCmdUnaryOp init( BardCmd operand )
  {
    this.operand = operand;
    return this;
  }

  public BardType type()
  {
    return operation_type( operand.type() );
  }

  public BardType operation_type( BardType operand_type )
  {
    BardCompiler bc = info.bc;
    if (operand_type == bc.type_Real) return bc.type_Real;
    if (operand_type == bc.type_Character)  return bc.type_Character;
    if (operand_type == bc.type_Integer) return bc.type_Integer;
    return operand_type;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    operand = operand.resolve( method_context );
    BardType operation_type = operation_type( operand.type() );
    operand = operand.cast_to( operation_type );
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    BardCompiler bc = writer.bc;
    BardType operation_type = operation_type( operand.type() );

    operand.compile(writer);

    int opcode = -1;
    //if (operation_type == bc.type_String)          opcode = opcode_String();
    if (operation_type == bc.type_Real)    opcode = opcode_Real();
    else if (operation_type == bc.type_Integer) opcode = opcode_Integer();
    else if (operation_type == bc.type_Logical) opcode = opcode_Logical();
    else                                        opcode = opcode_Object();

    if (opcode == -1)
    {
      throw error( "Illegal operand of type " + operation_type.name + " for '" + name() + "' operation." );
    }
    else
    {
      writer.write( opcode );
    }
  }

  static public class Negate extends BardCmdUnaryOp
  {
    public BardCmd duplicate() { return new Negate().init(operand.duplicate()).set_source_info(this); }
    public String name()        { return "-"; }
    public int opcode_Real()  { return BardOp.negate_Real; }
    public int opcode_Integer()   { return BardOp.negate_Integer; }
    public int opcode_Logical() { return BardOp.logical_not; }
  }

  static public class Not extends BardCmdUnaryOp
  {
    public BardCmd duplicate() { return new Not().init(operand.duplicate()).set_source_info(this); }
    public String name()        { return "not"; }
    public int opcode_Logical() { return BardOp.logical_not; }
  }

  static public class Logicalize extends BardCmdUnaryOp
  {
    public BardCmd duplicate() { return new Logicalize().init(operand.duplicate()).set_source_info(this); }
    public BardType type()      { return info.bc.type_Logical; } 

    public String name()        { return "?"; }
    public int opcode_Object()  { return BardOp.logicalize_Object; }
    public int opcode_Real()  { return BardOp.logicalize_Real; }
    public int opcode_Integer()   { return BardOp.logicalize_Integer; }

    public BardCmd resolve( BardMethod method_context )
    {
      operand = operand.resolve( method_context );
      if (operand.type() == info.bc.type_Logical) return operand;
      return this;
    }
  }
}
