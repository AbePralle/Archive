package boss.vm;

import java.util.*;

public class BossSymbolLookup
{
  public String[] symbols;
  public int[]    tokenTypes;

  public int[]    firstSymbol = new int[ 128 ];
  public int[]    symbolCount = new int[ 128 ];

  public BossSymbolLookup( BossTokenData tokens )
  {
    ArrayList<String> list = new ArrayList<String>( 64 );

    for (int tokenType=0; tokenType<=BossTokenType.LAST_TOKEN_TYPE; ++tokenType)
    {
      if (tokens.typeIsSymbol(tokenType))
      {
        // Add symbol to list using the following ordering:
        //   If first characters match, group by longest first.
        //   Otherwise sort by standard string ordering.
        insertSymbol( tokens.typeName(tokenType), list );
      }
    }

    symbols = new String[ list.size() ];
    tokenTypes = new int[ list.size() ];
    for (int i=0; i<list.size(); ++i) symbols[i] = list.get(i);

    for (int tokenType=0; tokenType<=BossTokenType.LAST_TOKEN_TYPE; ++tokenType)
    {
      if (tokens.typeIsSymbol(tokenType))
      {
        tokenTypes[ list.indexOf(tokens.typeName(tokenType)) ] = tokenType;
      }
    }

    for (int i=0; i<firstSymbol.length; ++i) firstSymbol[i] = -1;

    for (int i=0; i<list.size(); ++i)
    {
      String symbol = list.get( i );
      int ch = symbol.charAt( 0 );
      if (firstSymbol[ch] == -1) firstSymbol[ch] = i;
      ++symbolCount[ ch ];
    }
  }

  public int compareSymbols( String a, String b )
  {
    if (a.charAt(0) == b.charAt(0))
    {
      int a_count = a.length();
      int b_count = b.length();
      if (a_count == b_count) return a.compareTo( b );
      if (a_count > b_count)  return -1;
      else                    return  1;
    }
    else
    {
      return a.compareTo( b );
    }
  }

  public void insertSymbol( String symbol, ArrayList<String> list )
  {
    for (int i=list.size(); --i>=0; )
    {
      if (compareSymbols(symbol,list.get(i)) >= 0)
      {
        list.add( i+1, symbol );
        return;
      }
    }
    list.add( 0, symbol );
  }
}

