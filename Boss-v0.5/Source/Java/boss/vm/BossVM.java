package boss.vm;

import java.io.*;
import java.util.*;
import java.nio.*;

public class BossVM
{
  public BossTokens tokens = new BossTokens();

  static public void main( String[] args )
  {
    new BossVM();
  }

  public BossVM()
  {
    System.out.println( tokens.stringIndex("METHODS") );
    BossStringBuilder builder = new BossStringBuilder();
    builder.print("METH").print("ODS");
    System.out.println( tokens.stringIndex(builder) );
    System.out.println( builder.clear().print("Abe").hashCode() );
    System.out.println( builder.clear().print('A').print('b').print('e').hashCode() );
  }

}

