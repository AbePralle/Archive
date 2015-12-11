package compiler;

import alphavm.BardOp;

public class BardCmdList
{
  static public class Clear extends BardCmd
  {
    public BardCmd context;

    public Clear( BardCmd context )
    {
      this.context = context;
    }

    public BardCmd duplicate() { return new Clear( context.duplicate() ).set_source_info( this ); }

    public BardType type() { return info.bc.type_List; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context );
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      writer.write( BardOp.List_clear );
    }
  }

  static public class Count extends BardCmd
  {
    public BardCmd context;

    public Count( BardCmd context )
    {
      this.context = context;
    }

    public BardCmd duplicate() { return new Count( context.duplicate() ).set_source_info( this ); }

    public BardType type() { return info.bc.type_Integer; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context );
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      writer.write( BardOp.List_count );
    }
  }

  static public class Get extends BardCmd
  {
    public BardCmd  context, index;

    public Get( BardCmd context, BardCmd index )
    {
      this.context = context;
      this.index   = index;
    }

    public BardCmd duplicate() { return new Get( context.duplicate(), index.duplicate() ).set_source_info( this ); }

    public BardType type() 
    { 
      return info.bc.type_Variant; 
    }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context );
      index = index.resolve( method_context ).cast_to(info.bc.type_Integer);
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      index.compile( writer );
      writer.write( BardOp.List_get );
    }
  }

  static public class Set extends BardCmd
  {
    public BardCmd context, index, new_value;

    public Set( BardCmd context, BardCmd index, BardCmd new_value )
    {
      this.context   = context;
      this.index     = index;
      this.new_value = new_value;
    }

    public BardCmd duplicate() { return new Set( context.duplicate(), index.duplicate(), new_value.duplicate() ).set_source_info( this ); }

    public BardType type() { return null; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context );
      index = index.resolve( method_context ).require_value().cast_to( info.bc.type_Integer );
      new_value = new_value.resolve( method_context ).require_value().cast_to( info.bc.type_Variant );
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      index.compile( writer );
      new_value.compile( writer );
      writer.write( BardOp.List_set );
    }
  }

  static public class Add extends BardCmd
  {
    public BardCmd  context;
    public BardArgs new_values;

    public Add( BardCmd context, BardArgs new_values )
    {
      this.context    = context;
      this.new_values = new_values;
    }

    public BardCmd duplicate() { return new Add( context.duplicate(), new_values.duplicate() ).set_source_info( this ); }

    public BardType type() { return info.bc.type_List; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context );
      for (int i=0; i<new_values.size(); ++i)
      {
        new_values.set( i, new_values.get(i).resolve( method_context ).cast_to( info.bc.type_Variant ) );
      }
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      for (BardCmd new_value : new_values)
      {
        new_value.compile( writer );
        writer.write( BardOp.List_add );
      }
    }
  }

  static public class Insert extends BardCmd
  {
    public BardCmd  context;
    public BardCmd  new_value;
    public BardCmd  before_index;

    public Insert( BardCmd context, BardCmd new_value, BardCmd before_index )
    {
      this.context      = context;
      this.new_value    = new_value;
      this.before_index = before_index;
    }

    public BardCmd duplicate() { return new Insert( context.duplicate(), new_value.duplicate(), before_index.duplicate() ).set_source_info( this ); }

    public BardType type() { return info.bc.type_List; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context ).require_value();
      new_value = new_value.resolve( method_context ).cast_to( info.bc.type_Variant );
      before_index = before_index.resolve( method_context ).cast_to( info.bc.type_Integer );
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      new_value.compile( writer );
      before_index.compile( writer );
      writer.write( BardOp.List_insert );
    }
  }

  static public class RemoveAt extends BardCmd
  {
    public BardCmd context, index;

    public RemoveAt( BardCmd context, BardCmd index )
    {
      this.context = context;
      this.index   = index;
    }

    public BardCmd duplicate() { return new RemoveAt( context.duplicate(), index.duplicate() ).set_source_info( this ); }

    public BardType type() { return info.bc.type_Variant; }

    public BardCmd resolve( BardMethod method_context )
    {
      context = context.resolve( method_context );
      index = index.resolve( method_context ).cast_to(info.bc.type_Integer);
      return this;
    }

    public void compile( BardCodeWriter writer )
    {
      context.compile( writer );
      index.compile( writer );
      writer.write( BardOp.List_remove_at );
    }
  }

}
