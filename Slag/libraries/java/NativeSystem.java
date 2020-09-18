package com.plasmaworks.slag;

public class NativeSystem
{
  static public ClassString device_id( ClassSystem obj )
  {
    throw new RuntimeException( "TODO: NativeSystem::device_id()" );
  }

  static public void open_url( ClassSystem obj, ClassString url )
  {
    throw new RuntimeException( "TODO: NativeSystem::open_url()" );
  }

  static public void exit_program( ClassSystem obj, int code, ClassString mesg )
  {
    throw new RuntimeException( "TODO: NativeSystem::exit_program()" );
  }

  static public ClassString language( ClassSystem obj )
  {
    return new ClassString("english");
  }

  static public ClassString os( ClassSystem obj )
  {
    return new ClassString("android");
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

