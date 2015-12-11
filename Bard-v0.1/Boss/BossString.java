public class BossString
{
  static public void access( String value, BossVM vm, String name, int arg_count )
  {
    switch (arg_count)
    {
      case 0:
        if (name.equals("count"))
        {
          vm.pushInteger( value.length() );
          return;
        }
        else if (name.equals("Integer"))
        {
          vm.pushInteger( toInteger(value) );
          return;
        }
        break;

      case 1:
        if (name.equals("index_of"))
        {
          String st = vm.argAsString(1);
          vm.pushInteger( value.indexOf(st) );
          return;
        }
        break;

      case 2:
        if (name.equals("index_of"))
        {
          String st = vm.argAsString(1);
          int    i1 = vm.argAsInteger(2);
          vm.pushInteger( value.indexOf(st,i1) );
          return;
        }
        break;
    }
    vm.pushNull();
  }

  static public double toReal( String st )
  { 
    try
    {
      return Double.parseDouble( st );
    }
    catch (NumberFormatException err)
    {
      return 0.0;
    }
  }

  static public int toInteger( String st )
  {
    try
    {
      return Integer.parseInt( st );
    }
    catch (NumberFormatException err)
    {
      return 0;
    }
  }

  static public boolean toLogical( String st )
  {
    String lowercase = st.toLowerCase();
    if (lowercase.equals("true"))  return true;
    if (lowercase.equals("false")) return false;
    if (lowercase.equals("yes"))   return true;
    if (lowercase.equals("no"))    return false;
    return (toInteger(st) != 0);
  }
}

