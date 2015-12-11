package compiler;

import alphavm.BardOp;

abstract public class BardCmdLiteral extends BardCmd
{
  public boolean is_literal() { return true; }

  public BardCmd resolve( BardMethod m ) { return this; }

  static public class LiteralNull extends BardCmdLiteral
  {
    public BardType of_type;

    public BardCmd duplicate() 
    { 
      LiteralNull result = new LiteralNull();
      result.set_source_info( this );
      result.of_type = of_type;
      return result;
    }

    public BardType type() 
    { 
      if (of_type == null) of_type = info.bc.type_Object;
      return of_type;
    }

    public BardCmd cast_to( BardType to_type )
    {
      if (to_type.instance_of(info.bc.type_Object))
      {
        of_type = to_type;
        return this;
      }
      else
      {
        return super.cast_to( to_type );
      }
    }

    public void compile( BardCodeWriter writer )
    {
      writer.write( BardOp.push_literal_Object_null );
    }
  }

  static public class LiteralString extends BardCmdLiteral
  {
    public String value;

    public LiteralString( String value ) { this.value = value; }

    public BardCmd duplicate() { return new LiteralString( value ).set_source_info(this); }

    public BardType type() { return info.bc.type_String; }

    public void compile( BardCodeWriter writer )
    {
      writer.write( BardOp.push_literal_String, writer.index_of_String(value) );
    }
  }

  static public class LiteralReal extends BardCmdLiteral
  {
    public double value;

    public LiteralReal( double value ) { this.value = value; }

    public BardCmd duplicate() { return new LiteralReal( value ).set_source_info(this); }

    public BardType type() { return info.bc.type_Real; }

    public void compile( BardCodeWriter writer )
    {
      writer.write( BardOp.push_literal_Real, writer.index_of_Real(value) );
    }
  }

  static public class LiteralInteger extends BardCmdLiteral
  {
    public int value;

    public LiteralInteger( int value ) { this.value = value; }

    public BardCmd duplicate() { return new LiteralInteger( value ).set_source_info(this); }

    public BardType type() { return info.bc.type_Integer; }

    public String toString() { return ""+value; }

    public void compile( BardCodeWriter writer ) { writer.write( BardOp.push_literal_Integer, value ); }
  }

  static public class LiteralCharacter extends BardCmdLiteral
  {
    public int value;

    public LiteralCharacter( int value ) { this.value = value; }

    public BardCmd duplicate() { return new LiteralCharacter( value ).set_source_info(this); }

    public BardType type() { return info.bc.type_Character; }
    public void compile( BardCodeWriter writer ) { writer.write( BardOp.push_literal_Integer, value ); }
  }

  static public class LiteralLogical extends BardCmdLiteral
  {
    public boolean value;

    public LiteralLogical( boolean value ) { this.value = value; }

    public BardCmd duplicate() { return new LiteralLogical( value ).set_source_info(this); }

    public BardType type() { return info.bc.type_Logical; }

    public void compile( BardCodeWriter writer )
    {
      if (value) writer.write( BardOp.push_literal_Integer_1 );
      else       writer.write( BardOp.push_literal_Integer_0 );
    }
  }

  static public class LiteralList extends BardCmdLiteral
  {
    public BardArgs args;

    public LiteralList( BardArgs args ) { this.args = args; }

    public BardCmd duplicate() { return new LiteralList( args.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_List; }

    public BardCmd resolve( BardMethod method_context )
    {
      args.resolve( method_context );
      for (int i=0; i<args.size(); ++i)
      {
        args.set( i, args.get(i).cast_to(info.bc.type_Variant) );
      }
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      writer.write( BardOp.List_create, args.size() );
      for (BardCmd arg : args)
      {
        arg.compile( writer );
        writer.write( BardOp.List_add );
      }
    }
  }

  static public class LiteralTable extends BardCmdLiteral
  {
    public BardArgs keys;
    public BardArgs values;

    public LiteralTable( BardArgs keys, BardArgs values ) 
    {
      this.keys = keys; 
      this.values = values;
    }

    public BardCmd duplicate() { return new LiteralTable( keys.duplicate(), values.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_Table; }

    public BardCmd resolve( BardMethod method_context )
    {
      keys.resolve( method_context );
      for (int i=0; i<keys.size(); ++i)
      {
        keys.set( i, keys.get(i).cast_to(info.bc.type_Variant) );
      }

      values.resolve( method_context );
      for (int i=0; i<values.size(); ++i)
      {
        values.set( i, values.get(i).cast_to(info.bc.type_Variant) );
      }
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      writer.write( BardOp.Table_create );
      for (int i=0; i<keys.size(); ++i)
      {
        keys.get(i).compile( writer );
        values.get(i).compile( writer );
        writer.write( BardOp.Table_set );
      }
    }
  }
}
