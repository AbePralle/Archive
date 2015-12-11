package compiler;

import alphavm.BardOp;

public class BardCmdForEach extends BardCmdStatement
{
  public String            element_name;
  public BardCmd           collection;
  public BardStatementList body = new BardStatementList();
  public int               unique_id;

  public BardCmdForEach init( String element_name, BardCmd collection )
  {
    this.element_name = element_name;
    this.collection = collection;
    return this;
  }

  public BardCmd duplicate() 
  { 
    BardCmdForEach result = new BardCmdForEach().init( element_name, collection.duplicate() );
    result.set_source_info(this);
    result.body = body.duplicate();
    result.unique_id = unique_id;
    return result;
  }

  static public class ForEachIn extends BardCmdForEach
  {
    public BardCmd duplicate() 
    { 
      BardCmdForEach result = new ForEachIn().init( element_name, collection.duplicate() );
      result.set_source_info(this);
      result.body = body.duplicate();
      result.unique_id = unique_id;
      return result;
    }

    public BardCmd resolve( BardMethod method_context )
    {
      BardCompiler bc = info.bc;
      BardCmdBlock outer_scope = new BardCmdBlock();
      outer_scope.set_source_info( this );

      unique_id = method_context.locals.size();

      String collection_name = "collection:" + unique_id;
      String counter_name = "counter:" + unique_id;

      BardLocal v_collection = new BardLocal( collection_name );
      v_collection.initial_value = collection;

      BardLocal v_counter    = new BardLocal( counter_name, bc.type_Integer );

      outer_scope.body.add( new BardCmdLocalDeclaration(v_collection).set_source_info(collection) );
      outer_scope.body.add( new BardCmdWriteLocal( v_collection, collection ).set_source_info(collection) );
      outer_scope.body.add( new BardCmdLocalDeclaration(v_counter).set_source_info(this) );
      outer_scope.body.add( new BardCmdWriteLocal( v_counter, new BardCmdLiteral.LiteralInteger(0).set_source_info(this) ).set_source_info(collection) );

      BardCmd lhs = new BardCmdReadLocal(v_counter).set_source_info( this );
      BardCmd rhs = new BardCmdReadLocal(v_collection).set_source_info( this );
      rhs = new BardCmdContextAccess( rhs, "count", null ).set_source_info( rhs );
      BardCmd comparison = new BardCmdCmp.LT().init( lhs, rhs ).set_source_info( lhs );

      BardCmdWhile cmd_while = new BardCmdWhile().init( comparison );
      cmd_while.set_source_info( this );

      BardLocal v_element = new BardLocal( element_name );
      BardCmd get_element =
            new BardCmdContextAccess(
              new BardCmdReadLocal(v_collection).set_source_info(this), "get",
                new BardArgs(
                  new BardCmdReadLocal(v_counter).set_source_info(this)
                )
              ).set_source_info(this);
      v_element.initial_value = get_element;  // for type purposes

      cmd_while.body.add( new BardCmdLocalDeclaration(v_element).set_source_info(this) );

      cmd_while.body.add( new BardCmdWriteLocal( v_element, get_element).set_source_info(this) );

      cmd_while.body.add( new BardCmdAdjust.IncrementLocal().init(v_counter).set_source_info(this) );

      cmd_while.body.add( new BardCmdBlock(body).set_source_info(this) );

      outer_scope.body.add( cmd_while );


      BardCmd result = outer_scope.resolve( method_context );
      return result;
    }
  }

  static public class ForEachOf extends BardCmdForEach
  {
    public BardCmd duplicate() 
    { 
      BardCmdForEach result = new ForEachOf().init( element_name, collection.duplicate() );
      result.set_source_info(this);
      result.body = body.duplicate();
      result.unique_id = unique_id;
      return result;
    }

    public BardCmd resolve( BardMethod method_context )
    {
      BardCompiler bc = info.bc;
      BardCmdBlock outer_scope = new BardCmdBlock();
      outer_scope.set_source_info( this );

      unique_id = method_context.locals.size();

      String collection_name = "collection:" + unique_id;

      BardLocal v_collection = new BardLocal( collection_name );
      v_collection.initial_value = collection;

      BardLocal v_counter    = new BardLocal( element_name, bc.type_Integer );

      outer_scope.body.add( new BardCmdLocalDeclaration(v_collection).set_source_info(collection) );
      outer_scope.body.add( new BardCmdWriteLocal( v_collection, collection ).set_source_info(collection) );
      outer_scope.body.add( new BardCmdLocalDeclaration(v_counter).set_source_info(this) );
      outer_scope.body.add( new BardCmdWriteLocal( v_counter, new BardCmdLiteral.LiteralInteger(0).set_source_info(this) ).set_source_info(collection) );

      BardCmd lhs = new BardCmdReadLocal(v_counter).set_source_info( this );
      BardCmd rhs = new BardCmdReadLocal(v_collection).set_source_info( this );
      rhs = new BardCmdContextAccess( rhs, "count", null ).set_source_info( rhs );
      BardCmd comparison = new BardCmdCmp.LT().init( lhs, rhs ).set_source_info( lhs );

      BardCmdWhile cmd_while = new BardCmdWhile().init( comparison );
      cmd_while.set_source_info( this );

      cmd_while.body.add( new BardCmdBlock(body).set_source_info(this) );

      cmd_while.body.add( new BardCmdAdjust.IncrementLocal().init(v_counter).set_source_info(this) );

      outer_scope.body.add( cmd_while );


      BardCmd result = outer_scope.resolve( method_context );
      return result;
    }
  }
}

