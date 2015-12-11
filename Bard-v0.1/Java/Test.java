import java.util.*;

import compiler.*;

public class Test
{
  static public void main( String[] args )
  {
    BardCompiler bc = new BardCompiler();

    if ( !bc.compile( args[0] + ".bard" ) ) System.exit(1);
    if ( !bc.export( args[0] + "Program" ) ) System.exit(1);
    
    /*
    StringBuilder buffer = new StringBuilder();
    for (;;)
    {
      buffer.setLength(0);
      System.out.print( "Bard> " );
      try
      {
        for (int ch=System.in.read(); ch!=10; ch=System.in.read())
        {
          buffer.append( (char) ch );
        }
        if (buffer.length() == 0) break;
        bc.execute( buffer.toString() );
      }
      catch (Exception err)
      {
      }
    }
    */
  }
}
