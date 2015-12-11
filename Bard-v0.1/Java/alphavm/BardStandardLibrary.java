package alphavm;

import java.io.*;
import java.util.*;

public class BardStandardLibrary
{
  static public void install( BardVM vm )
  {
    vm.register_native( "Byte[]", "add(Integer)",       new ByteList__add__Integer() );
    vm.register_native( "Byte[]", "clear()",            new ByteList__clear() );
    vm.register_native( "Byte[]", "count()",            new ByteList__count() );
    vm.register_native( "Byte[]", "get(Integer)",       new ByteList__get__Integer() );
    vm.register_native( "Character[]", "add(Variant)",       new CharacterList__add__Variant() );
    vm.register_native( "Character[]", "ensure_capacity(Integer)", new CharacterList__ensure_capacity__Integer() );
    vm.register_native( "Character[]", "clear()",            new CharacterList__clear() );
    vm.register_native( "File",   "Character[]()",           new File__CharacterList() );
    vm.register_native( "FileReader", "init(String)",   new FileReader__init__String() );
    vm.register_native( "FileReader", "available()",    new FileReader__available() );
    vm.register_native( "FileReader", "has_another()",  new FileReader__has_another() );
    vm.register_native( "FileReader", "peek()",         new FileReader__peek() );
    vm.register_native( "FileReader", "read()",         new FileReader__read() );
    vm.register_native( "FileReader", "close()",        new FileReader__close() );
    vm.register_native( "Integer[]", "add(Integer)",    new IntegerList__add__Integer() );
    vm.register_native( "Integer[]", "clear()",         new IntegerList__clear() );
    vm.register_native( "Integer[]", "count()",         new IntegerList__count() );
    vm.register_native( "Integer[]", "ensure_capacity(Integer)", new IntegerList__ensure_capacity__Integer() );
    vm.register_native( "Integer[]", "get(Integer)",    new IntegerList__get__Integer() );
    vm.register_native( "Integer[]", "set(Integer,Integer)", new IntegerList__set__Integer_Integer() );
    vm.register_native( "List",   "remove(Variant)",    new List__remove__Variant() );
    vm.register_native( "Math",   "floor(Real)",        new Math__floor__Real() );
    vm.register_native( "NativeFileWriter", "init(String)", new NativeFileWriter__init__String() );
    vm.register_native( "NativeFileWriter", "ready()",      new NativeFileWriter__ready() );
    vm.register_native( "NativeFileWriter", "write(Character)",  new NativeFileWriter__write__Character() );
    vm.register_native( "NativeFileWriter", "close()",      new NativeFileWriter__close() );
    vm.register_native( "Object", "String()",           new Object__String() );
    vm.register_native( "Random", "seed(Integer)",      new Random__seed__Integer() );
    vm.register_native( "Random", "bits(Integer)",      new Random__bits__Integer() );
    vm.register_native( "String", "count()",            new String__count() );
    vm.register_native( "String", "get(Integer)",       new String__get__Integer() );
    vm.register_native( "String", "compare_to(String)",         new String__compare_to__String() );
    vm.register_native( "String", "substring(Integer,Integer)", new String__substring__Integer_Integer() );
    vm.register_native( "System", "sleep(Real)",        new System__sleep__Real() );
    vm.register_native( "System", "Real_to_Integer_high_bits(Real)",  new System__Real_to_Integer_high_bits__Real() );
    vm.register_native( "System", "Real_to_Integer_low_bits(Real)",  new System__Real_to_Integer_low_bits__Real() );
    vm.register_native( "Table",  "at(Integer)",        new Table__at__Integer() );
    vm.register_native( "Table",  "contains(Variant)",  new Table__contains__Variant() );
    vm.register_native( "Table",  "key(Integer)",       new Table__key__Integer() );
    vm.register_native( "Table",  "keys()",             new Table__keys() );
    vm.register_native( "Time",   "current()",          new Time__current() );

    //vm.register_native( "Byte[]", "add(Integer)",       new ByteList__add__Integer() );
    //vm.register_native( "Byte[]", "clear()",            new ByteList__clear() );
    //vm.register_native( "Byte[]", "count()",            new ByteList__count() );
    //vm.register_native( "Byte[]", "get(Integer)",       new ByteList__get__Integer() );
    //vm.register_native( "Character[]", "add(Variant)",       new CharacterList__add__Variant() );
    //vm.register_native( "Character[]", "ensure_capacity(Integer)", new CharacterList__ensure_capacity__Integer() );
    //vm.register_native( "Character[]", "clear()",            new CharacterList__clear() );
    //vm.register_native( "File",   "Character[]()",           new File__CharacterList() );
    //vm.register_native( "FileReader", "init(String)",   new FileReader__init__String() );
    //vm.register_native( "FileReader", "available()",    new FileReader__available() );
    //vm.register_native( "FileReader", "has_another()",  new FileReader__has_another() );
    //vm.register_native( "FileReader", "peek()",         new FileReader__peek() );
    //vm.register_native( "FileReader", "read()",         new FileReader__read() );
    //vm.register_native( "FileReader", "close()",        new FileReader__close() );
    //vm.register_native( "Integer[]", "add(Integer)",    new IntegerList__add__Integer() );
    //vm.register_native( "Integer[]", "clear()",         new IntegerList__clear() );
    //vm.register_native( "Integer[]", "count()",         new IntegerList__count() );
    //vm.register_native( "Integer[]", "ensure_capacity(Integer)", new IntegerList__ensure_capacity__Integer() );
    //vm.register_native( "Integer[]", "get(Integer)",    new IntegerList__get__Integer() );
    //vm.register_native( "Integer[]", "set(Integer,Integer)", new IntegerList__set__Integer_Integer() );
    //vm.register_native( "List",   "remove(Variant)",    new List__remove__Variant() );
    vm.register_native( "Math",   "floor(Real).Real",        new Math__floor__Real() );
    //vm.register_native( "NativeFileWriter", "init(String)", new NativeFileWriter__init__String() );
    //vm.register_native( "NativeFileWriter", "ready()",      new NativeFileWriter__ready() );
    //vm.register_native( "NativeFileWriter", "write(Character)",  new NativeFileWriter__write__Character() );
    //vm.register_native( "NativeFileWriter", "close()",      new NativeFileWriter__close() );
    //vm.register_native( "Object", "String()",           new Object__String() );
    //vm.register_native( "Random", "seed(Integer)",      new Random__seed__Integer() );
    //vm.register_native( "Random", "bits(Integer)",      new Random__bits__Integer() );
    //vm.register_native( "String", "count()",            new String__count() );
    //vm.register_native( "String", "get(Integer)",       new String__get__Integer() );
    //vm.register_native( "String", "compare_to(String)",         new String__compare_to__String() );
    //vm.register_native( "String", "substring(Integer,Integer)", new String__substring__Integer_Integer() );
    //vm.register_native( "System", "sleep(Real)",        new System__sleep__Real() );
    //vm.register_native( "System", "Real_to_Integer_high_bits(Real)",  new System__Real_to_Integer_high_bits__Real() );
    //vm.register_native( "System", "Real_to_Integer_low_bits(Real)",  new System__Real_to_Integer_low_bits__Real() );
    //vm.register_native( "Table",  "at(Integer)",        new Table__at__Integer() );
    //vm.register_native( "Table",  "contains(Variant)",  new Table__contains__Variant() );
    //vm.register_native( "Table",  "key(Integer)",       new Table__key__Integer() );
    //vm.register_native( "Table",  "keys()",             new Table__keys() );
    //vm.register_native( "Time",   "current()",          new Time__current() );

    vm.register_native( "Byte[]",
      new BardObjectFactory()
      {
        public BardObject create( BardType of_type )
        {
          return new BardByteList( of_type, 0 );
        }
      }
    );

    vm.register_native( "FileReader",
      new BardObjectFactory()
      {
        public BardObject create( BardType of_type )
        {
          return new BardFileReader( of_type );
        }
      }
    );

    vm.register_native( "NativeFileWriter",
      new BardObjectFactory()
      {
        public BardObject create( BardType of_type )
        {
          return new BardFileWriter( of_type );
        }
      }
    );

    vm.register_native( "Random",
        new BardObjectFactory()
        {
          public BardObject create( BardType of_type )
          {
            return new ClassRandom( of_type );
          }
        }
      );

  }

  //===========================================================================
  //  Byte[]
  //===========================================================================
  static public class ByteList__add__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int n = processor.pop_Integer();
      BardByteList list = (BardByteList) processor.peek_Object(); 
      list.add( n );
    }
  }

  static public class ByteList__clear extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardByteList list = (BardByteList) processor.peek_Object(); 
      list.clear();
    }
  }

  static public class ByteList__count extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardByteList list = (BardByteList) processor.pop_Object(); 
      processor.push_Integer( list.count );
    }
  }

  static public class ByteList__get__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int index = processor.pop_Integer();
      BardByteList list = (BardByteList) processor.pop_Object(); 
      processor.push_Integer( list.get( index ) );
    }
  }


  //===========================================================================
  //  Character[]
  //===========================================================================
  static public class CharacterList__add__Variant extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardCharacterList list = (BardCharacterList) processor.Object_stack[ processor.sp+1 ];

      switch (processor.peek_Variant_type())
      {
        case BardType.variant_Object:
          list.print( processor.pop_Object() );
          return;

        case BardType.variant_Object_null:
          processor.pop_discard();
          list.print( "null" );
          return;

        case BardType.variant_Real:
          list.print( processor.pop_Real() );
          return;

        case BardType.variant_Integer:
          list.print( processor.pop_Integer() );
          return;

        case BardType.variant_Character:
          list.add( processor.pop_Character() );
          return;

        case BardType.variant_Logical:
          list.print( processor.pop_Logical() );
          return;
      }

      throw new RuntimeException( "[Internal Error]" );
    }
  }

  static public class CharacterList__ensure_capacity__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int n = processor.pop_Integer();
      BardCharacterList list = (BardCharacterList) processor.pop_Object();
      list.ensure_capacity( n );
    }
  }

  static public class CharacterList__clear extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardCharacterList list = (BardCharacterList) processor.peek_Object(); 
      list.clear();
    }
  }

  //===========================================================================
  //  File
  //===========================================================================
  static public class File__CharacterList extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardObject file_object = processor.pop_Object();
      processor.push_property( file_object, "filepath" );
      String filepath = processor.pop_String();

      try
      {
        FileInputStream infile = new FileInputStream( filepath );
        BardCharacterList list = processor.vm.create_CharacterList_object( infile.available() );
        for (int ch=infile.read(); ch!=-1; ch=infile.read())
        {
          list.add( ch );
        }
        infile.close();
        processor.push_Object( list );
      }
      catch (IOException err)
      {
        processor.push_Object( null );
      }
    }
  }

  //===========================================================================
  //  FileReader
  //===========================================================================
  static public class FileReader__init__String extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      String filepath = processor.pop_String();
      BardFileReader reader = (BardFileReader) processor.peek_Object();
      reader.init( filepath );
    }
  }

  static public class FileReader__available extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardFileReader reader = (BardFileReader) processor.pop_Object();
      processor.push_Integer( reader.available() );
    }
  }

  static public class FileReader__has_another extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardFileReader reader = (BardFileReader) processor.pop_Object();
      processor.push_Logical( reader.next_ch != -1 );
    }
  }

  static public class FileReader__peek extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int ch = ((BardFileReader) processor.pop_Object()).next_ch;
      processor.push_Character_Variant( ch != -1 ? ch : 0 );
    }
  }

  static public class FileReader__read extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardFileReader reader = (BardFileReader) processor.pop_Object();
      processor.push_Character_Variant( reader.read() );
    }
  }

  static public class FileReader__close extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardFileReader reader = (BardFileReader) processor.pop_Object();
      reader.close();
    }
  }

  //===========================================================================
  //  Integer[]
  //===========================================================================
  static public class IntegerList__add__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int n = processor.pop_Integer();
      BardIntegerList list = (BardIntegerList) processor.peek_Object(); 
      list.add( n );
    }
  }

  static public class IntegerList__clear extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardIntegerList list = (BardIntegerList) processor.peek_Object(); 
      list.clear();
    }
  }

  static public class IntegerList__count extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardIntegerList list = (BardIntegerList) processor.pop_Object(); 
      processor.push_Integer( list.count );
    }
  }

  static public class IntegerList__ensure_capacity__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int n = processor.pop_Integer();
      BardIntegerList list = (BardIntegerList) processor.peek_Object();
      list.ensure_capacity( n );
    }
  }

  static public class IntegerList__get__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int index = processor.pop_Integer();
      BardIntegerList list = (BardIntegerList) processor.pop_Object(); 
      processor.push_Integer( list.get( index ) );
    }
  }

  static public class IntegerList__set__Integer_Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int new_value = processor.pop_Integer();
      int index = processor.pop_Integer();
      BardIntegerList list = (BardIntegerList) processor.peek_Object(); 
      list.set( index, new_value );
    }
  }

  //===========================================================================
  //  List__remove__Variant
  //===========================================================================
  static public class List__remove__Variant extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardList list = (BardList) processor.Object_stack[ processor.sp+1 ];

      boolean result;
      switch (processor.peek_Variant_type())
      {
        case BardType.variant_Object:
          result = list.remove_Object( processor.pop_Object() );
          break;

        case BardType.variant_Object_null:
          processor.pop_discard();
          result = list.remove_Object( null );
          break;

        case BardType.variant_Real:
          result = list.remove_Real( processor.pop_Real() );
          break;

        case BardType.variant_Integer:
          result = list.remove_Integer( processor.pop_Integer() );
          break;

        case BardType.variant_Character:
          result = list.remove_Character( processor.pop_Character() );
          break;

        case BardType.variant_Logical:
          result = list.remove_Logical( processor.pop_Logical() ? 1 : 0 );
          break;

        default:
          throw new RuntimeException("[Internal] Unhandled variant type in List::remove(Variant).");
      }

      processor.pop_discard();
      processor.push_Logical( result );
    }
  }

  //===========================================================================
  //  Math
  //===========================================================================
  static public class Math__floor__Real extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      double n = processor.pop_Real();
      processor.pop_discard();
      processor.push_Real( Math.floor(n) );
    }
  }


  //===========================================================================
  //  NativeFileWriter
  //===========================================================================
  static public class NativeFileWriter__init__String extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      String filepath = processor.pop_String();
      BardFileWriter writer = (BardFileWriter) processor.peek_Object();
      writer.init( filepath );
    }
  }

  static public class NativeFileWriter__ready extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardFileWriter writer = (BardFileWriter) processor.pop_Object();
      processor.push_Logical( writer.outfile != null );
    }
  }

  static public class NativeFileWriter__write__Character extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int ch = processor.pop_Integer();
//System.out.println( "Writing " + ch );
      BardFileWriter writer = (BardFileWriter) processor.pop_Object();
      writer.write( ch );
    }
  }

  static public class NativeFileWriter__close extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardFileWriter writer = (BardFileWriter) processor.pop_Object();
      writer.close();
    }
  }


  //===========================================================================
  //  Object
  //===========================================================================
  static public class Object__String extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardObject obj = processor.pop_Object();
      processor.push_String( obj.toString() );
    }
  }


  //===========================================================================
  //  Random
  //===========================================================================
  static public class Random__seed__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int seed = processor.pop_Integer();
      ClassRandom r = (ClassRandom) processor.pop_Object(); 
      r.set_seed( seed );
    }
  }

  static public class Random__bits__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int count = processor.pop_Integer();
      ClassRandom r = (ClassRandom) processor.pop_Object(); 
      processor.push_Integer( r.bits(count) );
    }
  }

  static public class String__count extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardString st = (BardString) processor.pop_Object(); 
      processor.push_Integer( st.count );
    }
  }

  static public class String__get__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int index = processor.pop_Integer();
      BardString st = (BardString) processor.pop_Object(); 
      processor.push_Character( st.data[ index ] );
    }
  }

  static public class String__compare_to__String extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardString st2 = (BardString) processor.pop_Object();
      BardString st1 = (BardString) processor.pop_Object();

      int result;
      if (st2 == null)
      {
        result = 1;
      }
      else
      {
        result = 0;
        char[] data1 = st1.data;
        char[] data2 = st2.data;
        int count1 = st1.count;
        int count2 = st2.count;
        int count = (count1 < count2) ? count1 : count2;
        for (int i=0; i<count; ++i)
        {
          int ch1 = data1[i];
          int ch2 = data2[i];
          if (ch1 == ch2) continue;
          if (ch1 > ch2) result =  1;
          else           result = -1;
          break;
        }
        if (result == 0)
        {
          if (count1 < count2) result = -1;
          else if (count1 > count2) result = 1;
        }
      }
      processor.push_Integer( result );
    }
  }

  static public class String__substring__Integer_Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int    i2 = processor.pop_Integer();
      int    i1 = processor.pop_Integer();
      BardString st = (BardString) processor.pop_Object();
      processor.push_Object( st.substring(i1,i2) );
    }
  }

  static public class System__sleep__Real extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      double seconds = processor.pop_Real();
      BardObject obj = processor.pop_Object();
      try
      {
        Thread.sleep( (long)(seconds*1000) );
      }
      catch (InterruptedException no_problem)
      {
      }
    }
  }

  static public class System__Real_to_Integer_high_bits__Real extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      double value = processor.pop_Real();  // System object
      BardObject obj = processor.pop_Object();
      processor.push_Integer( (int) (Double.doubleToLongBits(value) >> 32) );
    }
  }

  static public class System__Real_to_Integer_low_bits__Real extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      double value = processor.pop_Real();  // System object
      BardObject obj = processor.pop_Object();
      processor.push_Integer( (int) Double.doubleToLongBits(value) );
    }
  }

  static public class Table__at__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int index = processor.pop_Integer();
      BardTable  table = (BardTable) processor.pop_Object();
      processor.push_Variant( table.values[ table.order[index] ] );
    }
  }

  static public class Table__contains__Variant extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardObject key = processor.Object_stack[ processor.sp ];
      int table_sp = processor.sp + 1;

      BardTable  table = (BardTable) processor.Object_stack[table_sp];

      key = processor.stack_to_Variant( processor.sp, key, table.test_key );
      processor.sp = table_sp;

      if (key != null)
      {
        table.test_key = key;

        processor.Integer_stack[ table_sp ] = (table.get(key) != null) ? 1 : 0;
      }
    }
  }

  static public class Table__key__Integer extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      int index = processor.pop_Integer();
      BardTable table = (BardTable) processor.pop_Object();
      processor.push_Variant( table.key(index) );
    }
  }

  static public class Table__keys extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardTable table = (BardTable) processor.pop_Object();

      BardVM vm = processor.vm;
      BardList list = vm.create_List_object( table.count );

      for (int i=0; i<table.count; ++i)
      {
        processor.push_Variant( table.key(i) );
        list.add( processor.pop_Variant() );
      }

      processor.push_Object( list );
    }
  }

  static public class Time__current extends BardNativeMethod
  {
    public void execute( BardProcessor processor )
    {
      BardObject obj = processor.pop_Object();
      processor.push_Real( ((double) System.currentTimeMillis()) / 1000.0 );
    }
  }

  static public class ClassRandom extends BardObject
  {
    public long seed;
    public long next;

    public ClassRandom( BardType type ) { super(type); }

    public void set_seed( long seed )
    {
      this.seed = (seed ^ 0x5DEECe66dL) ^ ((1L<<48L)-1L);
      next = this.seed;
    }

    public int bits( int n )
    {
      next = (next * 0x5DEECe66dL + 11L) & ((1L<<48L)-1L);
      return (int) (next >> (48 - n));
    }
  }
}
