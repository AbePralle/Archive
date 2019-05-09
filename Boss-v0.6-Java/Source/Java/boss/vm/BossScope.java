package boss.vm;

public class BossScope
{
  public BossVM     vm;
  public BossScope  previousScope;
  public BossModule thisModule;

  public BossScope( BossModule thisModule )
  {
    this.vm         = thisModule.vm;
    this.thisModule = thisModule;
  }

  public BossScope push( BossScope newScope )
  {
    newScope.previousScope = this;
    return newScope;
  }

  public BossScope pop()
  {
    return previousScope;
  }
}

