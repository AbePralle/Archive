package compiler;

import alphavm.BardOp;

public class BardCmdCast extends BardCmdUnaryOp
{
  public BardType to_type;

  public BardCmdCast( BardCmd operand, BardType to_type )
  {
    init( operand );
    this.to_type = to_type;
  }

  public BardCmd duplicate()
  {
    return new BardCmdCast( operand.duplicate(), to_type ).set_source_info( this );
  }

  public String toString()
  {
    return operand + ".(" + to_type.name + ")";
  }

  public BardType type() { return to_type; }

  public BardCmd resolve( BardMethod method_context )
  {
    operand = operand.resolve( method_context ).require_value();

    BardCompiler bc = info.bc;
    if (to_type == info.bc.type_String)
    {
      if (operand.type() == to_type) return this;

      if (operand.type() == bc.type_Variant)
      {
        operand = new BardCmdCast( operand, bc.type_Object ).set_source_info(operand);
        operand = new BardCmdContextAccess( operand, "String", new BardArgs() ).set_source_info(operand);
        return operand.resolve( method_context );
      }
      else
      {
        BardCmd context = new BardCmdAccess( "Character[]", new BardArgs() ).set_source_info(this);
        BardCmd cmd = new BardCmdContextAccess( context, "print", new BardArgs(operand) ).set_source_info(this);
        return cmd.resolve( method_context );
      }
    }
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    operand.compile( writer );

    BardType from_type = operand.type();
    if (from_type == to_type) return;

    write_cast( from_type, to_type, writer );
  }

  public void write_cast( BardType from_type, BardType to_type, BardCodeWriter writer )
  {
    BardCompiler bc = info.bc;

    if (from_type == to_type) return;
//System.out.println( "-- casting " + from_type + " to " + to_type );
    boolean success = true;
    if (to_type == bc.type_Variant)
    {
      int opcode = -1;
      if (from_type == bc.type_Real)         opcode = BardOp.cast_Real_to_Variant;
      else if (from_type == bc.type_Integer) opcode = BardOp.cast_Integer_to_Variant;
      else if (from_type == bc.type_Character)    opcode = BardOp.cast_Character_to_Variant;
      else if (from_type == bc.type_Logical) opcode = BardOp.cast_Logical_to_Variant;
      else                                   opcode = BardOp.cast_Object_to_Variant;

      // Note: the only time Object needs conversion is with 'null' references
      writer.write( opcode );
    }
    else if (from_type.primitive_type != null && to_type.primitive_type == null)
    {
      // Unboxing before cast
      BardType boxed_type = from_type.primitive_type;

      int opcode = -1;
      if (boxed_type == bc.type_Real)         opcode = BardOp.unbox_Real;
      else if (boxed_type == bc.type_Integer) opcode = BardOp.unbox_Integer;
      else if (boxed_type == bc.type_Character)    opcode = BardOp.unbox_Character;
      else if (boxed_type == bc.type_Logical) opcode = BardOp.unbox_Logical;

      if (opcode != -1)
      {
        writer.write( opcode );
        write_cast( boxed_type, to_type, writer );
      }
      else
      {
        success = false;
      }
    }
    else if (from_type.instance_of(to_type))
    {
      // no cast required
    }
    else if (to_type.is_Object())
    {
      int opcode = -1;
      //if (from_type == bc.type_String)          opcode = BardOp.box_String;
      if (from_type == bc.type_Real)         opcode = BardOp.box_Real;
      else if (from_type == bc.type_Integer) opcode = BardOp.box_Integer;
      else if (from_type == bc.type_Character)    opcode = BardOp.box_Character;
      else if (from_type == bc.type_Logical) opcode = BardOp.box_Logical;
      else if (from_type == bc.type_Variant) opcode = BardOp.cast_Variant_to_Object;
      else success = false;

      if (success) writer.write( opcode );
    }
    /*
    else if (to_type == bc.type_String)
    {
      int opcode = -1;
      if (from_type == bc.type_Real)            opcode = BardOp.cast_Real_to_String;
      else if (from_type == bc.type_Integer)    opcode = BardOp.cast_Integer_to_String;
      else if (from_type == bc.type_Character)       opcode = BardOp.cast_Character_to_String;
      else if (from_type == bc.type_Logical)    opcode = BardOp.cast_Logical_to_String;
      else
      {
        BardMethod m = bc.type_Object.find_method( "String()" );
        if (m == null) throw new BardError( "No such method Object::String()." );
        writer.write( BardOp.cast_Object_to_String_null_check );
        writer.write( BardOp.dynamic_call );
        writer.write( 0, m.method_id );
      }

      if (opcode != -1) writer.write( opcode );
    }
    */
    else if (to_type == bc.type_Real)
    {
      int opcode = -1;
      //if (from_type == bc.type_String)               opcode = BardOp.cast_String_to_Real;
      if (from_type == bc.type_Integer)      opcode = BardOp.cast_Integer_to_Real;
      else if (from_type == bc.type_Character)
      {
        writer.write( BardOp.cast_Character_to_Integer );
        writer.write( BardOp.cast_Integer_to_Real );
      }
      else if (from_type == bc.type_Logical)
      {
        opcode = BardOp.cast_Integer_to_Real;
      }
      else if (from_type.is_Object()) writer.write_Object_call( "Real()" );
      else if (from_type == bc.type_Variant)    opcode = BardOp.cast_Variant_to_Real;
      else success = false;

      if (opcode != -1) writer.write( opcode );
    }
    else if (to_type == bc.type_Integer)
    {
      int opcode = -1;

      //if (from_type == bc.type_String)           opcode = BardOp.cast_String_to_Integer;
      if (from_type == bc.type_Real)     opcode = BardOp.cast_Real_to_Integer;
      else if (from_type == bc.type_Character )    opcode = BardOp.cast_Character_to_Integer;
      else if (from_type == bc.type_Logical)  return;
      else if (from_type.is_Object()) writer.write_Object_call( "Integer()" );
      else if (from_type == bc.type_Variant)                               opcode = BardOp.cast_Variant_to_Integer;
      else success = false;

      if (opcode != -1) writer.write( opcode );
    }
    else if (to_type == bc.type_Character)
    {
      int opcode = -1;
      if (from_type.is_Variant())  opcode = BardOp.cast_Variant_to_Character;
      else if (from_type != bc.type_Integer) success = false;

      if (opcode != -1) writer.write( opcode );
    }
    else if (to_type == bc.type_Logical)
    {
      if (from_type.is_Object()) writer.write_Object_call( "Logical()" );
      else success = false;
    }
    else if (from_type == bc.type_Variant && to_type.instance_of(bc.type_Object))
    {
      writer.write( BardOp.cast_Variant_to_Object );
      writer.write( BardOp.type_check, to_type.index );
    }
    else if (to_type.instance_of(bc.type_Object) && from_type.instance_of(bc.type_Object))
    {
      writer.write( BardOp.type_check, to_type.index );
    }
    else
    {
      success = false;
    }

    if ( !success ) throw error( "Unsupported cast: " + from_type.name + " -> " + to_type.name + " in " + this );
  }

  static public class As extends BardCmdCast
  {
    public As( BardCmd operand )
    {
      super( operand, null );
    }

    public As( BardCmd operand, BardType to_type )
    {
      super( operand, to_type );
    }

    public BardCmd duplicate()
    {
      return new BardCmdCast.As( operand.duplicate(), to_type ).set_source_info( this );
    }

    public void write_cast( BardType from_type, BardType to_type, BardCodeWriter writer )
    {
      BardCompiler bc = info.bc;

      boolean success = true;

      if (from_type == bc.type_Character)
      {
        success = (to_type == bc.type_Integer);
      }
      else if (from_type == bc.type_Integer)
      {
        success = (to_type == bc.type_Character);
      }
      else if (from_type == bc.type_Variant || (from_type.instance_of(bc.type_Object) && to_type.instance_of(bc.type_Object)))
      {
        super.write_cast( from_type, to_type, writer );
        return;
      }

      if ( !success ) throw error( "Unsupported cast: " + from_type.name + " -> " + to_type.name + " in " + this );
    }
  }
}

