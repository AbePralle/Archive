//=============================================================================
// Bard.cs
//
// $(PLASMACORE_VERSION) $(DATE)
//
// Bard standard library Native Layer implementation for Windows Phone 7.
// 
// ----------------------------------------------------------------------------
//
// Copyright 2010 Plasmaworks LLC
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//
//   http://www.apache.org/licenses/LICENSE-2.0 
//
// Unless required by applicable law or agreed to in writing, 
// software distributed under the License is distributed on an 
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, 
// either express or implied. See the License for the specific 
// language governing permissions and limitations under the License.
//
//=============================================================================
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BardFramework
{
  public class Bard
  {
    public enum CallInit { FALSE, TRUE };

    static public void configure()
    {
      GameXC.singleton_ClassGlobal.property_stdout = new ClassStdOutWriter();
    }

    static public void log(System.String mesg)
    {
      System.Diagnostics.Debug.WriteLine(mesg);
    }

    static public ClassString str( String st )
    {
      return new ClassString(st);
    }

    static public String cs_str( ClassString st )
    {
      return st.data;
    }

    static public double mod( double a, double b )
    {
      double q = a / b;
      return (double)(a - Math.Floor(q)*b);
    }

    static public float mod( float a, float b )
    {
      float q = a / b;
      return (float)(a - Math.Floor(q)*b);
    }

    static public long mod( long a, long b )
    {
      if (b == 1)
      {
        return 0;
      }
      else if ((a^b) < 0)
      {
        long r = a % b;
        return ((r!=0) ? (r+b) : 0);
      }
      else
      {
        return (a % b);
      }
    }

    static public int mod( int a, int b )
    {
      if (b == 1)
      {
        return 0;
      }
      else if ((a^b) < 0)
      {
        int r = a % b;
        return ((r!=0) ? (r+b) : 0);
      }
      else
      {
        return (a % b);
      }
    }

    static public long shr( long n, int count )
    {
      if (count == 0) return n;
      n = (n >> 1) & 0x7FFFffffFFFFffffL;
      if (count > 1)
      {
        n >>= (count-1);
      }
      return n;
    }

    static public int shr( int n, int count )
    {
      if (count == 0) return n;
      n = (n >> 1) & 0x7FFFffff;
      if (count > 1)
      {
        n >>= (count-1);
      }
      return n;
    }

    static public long rol( long n, int count )
    {
      if (count == 0) return n;
      return (n<<count) | ((n >> (64-count)) & ((1L << count) - 1L));
    }

    static public int rol( int n, int count )
    {
      if (count == 0) return n;
      return (n<<count) | ((n >> (32-count)) & ((1 << count) - 1));
    }

    static public long ror( long n, int count )
    {
      if (count == 0) return n;
      int lower_bit_count = 64 - count;
      return ((n >> count) & ((1L << lower_bit_count)-1)) | (n << lower_bit_count);
    }

    static public int ror( int n, int count )
    {
      if (count == 0) return n;
      int lower_bit_count = 32 - count;
      return ((n >> count) & ((1 << lower_bit_count)-1)) | (n << lower_bit_count);
    }

    static byte[] converter_bytes = new byte[8];

    static public double to_double( long n )
    {
      // Hella ugly, but apparently the only way.
      if (System.BitConverter.IsLittleEndian)
      {
        converter_bytes[0] = (byte) (n);
        converter_bytes[1] = (byte) (n>>8);
        converter_bytes[2] = (byte) (n>>16);
        converter_bytes[3] = (byte) (n>>24);
        converter_bytes[4] = (byte) (n>>32);
        converter_bytes[5] = (byte) (n>>40);
        converter_bytes[6] = (byte) (n>>48);
        converter_bytes[7] = (byte) (n>>56);
      }
      else
      {
        converter_bytes[7] = (byte) (n);
        converter_bytes[6] = (byte) (n>>8);
        converter_bytes[5] = (byte) (n>>16);
        converter_bytes[4] = (byte) (n>>24);
        converter_bytes[3] = (byte) (n>>32);
        converter_bytes[2] = (byte) (n>>40);
        converter_bytes[1] = (byte) (n>>48);
        converter_bytes[0] = (byte) (n>>56);
      }

      return System.BitConverter.ToDouble(converter_bytes,0);
    }

    static public float to_float( int n )
    {
      return (float) to_double(n);
    }

    static public void clear_array( Array array, int i1, int i2 )
    {
      Array.Clear( array, i1, (i2-i1)+1 );
    }

    static public Object[] array_duplicate( Object[] array )
    {
      if (array == null) return null;

      int count = array.Length;
      Object[] result = new Object[count];
      while (--count >= 0)
      {
        result[count] = array[count];
      }
      return result;
    }

    static public double[] array_duplicate( double[] array )
    {
      if (array == null) return null;

      int count = array.Length;
      double[] result = new double[count];
      while (--count >= 0)
      {
        result[count] = array[count];
      }
      return result;
    }

    static public float[] array_duplicate( float[] array )
    {
      if (array == null) return null;

      int count = array.Length;
      float[] result = new float[count];
      while (--count >= 0)
      {
        result[count] = array[count];
      }
      return result;
    }

    static public long[] array_duplicate( long[] array )
    {
      if (array == null) return null;

      int count = array.Length;
      long[] result = new long[count];
      while (--count >= 0)
      {
        result[count] = array[count];
      }
      return result;
    }

    static public int[] array_duplicate( int[] array )
    {
      if (array == null) return null;

      int count = array.Length;
      int[] result = new int[count];
      while (--count >= 0)
      {
        result[count] = array[count];
      }
      return result;
    }

    static public char[] array_duplicate( char[] array )
    {
      if (array == null) return null;

      int count = array.Length;
      char[] result = new char[count];
      while (--count >= 0)
      {
        result[count] = array[count];
      }
      return result;
    }

    static public byte[] array_duplicate( byte[] array )
    {
      if (array == null) return null;

      int count = array.Length;
      byte[] result = new byte[count];
      while (--count >= 0)
      {
        result[count] = array[count];
      }
      return result;
    }

    /*
    static public class FileReaderInfo extends Data
    {
      public byte[] data;
      public int    pos;

      public FileReaderInfo( String filename )
      {
        try
        {
          FileInputStream infile = new FileInputStream(filename);
          data = new byte[infile.available()];
          infile.read( data );
        }
        catch (IOException err)
        {
        }
      }
    }
    */

  }

  public class ClassException : Exception
  {
    public ClassString property_message;
    public ClassStackTrace property_stack_trace = new ClassStackTrace(Bard.CallInit.TRUE);

    public ClassException()
    {
      init_object();
      init();
    }

    public ClassException( Bard.CallInit call_init )
    {
      if (call_init == Bard.CallInit.TRUE) init_object();
    }

    public ClassException( ClassString local_0_message)
    {
      init_object();
      init(local_0_message);
    }

    public virtual void init()
    {
      property_message = new ClassString("");
    }

    public virtual void init(ClassString local_0_message)
    {
      property_message = local_0_message;
    }

    public virtual ClassString type_name() { return new ClassString("Exception"); }

    public virtual ClassString to_String()
    {
      return property_message;
    }

    public override String ToString()
    {
      return Bard.cs_str(property_message);
    }

    public virtual void init_object()
    {
    }
  }

  abstract public class NativeData 
  {
    abstract public void release();
  }

  /*
  public class NativeDate
  { 
    static public void init( ClassDate date, long timestamp )
    {
    }

    static public long timestamp( ClassDate date )
    {
    }
  }
  */

  public class NativeFile
  {
    static public ClassString absolute_filepath( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::absolute_filepath()" );
      return null;
    }

    static public bool is_directory( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::is_directory()" );
      return false;
    }

    static public ClassArrayList_of_String directory_listing( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::directory_listing()" );
      return null;
    }

    static public bool exists( ClassFile file )
    {
      return false;
    }

    static public void rename( ClassFile file, ClassString new_name )
    {
      Bard.log( "TODO: NativeFile::rename()" );
    }

    static public void delete( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::delete()" );
    }

    static public void native_mkdir( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::native_mkdir()" );
    }

    static public long timestamp( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::timestamp()" );
      return 0;
    }

    static public void touch( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::touch()" );
    }

    static public void change_dir( ClassFile file )
    {
      Bard.log( "TODO: NativeFile::change_dir()" );
    }

  }

  public class NativeFileReader
  {
    static public void init( ClassFileReader reader, ClassString filename )
    {
      //reader.property_native_data = null;
    }

    static public void close( ClassFileReader reader )
    {
      //reader.property_native_data = null;
    }

    static public bool has_another( ClassFileReader reader )
    {
      //FileReaderInfo info = (FileReaderInfo) reader.property_native_data;
      //return (info != null && info.pos < info.data.Length) ? 1 : 0;
      return false;
    }

    static public char peek( ClassFileReader reader )
    {
      //FileReaderInfo info = (FileReaderInfo) reader.property_native_data;
      //return (char) info.data[info.pos++];
      return (char) 0;
    }

    static public char read( ClassFileReader reader )
    {
      //FileReaderInfo info = (FileReaderInfo) reader.property_native_data;
      //return (char) info.data[info.pos++];
      return (char) 0;
    }

    static public int read( ClassFileReader reader, byte[] array, int index, int count )
    {
      Bard.log( "TODO: NativeFileReader::read(byte[],...)" );
      return 0;
    }

    static public int read( ClassFileReader reader, char[] array, int index, int count )
    {
      Bard.log( "TODO: NativeFileReader::read(char[],...)" );
      return 0;
    }

    static public int remaining( ClassFileReader reader )
    {
      Bard.log( "TODO: NativeFileReader::remaining()" );
      return 0;
    }

    static public void skip( ClassFileReader reader, int skip_count )
    {
      Bard.log( "TODO: NativeFileReader::skip()" );
    }

    static public int position( ClassFileReader reader )
    {
      Bard.log( "TODO: NativeFileReader::position()" );
      return 0;
    }

  }

  public class NativeFileWriter
  {
    static public void init( ClassFileWriter writer, ClassString filename, bool append_mode )
    {
      Bard.log( "TODO: Native FileWriter::init()" );
    }

    static public void close( ClassFileWriter writer )
    {
      Bard.log( "TODO: Native FileWriter" );
    }

    static public void write( ClassFileWriter writer, char value )
    {
      Bard.log( "TODO: Native FileWriter" );
    }

    static public void write( ClassFileWriter writer, char[] array, int index, int count )
    {
      Bard.log( "TODO: Native FileWriter" );
    }

    static public void write( ClassFileWriter writer, byte[] array, int index, int count )
    {
      Bard.log( "TODO: Native FileWriter" );
    }

    static public void write( ClassFileWriter writer, ClassString st )
    {
      Bard.log( "TODO: Native FileWriter" );
    }

    static public int position( ClassFileWriter writer )
    {
      Bard.log( "TODO: Native FileWriter" );
      return 0;
    }
  }

  public class NativeWeakReferenceManager
  {
    static public ClassWeakReference create_from( Object context, ClassObject obj )
    {
      return new ClassWeakReference( obj );
    }
  }

  public class NativeWeakReference
  {
    static public void _object( ClassWeakReference context, ClassObject obj )
    {
      context.weak_reference.Target = obj;
    }

    static public ClassObject _object( ClassWeakReference context )
    {
      return context.weak_reference.Target as ClassObject;
    }
  }

  public class NativeObject
  {
    static public int hash_code( ClassObject context )
    {
      return context.GetHashCode();
    }

    static public void update( Object context )
    {
    }

    static public void release( Object context )
    {
    }
  }


  public class NativeParseReader
  {
    static public void prep_data( ClassParseReader reader )
    {
      if (reader.property_spaces_per_tab < 0) reader.property_spaces_per_tab = 0;

      int spaces_per_tab = reader.property_spaces_per_tab;

      // Count how many characters we'll have after removing \r and changing
      // each tab into a given number of spaces.
      int read_pos  = -1;
      int remaining = reader.property_remaining + 1;
      int count = 0;
      while (--remaining != 0)
      {
        switch (reader.property_data[++read_pos])
        {
          case  (char)9: count += spaces_per_tab; break;
          case (char)13: break;
          default: ++count; break;
        }
      }

      // Create a new array of the appropriate size.
      char[] new_array = new char[count];

      // Copy the data into it while filtering
      int write_pos = -1;
      read_pos  = -1;
      remaining = reader.property_remaining + 1;
      while (--remaining != 0)
      {
        char ch = reader.property_data[++read_pos];
        switch (ch)
        {
          case  (char)9: 
          {
            int spaces = spaces_per_tab + 1;
            while (--spaces != 0) new_array[++write_pos] = (char) 32;
            break;
          }

          case (char)13: break;

          default:
            new_array[++write_pos] = ch;
            break;
        }
      }

      reader.property_data = new_array;
      reader.property_remaining = count;
    }

    static public bool has_another( ClassParseReader reader )
    {
      return (reader.property_remaining > 0);
    }

    static public char peek( ClassParseReader reader )
    {
      if (reader.property_remaining == 0) return (char) 0;
      return reader.property_data[reader.property_pos];
    }

    static public char peek( ClassParseReader reader, int num_ahead )
    {
      if ((uint) --num_ahead >= (uint) reader.property_remaining) return (char) 0;
      return reader.property_data[reader.property_pos + num_ahead];
    }

    static public char read( ClassParseReader reader )
    {
      --reader.property_remaining;
      char ch = reader.property_data[reader.property_pos++];
      switch (ch)
      {
        case (char) 10:
          ++reader.property_line;
          reader.property_column = 1;
          break;

        default:
          ++reader.property_column;
          break;
      }
      return ch;
    }

    static public bool consume( ClassParseReader reader, char ch )
    {
      if (reader.property_remaining <= 0) return false;

      if (reader.property_data[reader.property_pos] == ch)
      {
        read(reader);
        return true;
      }
      else
      {
        return false;
      }
    }

    static public bool consume( ClassParseReader reader, ClassString st )
    {
      int count = st.data.Length;
      if (count > reader.property_remaining) return false;

      String data1 = st.data;
      char[] data2 = reader.property_data;
      int i1 = count;
      int i2 = reader.property_pos + count;
      while (i1 > 0)
      {
        if (data1[--i1] != data2[--i2]) return false;
      }

      reader.property_pos += count;
      reader.property_column += count;
      reader.property_remaining -= count;
      return true;
    }

  }

  public class NativeStdInReader
  {
    static public char native_read_char( ClassStdInReader reader )
    {
      return (char) 0;
    }

  }

  public class NativeStdOutWriter
  {
    static public StringBuilder log_buffer = new StringBuilder();

    static public void write( ClassStdOutWriter obj, char ch )
    {
      if (ch == 10)
      {
        Bard.log(log_buffer.ToString());
        log_buffer.Length = 0;
      }
      else
      {
        log_buffer.Append(ch);
      }
    }

    static public void print( ClassStdOutWriter obj, char ch )
    {
      if (ch == 10)
      {
        Bard.log( log_buffer.ToString() );
        log_buffer.Length = 0;
      }
      else
      {
        log_buffer.Append(ch);
      }
    }

    static public void print( ClassStdOutWriter obj, ClassString st )
    {
      String cst = Bard.cs_str(st);
      if (cst.Length > 1 || cst[0] != 10)
      {
        Bard.log(cst);
      }
    }

    static public void flush( ClassStdOutWriter obj )
    {
    }
  }

  public class NativeSocket
  {
    static public void init( ClassObject socket, ClassString address, int port )
    {
      Bard.log( "TODO: NativeSocket::init()" );
    }

    static public bool is_connected( ClassObject socket )
    {
      Bard.log( "TODO: NativeSocket::is_connected()" );
      return false;
    }

    static public void close( ClassObject socket )
    {
      Bard.log( "TODO: NativeSocket::close()" );
    }
  }


  public class NativeSocketReader
  {
    static public int available( ClassObject reader )
    {
      Bard.log( "TODO: NativeSocketReader::available()" );
      return 0;
    }

    static public bool has_another( ClassObject reader )
    {
      Bard.log( "TODO: NativeSocketReader::has_another()" );
      return false;
    }

    static public char peek( ClassObject reader )
    {
      Bard.log( "TODO: NativeSocketReader::peek()" );
      return (char) 0;
    }

    static public char read( ClassObject reader )
    {
      Bard.log( "TODO: NativeSocketReader::read()" );
      return (char) 0;
    }
  }

  public class NativeSocketWriter
  {
    static public void write( ClassObject writer, char ch )
    {
      Bard.log( "TODO: NativeSocketWriter::write()" );
    }
  }

  public class NativeStackTrace
  {
    static public long[] native_history( ClassObject stack_trace_obj )
    {
      return null;
    }

    static public ClassString describe( ClassObject stack_trace_obj, long ip )
    {
      return new ClassString("[Compiled Code]");
    }
  }

  public class NativeString
  {
    static public bool opEQ( ClassString st1, ClassString st2 )
    {
      if (st1 == st2)  return true;
      if (st2 == null) return false;

      if (st1.property_hash_code != st2.property_hash_code) return false;

      int count = st1.data.Length;
      if (count != st2.data.Length) return false;

      String data1 = st1.data;
      String data2 = st2.data;

      for (int i=0; i<count; ++i)
      {
        if (data1[i] != data2[i]) return false;
      }

      return true;
    }

    static public int opCMP( ClassString st1, ClassString st2 )
    {
      if (st1 == st2)  return 0;
      if (st2 == null) return 1;

      int count = st1.data.Length;
      if (count > st2.data.Length) count = st2.data.Length;

      String data1 = st1.data;
      String data2 = st2.data;

      for (int i=0; i<count; ++i)
      {
        if (data1[i] != data2[i])
        {
          if (data1[i] < data2[i]) return -1;
          else                     return  1;
        }
      }

      // Equal so far
      if (count < st1.data.Length) return  1;
      if (count < st2.data.Length) return -1;
      return 0;
    }

    static public ClassString substring( ClassString st, int i1, int i2 )
    {
      return new ClassString( st.data.Substring( i1, (i2-i1)+1 ) );
    }

    static public char[] to_Array( ClassString bard_st )
    {
      String st = bard_st.data;

      int count = st.Length;
      char[] result = new char[ count ];
      for (int i=0; i<count; ++i)
      {
        result[i] = st[i];
      }
      return result;
    }
  }

  public class NativeStringBuilder
  {
    static public void native_copy( ClassObject builder, ClassString bard_st, char[] array, int to_index )
    {
      String st = bard_st.data;
      for (int i=0; i<st.Length; ++i)
      {
        array[to_index+i] = st[i];
      }
    }
  }

  public class NativeStringManager
  {
    static public ClassString create_from( ClassObject context, char[] array, int count )
    {
      if (count == -1) count = array.Length;
      StringBuilder builder = new StringBuilder(count);
      for (int i=0; i<count; ++i) builder.Append( array[i] );
      return new ClassString( builder.ToString() );
    }

    static public ClassString create_from( ClassObject context, char ch )
    {
      return new ClassString( ""+ch );
    }
  }

  public class NativeSystem
  {
    static public void catch_control_c( bool setting )
    {
      // Ignore
    }

    static public bool control_c_pressed()
    {
      return false;
    }

    static public ClassString device_id( ClassSystem obj )
    {
      try
      {
        string anid = Microsoft.Phone.Info.UserExtendedProperties.GetValue("ANID") as string;
        if (anid.Length >= 34) return new ClassString(anid.Substring(2, 32));        
      }
      catch (Exception)
      {        
      }
      return new ClassString("anonymous");
    }

    static public void open_url( ClassSystem obj, ClassString url )
    {
      String url_str = Bard.cs_str(url);
      if (url_str.StartsWith("http"))
      {
        Microsoft.Phone.Tasks.WebBrowserTask task = new Microsoft.Phone.Tasks.WebBrowserTask();
        task.URL = url_str;
        task.Show();
      }
      else
      {
        Microsoft.Phone.Tasks.MarketplaceDetailTask task = new Microsoft.Phone.Tasks.MarketplaceDetailTask();
        task.ContentType = Microsoft.Phone.Tasks.MarketplaceContentType.Applications;
        task.ContentIdentifier = url_str;
        task.Show();
      }
    }

    static public void exit_program( ClassSystem obj, int code, ClassString mesg )
    {
      if (mesg != null) Bard.log( "EXITING: " + mesg.data );
      PlasmacoreWP7.Main.instance.Exit();
    }

    static public ClassString language( ClassSystem obj )
    {
      return new ClassString("english");
    }

    static public ClassString os( ClassSystem obj )
    {
#if WINDOWS_PHONE
      return new ClassString("wp7");
#elif XBOX
      return new ClassString("xbox");
#else
      return new ClassString("windows");
#endif
    }

    static public ClassString hardware_version( ClassSystem obj )
    {
      return new ClassString("unknown");
    }

    static public CompoundVector2 max_texture_size( ClassSystem obj )
    {
      return new CompoundVector2(1024,1024);
    }
  }
}

