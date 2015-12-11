package compiler;

import java.util.*;
import alphavm.BardOp;

public class BardCmdIf extends BardCmdStatement
{
  static public boolean tron = false;

  public BardCmd           condition;
  public BardStatementList body = new BardStatementList();
  public ArrayList<ElseIf> else_ifs = new ArrayList<ElseIf>();
  public BardStatementList else_body;

  public BardCmdIf init( BardCmd condition )
  {
    this.condition = condition;
    return this;
  }

  public BardCmd duplicate() 
  { 
    BardCmdIf result = new BardCmdIf().init( condition.duplicate() );
    result.set_source_info(this);
    result.body = body.duplicate();

    for (int i=0; i<else_ifs.size(); ++i)
    {
      result.else_ifs.add( else_ifs.get(i) );
    }

    if (else_body != null) result.else_body = else_body.duplicate();

    return result;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    condition = condition.resolve( method_context );
    condition.require_Logical();
    body.resolve( method_context );

    for (int i=0; i<else_ifs.size(); ++i)
    {
      else_ifs.get(i).resolve( method_context );
    }

    if (else_body != null) else_body.resolve( method_context );

    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    int control_id = writer.next_control_id();
//if (tron) System.out.println( "Writing if:" + control_id );
    int next_else = 1;

    condition.compile( writer );
    writer.write( BardOp.jump_if_false );
    writer.write_address( control_id, "else_1" );

    body.compile( writer );
    writer.write( BardOp.jump );
    writer.write_address( control_id, "end" );

    for (int i=0; i<else_ifs.size(); ++i)
    {
//if (tron) System.out.println( "Writing else_if:" + control_id + " with next_else:" + next_else );
      else_ifs.get(i).compile( writer, control_id, next_else );
      ++next_else;
    }

//if (tron) System.out.println( "Writing else:" + control_id + " with next_else:" + next_else );
    writer.define_address( control_id, "else_" + next_else );
    if (else_body != null)
    {
      else_body.compile( writer );
    }

//if (tron) System.out.println( "endIf:" + control_id );
    writer.define_address( control_id, "end" );
  }

  static public class ElseIf extends BardCmd
  {
    public BardCmd condition;
    public BardStatementList body = new BardStatementList();

    public ElseIf( BardCmd condition )
    {
      this.condition = condition;
    }

    public ElseIf duplicate()
    {
      
      ElseIf result = new ElseIf( condition.duplicate() );
      result.set_source_info( this );
      result.body = body.duplicate();
      return result;
    }

    public BardCmd resolve( BardMethod method_context )
    {
      condition = condition.resolve( method_context );
      body.resolve( method_context );
      return this;
    }

    public void compile( BardCodeWriter writer, int control_id, int next_else_id )
    {
      writer.define_address( control_id, "else_" + next_else_id );
      condition.compile( writer );

      writer.write( BardOp.jump_if_false );
      writer.write_address( control_id, "else_" + (next_else_id+1) );

      body.compile( writer );
      writer.write( BardOp.jump );
      writer.write_address( control_id, "end" );
    }
  }
}

