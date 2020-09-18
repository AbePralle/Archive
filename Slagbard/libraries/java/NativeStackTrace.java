package com.plasmaworks.bard;

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

