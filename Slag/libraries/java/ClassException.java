package com.plasmaworks.slag;

public class ClassException extends RuntimeException
{
  public ClassString property_message;
  public ClassStackTrace property_stack_trace = new ClassStackTrace();

  public ClassException()
  {
    init_object();
    init();
  }

  public ClassException( Slag.CallInit call_init )
  {
    if (call_init == Slag.CallInit.TRUE) init_object();
  }

  public ClassException( ClassString local_0_message)
  {
    init_object();
    init(local_0_message);
  }

  public void init()
  {
    property_message = new ClassString("");
  }

  public void init(ClassString local_0_message)
  {
    property_message = local_0_message;
  }

  public ClassString type_name() { return new ClassString("Exception"); }

  public ClassString to_String()
  {
    return property_message;
  }

  public String toString()
  {
    return property_message.data;
  }

  public void init_object()
  {
  }
}

