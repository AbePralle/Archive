package compiler;

import alphavm.BardOp;

public class BardCmdString
{
  /*
  static public class Count extends BardCmdUnaryOp
  {
    public BardCmd duplicate() { return new Count().init( operand.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_Integer; }

    public void compile( BardCodeWriter writer )
    {
      operand.compile( writer );
      writer.write( BardOp.String_count );
    }
  }

  static public class Get extends BardCmdUnaryOp
  {
    public BardCmd index;

    public Get( BardCmd operand, BardCmd index )
    {
      init( operand );
      this.index = index;
    }

    public BardCmd duplicate() { return new Get( operand.duplicate(), index.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_Character; }

    public BardCmd resolve( BardMethod method_context )
    {
      operand = operand.resolve(method_context).require_value();
      index = index.resolve(method_context).require_value().cast_to( info.bc.type_Integer );
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      operand.compile( writer );
      index.compile( writer );
      writer.write( BardOp.String_get );
    }
  }
  */
}
