package compiler;

import alphavm.BardOp;

public class BardCmdLocalDeclaration extends BardCmdStatement
{
  public BardLocal local_info;
  public boolean   resolved;

  public BardCmdLocalDeclaration( BardLocal local_info )
  {
    this.local_info = local_info;
  }

  public BardCmd duplicate() 
  {
    BardCmdLocalDeclaration result = new BardCmdLocalDeclaration( local_info );
    result.resolved = resolved;
    return result;
  }

  public BardCmd resolve( BardMethod method_context )
  {
    if (resolved) return this;
    resolved = true;

    if (local_info.initial_value != null)
    {
      local_info.initial_value = local_info.initial_value.resolve( method_context );
      if (local_info.type == null) local_info.type = local_info.initial_value.type();
    }

    if (local_info.type == null)
    {
      local_info.type = info.bc.type_Variant;
    }

    method_context.add_local( local_info );
    return this;
  }

  public void compile( BardCodeWriter writer )
  {
    // no action
  }
}

