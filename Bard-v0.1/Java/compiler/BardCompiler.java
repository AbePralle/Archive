package compiler;

import vm.BardOp;

import java.util.*;
import java.io.*;

// TODO: never remove types from list for incremental link - messes up singleton index

public class BardCompiler
{
  public BardIDTable id_table = new BardIDTable();

  public ArrayList<BardModule>   modules = new ArrayList<BardModule>();
  public BardModule              immediate_mode_module;

  public ArrayList<BardType>      type_list  = new ArrayList<BardType>();
  public HashMap<String,BardType> type_table = new HashMap<String,BardType>();

  public BardParser   parser = new BardParser(this);
  public BardCodeWriter code_writer = new BardCodeWriter( this );

  public BardType type_Real;
  public BardType type_Integer;
  public BardType type_Character;
  public BardType type_Logical;
  public BardType type_NullObject;

  public BardType type_Real_wrapper;
  public BardType type_Integer_wrapper;
  public BardType type_Character_wrapper;
  public BardType type_Logical_wrapper;

  public BardType type_String;

  public BardType type_Variant;
  public BardType type_Object;
  public BardType type_Value;
  public BardType type_Collection;
  public BardType type_List;
  public BardType type_Table;

  public BardType type_Global;

  public boolean resolved;

  public BardCompiler()
  {
    BardModule core_module = new BardModule( this, "[Internal]" );
    modules.add( core_module );

    int q = BardQualifiers.is_built_in;
    type_Object     = core_module.define_type( "Object", q );
    type_Variant    = core_module.define_type( "Variant", q );

    q = BardQualifiers.is_primitive | BardQualifiers.is_built_in | BardQualifiers.is_native;
    type_Real       = core_module.define_type( "Real", q );
    type_Integer    = core_module.define_type( "Integer", q );
    type_Character       = core_module.define_type( "Character", q );
    type_Logical    = core_module.define_type( "Logical", q );
    type_NullObject = core_module.define_type( "null", q );

    type_Real.variant_type    = BardType.variant_Real;
    type_Integer.variant_type = BardType.variant_Integer;
    type_Character.variant_type    = BardType.variant_Character;
    type_Logical.variant_type = BardType.variant_Logical;
    type_NullObject.variant_type = BardType.variant_Object_null;

    q = BardQualifiers.is_native;
    type_Real_wrapper      = core_module.define_type( "RealObject", q );
    type_Integer_wrapper   = core_module.define_type( "IntegerObject", q );
    type_Character_wrapper      = core_module.define_type( "CharacterObject", q );
    type_Logical_wrapper   = core_module.define_type( "LogicalObject", q );
    type_String               = core_module.define_type( "String", q );

    type_Value      = core_module.define_type( "Value", 0 );
    type_Global     = core_module.define_type( "Global", 0 );
    type_Collection = core_module.define_type( "Collection", 0 );
    type_List       = core_module.define_type( "List", 0 );
    type_Table      = core_module.define_type( "Table", 0 );
  }

  public BardType define_type( String name, int qualifiers )
  {
    BardType type = get_type_reference( name );
    type.reset();
    type.defined  = true;
    type.qualifiers = qualifiers;
    return type;
  }


  public BardType get_type_reference( String name )
  {
    BardType type = type_table.get( name );
    if (type != null) return type;

    type = new BardType( this, this.id_table.consolidate(name) );
    type_table.put( name, type );
    type_list.add( type );
    return type;
  }

  public boolean compile( String filepath )
  {
    try
    {
      parser.parse( filepath );
      compile();
      return true;
    }
    catch (BardError err)
    {
      System.out.println( err );
      return false;
    }
    catch (Exception err)
    {
      err.printStackTrace();
      return false;
    }
  }

  public void compile()
  {
    for (int i=0; i<type_list.size(); ++i) type_list.get(i).index = i;

    for (BardType type : type_list) type.organize( type );
    for (BardType type : type_list) type.resolve();

    for (BardType type : type_list) type.compile( code_writer );
  }

  public boolean export( String filename )
  {
    try
    {
      BardIntegerList code_buffer = new BardIntegerList();

      FileOutputStream outfile = new FileOutputStream( filename + ".java" );
      PrintStream out = new PrintStream( outfile );

      out.println( "import alphavm.*;" );
      out.println();
      out.println( "public class " + filename + " extends BardProgram" );
      out.println( "{" );
      out.println( "  static public void main( String[] args )" );
      out.println( "  {" );
      out.println( "    new " + filename + "().launch();" );
      out.println( "  }" );
      out.println();
      out.println( "  public " + filename + "()" );
      out.println( "  {" );
      out.println( "    init();" );
      out.println( "  }" );
      out.println();
      out.println( "  public void define_id_table()" );
      out.println( "  {" );
      out.println( "    vm.id_table.configure(" );
      out.println( "      new String[]" );
      out.println( "      {" );
      boolean first = true;
      for (String st : id_table.name_list)
      {
        if (first) first = false;
        else       out.println(',');
        out.print( "        \"" + st + "\"" );
      }
      out.println();
      out.println( "      }" );
      out.println( "    );" );
      out.println( "  }" );
      out.println();
      out.println( "  public void define_types()" );
      out.println( "  {" );
      out.println( "    // Type definitions" );
      for (BardType type : type_list)
      {
        if ( !type.defined ) throw new BardError( "Type " + type.name + " was referenced but never defined." );
        int property_count = type.properties.size();
        out.println( "    define_type( \"" + type.name + "\", " + type.qualifiers + ", " + property_count + ");" );
      }
      out.println();
      out.println( "    // Base type and property declarations." );
      out.println( "    BardType type;" );
      for (BardType type : type_list)
      {
        if (type.base_types.size() > 0 || type.properties.size() > 0)
        {
          out.println( "    type = vm.must_find_type( \"" + type.name + "\" );" );
          for (BardType base_type : type.base_types)
          {
            out.println( "    add_base_type( type, \"" + base_type.name + "\" );" );
          }
          int property_count = type.properties.size();
          for (int i=0; i<property_count; ++i)
          {
            BardProperty p = type.properties.get(i);
            out.println( "    add_property( type, " + i + ", \"" + p.name + "\", \"" + p.type.name + "\" );" );
          }
        }
      }
      out.println( "  }" );
      out.println();
      out.println( "  public void define_methods()" );
      out.println( "  {" );
      out.println( "    BardType type_String = vm.must_find_type(\"String\");" );
      out.println( "    BardMethod m;" );
      for (BardType type : type_list)
      {
        for (BardMethod m : type.methods)
        {
          if (m.type_context == type)
          {
            out.print( "    m = add_method( \""+type.name+"\", \""+m.name+"\", "+ m.qualifiers+", "+m.parameter_count + ", " +m.local_count + "," );
            if (m.return_type == null) out.print( "null" );
            else                       out.print( "\"" + m.return_type.name + "\"" );

            if (m.code != null)
            {
              out.print( ", code, " + (code_buffer.count-1) );
              code_buffer.add( m.code );
            }

            out.println( " );" );

            // locals
            for (int i=0; i<m.locals.size(); ++i)
            {
              BardLocal v = m.locals.get(i);
              if (i < m.parameters.size()) out.print( "    add_parameter" );
              else                         out.print( "    add_local" );
              out.println( "( m, "+i+", \""+v.name+"\", \""+v.type.name+"\" );" );
            }

            if (m.String_literals != null)
            {
              out.print( "    m.String_literals = new BardString["+m.String_literals.length+"];" );
              for (int j=0; j<m.String_literals.length; ++j)
              {
                String st = m.String_literals[j];
                out.print( "    m.String_literals[" + j + "] = new BardString( type_String, " );
                if (st == null)
                {
                  out.print( "null" );
                }
                else
                {
                  out.print( '"' );
                  for (int i=0; i<st.length(); ++i)
                  {
                    char ch = st.charAt(i);
                    if (ch >= 32 && ch <= 126)
                    {
                      switch (ch)
                      {
                        case '\\':
                          out.print( "\\\\" );
                          break;
                        case '"':
                          out.print( "\\\"" );
                          break;
                        default:
                          out.print( ch );
                      }
                    }
                    else
                    {
                      if (ch <= 255)
                      {
                        out.print( '\\' );
                        out.print( ch / 64 );
                        out.print( (ch % 64) / 8 );
                        out.print( ch % 8 );
                      }
                      else
                      {
                        throw new RuntimeException( "TODO: print unicode escape." );
                      }
                    }
                  }
                  out.print( '"' );
                }
                out.println( " );" );
              }
            }
            if (m.Real_literals != null)
            {
              out.print( "    m.Real_literals = to_double_array( new long[]{ " );
              first = true;
              for (double n : m.Real_literals)
              {
                if (first) first = false;
                else out.print(',');
                out.print( "0x" + Long.toString( Double.doubleToLongBits(n), 16 ) + "L" );
              }
              out.println( " } );" );
            }
          }
        }
        //out.println( "    define_type( \"" + type.name + "\", " + type.qualifiers + ");" );
      }
      out.println( "  }" );
      out.println();

      // Codec Test
      //code_buffer.clear();
      //code_buffer.add( 0x80000000 );
      //code_buffer.add( -65 );
      //code_buffer.add( -64 );
      //code_buffer.add( -1 );
      //code_buffer.add( 0 );
      //code_buffer.add( 1 );
      //code_buffer.add( 127 );
      //code_buffer.add( 128 );
      //code_buffer.add( 255 );
      //code_buffer.add( 256 );
      //code_buffer.add( 0xfFF );
      //code_buffer.add( 0x1000 );
      //code_buffer.add( 0xfFFFF );
      //code_buffer.add( 0x100000 );
      //code_buffer.add( 0xfFFFFff );
      //code_buffer.add( 0x10000000);
      //code_buffer.add( 0x7FFFffff );

      out.println( "  public " + filename + " init()" );
      out.println( "  {" );
      out.println( "    vm = create_vm();" );
      out.println( "    set_up_code( code_strings, " + code_buffer.count + " );" );
      out.println( "    define_id_table();" );
      out.println( "    define_types();" );
      out.println( "    define_methods();" );
      out.println( "    return this;" );
      out.println( "  }" );
      out.println();

      // Code buffer
      out.println( "  public String[] code_strings = new String[]" );
      out.println( "  {" );

      first = true;
      for (int i=0; i<code_buffer.count; )
      {
        if (first) first = false;
        else       out.println( "," );
        out.print( "    \"" );
        for (int j=0; j<1024; ++j,++i)
        {
          if (i == code_buffer.count) break;
          out.print( to_byte_String(code_buffer.get(i)) );
        }
        out.print( "\"" );
      }
      out.println();
      out.println( "  };" );

      out.println( "}" );

      outfile.close();
      return true;
    }
    catch (IOException err)
    {
      System.out.println( "Error writing " + filename + ".java" );
      return false;
    }
  }

  public String to_byte_String( int n )
  {
    if (n >= -64 && n <= 127)
    {
      // %01111111 or %11xxxxxx
      return to_octal_String( n );
    }
    else if (n >= 128 && n <= 0xfff)
    {
      // %1000xxxx yyyyyyyy
      return to_octal_String(0x80 | (n>>8)) + to_octal_String(n);
    }
    else if (n >= 0x1000 && n <= 0xfFFFF)
    {
      // %1001xxxx yyyyyyyy zzzzzzzz
      return to_octal_String(0x90 | (n>>16)) + to_octal_String(n>>8) + to_octal_String(n);
    }
    else if (n >= 0x100000 && n <= 0xfFFFFff)
    {
      // %1010xxxx yyyyyyyy zzzzzzzz aaaaaaaa
      return to_octal_String(0xa0 | (n>>24)) + to_octal_String(n>>16) + to_octal_String(n>>8) + to_octal_String(n);
    }
    else
    {
      // %10110000 xxxxxxxx yyyyyyyy zzzzzzzz aaaaaaaa
      return to_octal_String(0xb0) + to_octal_String(n>>24) + to_octal_String(n>>16) + to_octal_String(n>>8) + to_octal_String(n);
    }
  }

  public String to_octal_String( int n )
  {
    n &= 255;
    if (n == '\\') return "\\\\";
    if (n == '"') return "\\\"";
    if (n >= 32 && n <= 126) return "" + (char) n;
    return "\\" + (n>>6) + "" + ((n>>3)&7) + "" + (n&7);
  }

  public BardType find_type( String name )
  {
    return type_table.get( name );
  }
}

