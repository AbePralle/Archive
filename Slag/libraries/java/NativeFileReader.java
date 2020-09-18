package com.plasmaworks.slag;

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

  static public boolean has_another( ClassFileReader reader )
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
    Slag.log( "TODO: NativeFileReader::read(byte[],...)" );
    return 0;
  }

  static public int read( ClassFileReader reader, char[] array, int index, int count )
  {
    Slag.log( "TODO: NativeFileReader::read(char[],...)" );
    return 0;
  }

  static public int remaining( ClassFileReader reader )
  {
    Slag.log( "TODO: NativeFileReader::remaining()" );
    return 0;
  }

  static public void skip( ClassFileReader reader, int skip_count )
  {
    Slag.log( "TODO: NativeFileReader::skip()" );
  }

  static public int position( ClassFileReader reader )
  {
    Slag.log( "TODO: NativeFileReader::position()" );
    return 0;
  }
}

