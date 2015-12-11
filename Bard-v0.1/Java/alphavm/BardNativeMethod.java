package alphavm;

abstract public class BardNativeMethod
{
  public String type_name;
  public String signature;

  abstract public void execute( BardProcessor processor );

  public String toString()
  {
    return type_name + "::" + signature;
  }
}

