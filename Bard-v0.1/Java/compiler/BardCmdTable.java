package compiler;

import alphavm.BardOp;

public class BardCmdTable
{
  static public class Clear extends BardCmd
  {
    public BardCmd context;

    public Clear( BardCmd context )
    {
      this.context = context;
    }

    public BardCmd duplicate() { return new Clear( context.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_Table; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context ).require_value();
      return this;
    }

    //public void compile( BardCodeWriter writer )
    //{
    //  context.compile( writer );
    //  writer.write( BardOp.List_clear );
    //}
  }

  static public class Count extends BardCmd
  {
    public BardCmd context;

    public Count( BardCmd context )
    {
      this.context = context;
    }

    public BardCmd duplicate() { return new Count( context.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_Integer; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context ).require_value();
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      writer.write( BardOp.Table_count );
    }
  }

  static public class GetKey extends BardCmd
  {
    public BardCmd context, key;

    public GetKey( BardCmd context, BardCmd key )
    {
      this.context = context;
      this.key   = key;
    }

    public BardCmd duplicate() { return new GetKey( context.duplicate(), key.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_Variant; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context ).require_value();
      key = key.resolve( method_context ).require_value().cast_to(info.bc.type_Variant);
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      key.compile( writer );
      writer.write( BardOp.Table_get_key );
    }
  }

  static public class SetIndex extends BardCmd
  {
    public BardCmd context, index, new_value;

    public SetIndex( BardCmd context, BardCmd index, BardCmd new_value )
    {
      this.context   = context;
      this.index     = index;
      this.new_value = new_value;
    }

    public BardCmd duplicate() { return new SetIndex( context.duplicate(), index.duplicate(), new_value.duplicate() ).set_source_info(this); }

    public BardType type() { return null; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context ).require_value();
      index = index.resolve( method_context ).require_value().cast_to( info.bc.type_Integer );
      new_value = new_value.resolve( method_context ).require_value().cast_to( info.bc.type_Variant );
      return this;
    }

    //public void compile( BardCodeWriter writer )
    //{
    //  context.compile( writer );
    //  index.compile( writer );
    //  new_value.compile( writer );
    //  writer.write( BardOp.List_set );
    //}
  }

  static public class SetKey extends BardCmd
  {
    public BardCmd context, key, value;

    public SetKey( BardCmd context, BardCmd key, BardCmd value )
    {
      this.context = context;
      this.key   = key;
      this.value = value;
    }

    public BardCmd duplicate() { return new SetKey( context.duplicate(), key.duplicate(), value.duplicate() ).set_source_info(this); }

    public BardType type() { return info.bc.type_Table; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context ).require_value();
      key = key.resolve( method_context ).require_value().cast_to(info.bc.type_Variant);
      value = value.resolve( method_context ).require_value().cast_to(info.bc.type_Variant);
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      key.compile( writer );
      value.compile( writer );
      writer.write( BardOp.Table_set );
    }
  }
}
