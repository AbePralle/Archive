package boss.vm;

import java.io.*;
import java.util.*;

public class BossModule
{
  public BossVM                   vm;
  public String                   name;
  public BossInt32List            code = new BossInt32List();
  public int                      globalStatements;
  public int                      globalStatementsCodeIndex;
  public ArrayList<BossType>      typeList   = new ArrayList<BossType>();
  public HashMap<String,BossType> typeLookup = new HashMap<String,BossType>();

  public boolean isResolved;
  public boolean isLaunched;

  public BossModule( BossVM vm, String name )
  {
    this.vm = vm;
    this.name = name;
  }

  public BossType defineType( BossSourceInfo t, String name )
  {
    BossType type = new BossType( this, t, name );
    typeList.add( type );
    typeLookup.put( name, type );
    type.isDefined = true;
    return type;
  }

  public void generateCode()
  {
    for (int i=0; i<typeList.size(); ++i) typeList.get( i ).generateCode();

    globalStatementsCodeIndex = vm.code.count;
    if (globalStatements > 0)
    {
      vm.cmdData.generateCode( globalStatements, vm );
    }
    vm.code.add( BossOpcode.RETURN_NIL );
  }

  public void launch()
  {
    if (isLaunched) return;
    isLaunched = true;

    vm.execute( globalStatementsCodeIndex );
  }

  /*
  public void parse( File file )
  {
    parse( new BossParser(vm,file) );
  }

  public void parse( String filename, String source )
  {
    parse( new BossParser(vm,filename,source) );
  }

  public void parse( BossParser parser )
  {
    globalStatements = parser.parseMultiLineStatements( 0 );
  }
  */

  public void resolve()
  {
    if (isResolved) return;
    isResolved = true;

    for (int i=0; i<typeList.size(); ++i) typeList.get( i ).resolve();

    if (globalStatements > 0)
    {
      globalStatements = vm.cmdData.resolved( globalStatements, new BossScope(this) );
      System.out.println( vm.cmdData.toString(globalStatements) );
    }
  }
}
