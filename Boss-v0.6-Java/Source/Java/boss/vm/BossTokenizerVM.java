package boss.vm;

import java.io.*;

public class BossTokenizerVM
{
  final static public int
    OPCODE_HALT                =  0,
    OPCODE_RESTART             =  1,
    OPCODE_MODE_INT32          =  2,
    OPCODE_ERROR               =  3,
    OPCODE_MARK_SOURCE_POS     =  4,
    OPCODE_ACCEPT_INT32        =  5,
    OPCODE_JUMP                =  6,
    OPCODE_JUMP_EQ             =  7,
    OPCODE_JUMP_NE             =  8,
    OPCODE_JUMP_LT             =  9,
    OPCODE_JUMP_LE             = 10,
    OPCODE_JUMP_GT             = 11,
    OPCODE_JUMP_GE             = 12,
    OPCODE_CALL                = 13,
    OPCODE_RETURN              = 14,
    OPCODE_HAS_ANOTHER         = 15,
    OPCODE_PEEK_CH_INT32       = 16,
    OPCODE_PEEK_CH_COUNT       = 17,
    OPCODE_READ_CH             = 18,
    OPCODE_CONSUME_CHARACTER   = 19,
    OPCODE_SCAN_DIGITS         = 20,
    OPCODE_SCAN_IDENTIFIER     = 21,
    OPCODE_CLEAR_BUFFER        = 22,
    OPCODE_COLLECT_CH          = 23,
    OPCODE_COLLECT_CHARACTER   = 24,
    OPCODE_COLLECT_STRING      = 25,
    OPCODE_PRINT_BUFFER        = 26,
    OPCODE_PRINT_CH            = 27,
    OPCODE_PRINT_COUNT         = 28,
    OPCODE_PRINT_CHARACTER     = 29,
    OPCODE_PRINT_STRING        = 30,
    OPCODE_COMPARE_CH_INT32    = 31,
    OPCODE_COMPARE_COUNT_INT32 = 32,
    OPCODE_CH_IS_DIGIT_INT32   = 33,
    OPCODE_CH_IS_DIGIT_COUNT   = 34,
    OPCODE_CH_IS_LETTER        = 35,
    OPCODE_TEST_CH             = 36,
    OPCODE_TEST_COUNT          = 37,
    OPCODE_PUSH_CH             = 38,
    OPCODE_PUSH_COUNT          = 39,
    OPCODE_POP_CH              = 40,
    OPCODE_POP_COUNT           = 41,
    OPCODE_SET_CH_TO_INT32     = 42,
    OPCODE_SET_CH_TO_COUNT     = 43,
    OPCODE_SET_COUNT_TO_INT32  = 44,
    OPCODE_SET_COUNT_TO_CH     = 45,
    OPCODE_ADD_CH_INT32        = 46,
    OPCODE_ADD_COUNT_INT32     = 47,
    OPCODE_SCAN_TABLE          = 48,
    OPCODE_WHICH_BUFFER        = 49;

  public BossVM   vm;
  public String[] strings;
  public String[] entryPointNames;
  public int[]    entryPointIPs;
  public int[]    code;

  public String filepath;
  public String source;
  public int    nextLine, nextColumn;
  public int    spacesPerTab = 4;

  public int    startIP;
  public BossStringBuilder buffer = new BossStringBuilder();

  public BossInt32List stack = new BossInt32List();

  public int[] id_characters = new int[128];

  public BossTokenizerVM( BossVM vm )
  {
    this.vm = vm;
    this.tokenData = vm.tokenData;

    for (int ch='a'; ch<='z'; ++ch) id_characters[ch] = 1;
    for (int ch='A'; ch<='Z'; ++ch) id_characters[ch] = 1;
    for (int ch='0'; ch<='9'; ++ch) id_characters[ch] = 2;
    id_characters['_'] = 1;

    Base64IntXDecoder decoder = new Base64IntXDecoder(
"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"
  );

    int version = decoder.readInt32X();
    //require version >= MIN_VERSION

    // Strings
    int n = decoder.readInt32X();
    strings = new String[n];
    for (int i=0; i<n; ++i)
    {
      strings[i] = decoder.readString();
    }

    // Entry points
    n = decoder.readInt32X();
    entryPointNames = new String[n];
    entryPointIPs   = new int[n];
    for (int i=0; i<n; ++i)
    {
      entryPointNames[i] = strings[ decoder.readInt32X() ];
      entryPointIPs[i] = decoder.readInt32X();
    }

    n = decoder.readInt32X();
    code = new int[ n ];
    for (int i=0; i<n; ++i)
    {
      code[i] = decoder.readInt32X();
    }
  }

  public BossTokenData tokenData;
  BossInt32List tokens = new BossInt32List();
  public void accept( int tokenType )
  {
    tokenData.setPosition( filepath, nextLine, nextColumn );
    if (tokenType == 73)  // 73 == STRING - FIXME
    {
      tokens.add( tokenData.createToken(tokenType,buffer.toString()) );
    }
    else
    {
      tokens.add( tokenData.createToken(tokenType) );
    }
    //System.out.println( tokenType + " (" + buffer + ")" );
  }

  public void execute()
  {
    int ch = 0;
    int count = 0;
    int result = 0;
    int ip = startIP;
    BossStringBuilder buffer = this.buffer;
    buffer.clear();
    String source = this.source;
    int sourceCount = source.length();
    int sourcePosition = 0;
    int line=1, column=1;

    for (;;)
    {
      int opcode = code[ ip++ ];
      switch (opcode)
      {
        case OPCODE_HALT:
          return;
        case OPCODE_RESTART:
          buffer.clear();
          ip = startIP;
          continue;
        case OPCODE_MODE_INT32:
          startIP = code[ ip++ ];
          continue;
        case OPCODE_ERROR:
          throw new BossError( buffer.toString(), filepath, line, column );
        case OPCODE_MARK_SOURCE_POS:
          nextLine   = line;
          nextColumn = column;
          continue;
        case OPCODE_ACCEPT_INT32:
        {
          int tokenType = code[ ip ];
          ip = startIP;
          accept( tokenType );
          buffer.clear();
          continue;
        }
        case OPCODE_JUMP:
          ip = code[ ip ];
          continue;
        case OPCODE_JUMP_EQ:
          if (result == 0) ip = code[ ip ];
          else             ++ip;
          continue;
        case OPCODE_JUMP_NE:
          if (result != 0) ip = code[ ip ];
          else             ++ip;
          continue;
        case OPCODE_JUMP_LT:
          if (result < 0) ip = code[ ip ];
          else            ++ip;
          continue;
        case OPCODE_JUMP_LE:
          if (result <= 0) ip = code[ ip ];
          else             ++ip;
          continue;
        case OPCODE_JUMP_GT:
          if (result > 0) ip = code[ ip ];
          else            ++ip;
          continue;
        case OPCODE_JUMP_GE:
          if (result >= 0) ip = code[ ip ];
          else             ++ip;
          continue;
        case OPCODE_CALL:
          stack.add( ip+1 );
          ip = code[ ip ];
          continue;
        case OPCODE_RETURN:
          if (stack.count == 0) throw new BossError( "[INTERNAL] 'return' on empty stack." );
          ip = stack.removeAt( stack.count-1 );
          continue;
        case OPCODE_HAS_ANOTHER:
          result = (sourcePosition < sourceCount) ? 1 : 0;
          continue;
        case OPCODE_PEEK_CH_INT32:
        {
          int pos = sourcePosition + code[ip++];
          if (pos < sourceCount) ch = source.charAt(pos);
          else                   ch = 0;
          continue;
        }
        case OPCODE_PEEK_CH_COUNT:
        {
          int pos = sourcePosition + count;
          if (pos < sourceCount) ch = source.charAt(pos);
          else                   ch = 0;
          continue;
        }
        case OPCODE_READ_CH:
          if (sourcePosition < sourceCount)
          {
            switch (ch=source.charAt(sourcePosition++))
            {
              case '\n': ++line; column=1; break;
              case '\r': column=1; break;
              case '\t': column += 4; break;
              default:   ++column;
            }
          }
          else
          {
            ch = 0;
          }
          continue;
        case OPCODE_CONSUME_CHARACTER:
        {
          char value = (char) code[ip++];
          if (sourcePosition < sourceCount && source.charAt(sourcePosition) == value)
          {
            ++sourcePosition;
            result = 1;
          }
          else
          {
            result = 0;
          }
          continue;
        }
        case OPCODE_SCAN_DIGITS:
        {
          int min_digits = code[ ip ];
          int max_digits = code[ ip+1 ];
          int base = code[ ip+2 ];
          ip += 3;
          ch = 0;
          int n = 0;
          for (int i=0; i<max_digits; ++i)
          {
            if (sourcePosition >= sourceCount) break;
            int nextCh = source.charAt( sourcePosition );
            if (nextCh < '0' || nextCh > '9')  break;
            ch = ch * base + (nextCh - '0');
            ++n;
          }
          result = n - min_digits;
          continue;
        }
        case OPCODE_SCAN_IDENTIFIER:
        {
          char c;
          if (sourcePosition < sourceCount && (id_characters[(c=source.charAt(sourcePosition))&0x7F]) == 1)
          {
            buffer.clear();
            ++sourcePosition;
            buffer.print( c );
            while (sourcePosition < sourceCount && (id_characters[(c=source.charAt(sourcePosition))&0x7F]) > 0)
            {
              ++sourcePosition;
              buffer.print( c );
            }
            result = 1;
          }
          else
          {
            result = 0;
          }
          continue;
        }
        case OPCODE_CLEAR_BUFFER:
          buffer.clear();
          continue;
        case OPCODE_COLLECT_CH:
          buffer.print( (char)ch );
          continue;
        case OPCODE_COLLECT_CHARACTER:
          buffer.print( (char)(code[ip++]) );
          continue;
        case OPCODE_COLLECT_STRING:
          buffer.print( strings[code[ip++]] );
          continue;
        case OPCODE_PRINT_BUFFER:
          for (int i=0; i<buffer.count; ++i)
          {
            System.out.print( buffer.charAt(i) );
          }
          continue;
        case OPCODE_PRINT_CH:
          System.out.print( (char)ch );
          continue;
        case OPCODE_PRINT_COUNT:
          System.out.print( count );
          continue;
        case OPCODE_PRINT_CHARACTER:
          System.out.print( (char)(code[ip++]) );
          continue;
        case OPCODE_PRINT_STRING:
          System.out.print( strings[ code[ip++] ] );
          continue;
        case OPCODE_COMPARE_CH_INT32:
          result = ch - code[ip++];
          continue;
        case OPCODE_COMPARE_COUNT_INT32:
          result = count - code[ip++];
          continue;
        case OPCODE_CH_IS_DIGIT_INT32:
        {
          int base = code[ip++];
          int n = toNumber( (char)ch, base );
          result = (n>=0 && n<base) ? 1 : 0;
          continue;
        }
        case OPCODE_CH_IS_DIGIT_COUNT:
        {
          int n = toNumber( (char)ch, count );
          result = (n>=0 && n<count) ? 1 : 0;
          continue;
        }
        case OPCODE_CH_IS_LETTER:
          result = ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z')) ? 1 : 0;
          continue;
        case OPCODE_TEST_CH:
          result = ch;
          continue;
        case OPCODE_TEST_COUNT:
          result = count;
          continue;
        case OPCODE_PUSH_CH:
          stack.add( ch );
          continue;
        case OPCODE_PUSH_COUNT:
          stack.add( count );
          continue;
        case OPCODE_POP_CH:
          if (stack.count > 0) ch = stack.removeLast();
          continue;
        case OPCODE_POP_COUNT:
          if (stack.count > 0) count = stack.removeLast();
          continue;
        case OPCODE_SET_CH_TO_INT32:
          result = ch = code[ ip++ ];
          continue;
        case OPCODE_SET_CH_TO_COUNT:
          result = ch = count;
          continue;
        case OPCODE_SET_COUNT_TO_INT32:
          result = count = code[ ip++ ];
          continue;
        case OPCODE_SET_COUNT_TO_CH:
          result = count = ch;
          continue;
        case OPCODE_ADD_CH_INT32:
          ch += code[ ip++ ];
          continue;
        case OPCODE_ADD_COUNT_INT32:
          count += code[ ip++ ];
          continue;
        case OPCODE_SCAN_TABLE:
        {
          int curNode = ip;
          int lookahead = 0;
          int lastAcceptableNode = 0;
          int lastAcceptableLinkCount = 0;
          int lastAcceptableLookahead = 0;
          for (;;)
          {
            int linkCount = code[curNode+1];
            if (code[curNode] != 0)
            {
              lastAcceptableNode = curNode;
              lastAcceptableLinkCount = linkCount;
              lastAcceptableLookahead = lookahead;
            }
            if (sourcePosition + lookahead + 1 > sourceCount) break;
            int c = source.charAt( sourcePosition+lookahead );
            ip = curNode + 2;
            boolean sufficient = false;
            for (int i=linkCount; --i>=0;)
            {
              if (c == code[ip])
              {
                curNode = code[ ip+1 ];
                ++lookahead;
                sufficient = true;
                break;
              }
              ip += 2;
            }
            if (sufficient) continue;
            break;
          }

          // Either no links match or EOI - jump to code of last acceptable node.
          // The start node is always acceptable and either contains the
          // 'others' case or jumps to the end of the scan table.
          ip = lastAcceptableNode + lastAcceptableLinkCount * 2 + 2;
          for (int i=lastAcceptableLookahead; --i>=0;) buffer.print( source.charAt(sourcePosition++) );
          continue;
        }
        case OPCODE_WHICH_BUFFER:
        {
          int curNode = ip;
          int startNode = curNode;
          int lookahead = 0;
          for (;;)
          {
            int linkCount = code[curNode+1];
            if (lookahead == buffer.count) break;
            int c = buffer.characters[lookahead];
            ip = curNode + 2;
            boolean sufficient = false;
            for (int i=linkCount; --i>=0;)
            {
              if (c == code[ip])
              {
                curNode = code[ ip+1 ];
                ++lookahead;
                sufficient = true;
                break;
              }
              ip += 2;
            }
            if (sufficient) continue;
            break;
          }

          // Either no links match or EOI
          if (code[curNode] == 0 || lookahead < buffer.count)
          {
            // Not a complete match; use the 'others' code in the start node
            curNode = startNode;
          }
          ip = curNode + code[curNode+1] * 2 + 2;
          continue;
        }
        default:
        {
          throw new BossError( "[INTERNAL] Unhandled opcode: " + opcode );
        }
      }
    }
  }

  public int toNumber( char ch, int base )
  {
    if (ch>='0' && ch<='9') return (ch - '0');
    if (ch>='a' && ch<='z') return (ch - 'a') + 26;
    if (ch>='A' && ch<='Z') return (ch - 'A') + 26;
    return -1;
  }

  public int[] tokenize( File file )
  {
    buffer.clear();
    buffer.reserve( (int) file.length() );
    try
    {
      BufferedReader reader = new BufferedReader( new InputStreamReader( new FileInputStream(file.getPath()) ), 1024 );

      int firstCh = reader.read();
      if (firstCh != -1 && firstCh != 0xFEFF)
      {
        // Discard Byte Order Mark (BOM)
        buffer.print( (char) firstCh );
      }

      for (int ch=reader.read(); ch!=-1; ch=reader.read())
      {
        buffer.print( (char) ch );
      }
      reader.close();

      filepath = file.getPath();
      source = buffer.toString();
    }
    catch (IOException err)
    {
      throw new BossError( "Error reading \"" + file.getPath() + "\"." );
    }

    tokens.clear();
    execute();
    return tokens.toArray();
  }

  static class Base64IntXDecoder
  {
    String            data;
    BossStringBuilder builder = new BossStringBuilder();
    int    remainingBase64;
    int    position;
    int    available;
    int    nextByte;
    int    nextNextByte;

    Base64IntXDecoder( String encoded )
    {
      while ((encoded.length() & 3) != 0) encoded += '=';

      remainingBase64 = (encoded.length() * 3) / 4;
      for (int i=encoded.length(); --i>=0;)
      {
        if (encoded.charAt(i) != '=') break;
        --remainingBase64;
      }
      data = encoded;
    }

    boolean hasAnother()
    {
      return (remainingBase64 > 0);
    }

    int readInt32X()
    {
      int result = readBase64();
      if ((result & 0xC0) != 0x80)
      {
        // leftmost 2 bits are not %10
        // -64..127
        if (result < 128) return result;
        else              return (result - 256);
      }
      else
      {
        // 10NNNxxx xxxxxxxx*N
        int n = ((result>>3) & 7) + 1; // 1 to 8 bytes follow
        result = (result & 7);         //  0..7 (unsigned)
        if (result >= 4) result -= 8;  // -4..3 (signed)

        for (int i=n; --i>=0;) result = (result << 8) | readBase64();

        return result;
      }
    }

    String readString()
    {
      builder.clear();
      int n = readInt32X();
      for (int i=n; --i>=0;)
      {
        builder.writeUnicode( readInt32X() );
      }
      return builder.toString();
    }

    int readBase64()
    {
      if (remainingBase64 == 0) return 0;
      --remainingBase64;

      if (--available >= 0)
      {
        int result = nextByte;
        nextByte = nextNextByte;
        return result;
      }

      int b1 = base64ToValue( position );
      int b2 = base64ToValue( position+1 );
      int b3 = base64ToValue( position+2 );
      int b4 = base64ToValue( position+3 );
      position += 4;

      int result = (b1 << 2) | (b2 >> 4);
      nextByte = ((b2 & 15) << 4) | (b3 >> 2);
      nextNextByte = ((b3 & 3) << 6) | b4;
      available = 2;
      return result;
    }

    int base64ToValue( int pos )
    {
      int base64 = data.charAt( pos );
      if (base64 >= 'A' && base64 <= 'Z') return (base64 - 'A');
      if (base64 >= 'a' && base64 <= 'z') return (base64 - 'a') + 26;
      if (base64 >= '0' && base64 <= '9') return (base64 - '0') + 52;
      if (base64 == '+') return 62;
      return 63;
    }
  }

}

