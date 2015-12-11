package compiler;

import alphavm.BardOp;

import java.util.*;

public class BardModule
{
  public BardCompiler            bc;
  public String            filepath;

  public BardStatementList global_statements = new BardStatementList();

  public BardTypeList      defined_type_list = new BardTypeList();

  public BardModule( BardCompiler bc, String filepath )
  {
    this.bc = bc;
    this.filepath = filepath;
  }

  public void invalidate()
  {
  }

  public BardType define_type( String name, int qualifiers )
  {
    BardType type = bc.define_type( name, qualifiers );
    defined_type_list.add( type );
    return type;
  }
}

