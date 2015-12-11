abstract public class BossCommand
{
  public String keyword;

  public BossCommand( String keyword )
  {
    this.keyword = keyword;
  }

  abstract public BossCode parse( BossParser parser, BossToken t );
}

