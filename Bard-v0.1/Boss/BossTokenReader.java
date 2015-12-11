import java.util.*;

class BossTokenReader
{
  ArrayList<BossToken> tokens;
  int position;
  int count;

  BossTokenReader( ArrayList<BossToken> tokens )
  {
    this.tokens = tokens;
    count = tokens.size();
  }

  boolean has_another() { return position < count; }

  BossToken peek()
  {
    if (position >= count) return null;
    return tokens.get( position );
  }

  BossToken peek( int num_ahead )
  {
    int peek_pos = position + num_ahead;
    if (peek_pos >= count) return null;
    return tokens.get( peek_pos );
  }

  BossToken read()
  {
    return tokens.get( position++ );
  }
}
