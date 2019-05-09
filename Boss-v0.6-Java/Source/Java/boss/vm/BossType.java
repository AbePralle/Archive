package boss.vm;

import java.util.*;

public class BossType
{
  final static public int
    NIL       = 0,
    REAL64    = 1,
    REAL32    = 2,
    INT32     = 3,
    CHARACTER = 4,
    BYTE      = 5,
    LOGICAL   = 6,
    OBJECT    = 7,
    STRING    = 8;

  public BossModule     moduleContext;
  public BossSourceInfo t;
  public String         name;
  public int            attributes;
  public int            genericType;
  public boolean        isDefined;
  public boolean        isResolved;

  public BossType( BossModule moduleContext, BossSourceInfo t, String name )
  {
    this.moduleContext = moduleContext;
    if (t == null) t = new BossSourceInfo( "[BossVM]", 0, 0 );
    this.t = t;
    this.name = name;
  }

  public void generateCode()
  {
  }

  public void resolve()
  {
    if (isResolved) return;
    isResolved = true;
  }
}

