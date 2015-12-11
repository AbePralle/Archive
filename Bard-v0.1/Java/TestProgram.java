import vm.*;

public class TestProgram extends BardProgram
{
  static public void main( String[] args )
  {
    new TestProgram().launch();
  }

  public TestProgram()
  {
    init();
  }
  
  public void define_id_table()
  {
    vm.id_table.configure(
      new String[]
      {
        "init_object().Object",
        "String().String",
        "init_object().String",
        "init_object().Real",
        "init_object().Integer",
        "init_object().Character",
        "init_object().Logical",
        "init_object().RealProperty",
        "init_object().IntegerProperty",
        "init_object().CharacterProperty",
        "init_object().LogicalProperty",
        "init_object().Variant",
        "init_object().null",
        "init_object().RealObject",
        "init_object().IntegerObject",
        "init_object().CharacterObject",
        "init_object().LogicalObject",
        "init_object().Integer[]",
        "init_object().Character[]",
        "init_object().Byte[]",
        "init_object().Value",
        "init_object().Global",
        "init_object().Collection",
        "init_object().List",
        "init_object().Table",
        "init().Main",
        "init_object().Main",
        "init(Real).Number<<Real,Integer>>",
        "test().Integer",
        "Integer().Integer",
        "Real().Real",
        "init_object().Number<<Real,Integer>>",
        "init(Integer).Number<<Integer,Integer>>",
        "init_object().Number<<Integer,Integer>>",
        "init(Integer).Number<<Integer,Real>>",
        "test().Real",
        "init_object().Number<<Integer,Real>>",
        "Object",
        "String",
        "Real",
        "Integer",
        "Character",
        "Logical",
        "RealProperty",
        "IntegerProperty",
        "CharacterProperty",
        "LogicalProperty",
        "Variant",
        "null",
        "RealObject",
        "IntegerObject",
        "CharacterObject",
        "LogicalObject",
        "Integer[]",
        "Character[]",
        "Byte[]",
        "Value",
        "Global",
        "Collection",
        "List",
        "Table",
        "Main",
        "Number<<Real,Integer>>",
        "value",
        "Number<<Integer,Integer>>",
        "Number<<Integer,Real>>"
      }
    );
  }

  public void define_types()
  {
    define_type( "Object", 2, 0 );
    define_type( "String", 0, 0 );
    define_type( "Real", 3, 0 );
    define_type( "Integer", 3, 0 );
    define_type( "Character", 3, 0 );
    define_type( "Logical", 3, 0 );
    define_type( "RealProperty", 0, 0 );
    define_type( "IntegerProperty", 0, 0 );
    define_type( "CharacterProperty", 0, 0 );
    define_type( "LogicalProperty", 0, 0 );
    define_type( "Variant", 0, 0 );
    define_type( "null", 0, 0 );
    define_type( "RealObject", 0, 0 );
    define_type( "IntegerObject", 0, 0 );
    define_type( "CharacterObject", 0, 0 );
    define_type( "LogicalObject", 0, 0 );
    define_type( "Integer[]", 0, 0 );
    define_type( "Character[]", 0, 0 );
    define_type( "Byte[]", 0, 0 );
    define_type( "Value", 0, 0 );
    define_type( "Global", 0, 0 );
    define_type( "Collection", 0, 0 );
    define_type( "List", 0, 0 );
    define_type( "Table", 0, 0 );
    define_type( "Main", 0, 0 );
    define_type( "Number<<Real,Integer>>", 0, 1 );
    define_type( "Number<<Integer,Integer>>", 0, 1 );
    define_type( "Number<<Integer,Real>>", 0, 1 );

    add_base_type( "String", "Object" );
    add_base_type( "RealProperty", "Object" );
    add_base_type( "IntegerProperty", "Object" );
    add_base_type( "CharacterProperty", "Object" );
    add_base_type( "LogicalProperty", "Object" );
    add_base_type( "Variant", "Object" );
    add_base_type( "null", "Object" );
    add_base_type( "RealObject", "Object" );
    add_base_type( "IntegerObject", "Object" );
    add_base_type( "CharacterObject", "Object" );
    add_base_type( "LogicalObject", "Object" );
    add_base_type( "Integer[]", "Object" );
    add_base_type( "Character[]", "Object" );
    add_base_type( "Byte[]", "Object" );
    add_base_type( "Value", "Object" );
    add_base_type( "Global", "Object" );
    add_base_type( "Collection", "Object" );
    add_base_type( "List", "Object" );
    add_base_type( "Table", "Object" );
    add_base_type( "Main", "Object" );
    add_base_type( "Number<<Real,Integer>>", "Object" );
    add_property( "Number<<Real,Integer>>", 0, "value", "Real" );
    add_base_type( "Number<<Integer,Integer>>", "Object" );
    add_property( "Number<<Integer,Integer>>", 0, "value", "Integer" );
    add_base_type( "Number<<Integer,Real>>", "Object" );
    add_property( "Number<<Integer,Real>>", 0, "value", "Integer" );
  }

  public void define_methods()
  {
    vm.type_String = vm.must_find_type( "String" );
    add_method( "Object", "init_object", 0, 0, 0, "Object", code, 0, 0 );
    add_method( "String", "init_object", 0, 0, 0, "String", code, 2, 0 );
    add_method( "String", "String", 0, 0, 0, "String", code, 4, 0 );
    add_method( "Real", "init_object", 0, 0, 0, "Real", code, 7, 0 );
    add_method( "Integer", "init_object", 0, 0, 0, "Integer", code, 9, 0 );
    add_method( "Character", "init_object", 0, 0, 0, "Character", code, 11, 0 );
    add_method( "Logical", "init_object", 0, 0, 0, "Logical", code, 13, 0 );
    add_method( "RealProperty", "init_object", 0, 0, 0, "RealProperty", code, 15, 0 );
    add_method( "IntegerProperty", "init_object", 0, 0, 0, "IntegerProperty", code, 17, 0 );
    add_method( "CharacterProperty", "init_object", 0, 0, 0, "CharacterProperty", code, 19, 0 );
    add_method( "LogicalProperty", "init_object", 0, 0, 0, "LogicalProperty", code, 21, 0 );
    add_method( "Variant", "init_object", 0, 0, 0, "Variant", code, 23, 0 );
    add_method( "null", "init_object", 0, 0, 0, "null", code, 25, 0 );
    add_method( "RealObject", "init_object", 0, 0, 0, "RealObject", code, 27, 0 );
    add_method( "IntegerObject", "init_object", 0, 0, 0, "IntegerObject", code, 29, 0 );
    add_method( "CharacterObject", "init_object", 0, 0, 0, "CharacterObject", code, 31, 0 );
    add_method( "LogicalObject", "init_object", 0, 0, 0, "LogicalObject", code, 33, 0 );
    add_method( "Integer[]", "init_object", 0, 0, 0, "Integer[]", code, 35, 0 );
    add_method( "Character[]", "init_object", 0, 0, 0, "Character[]", code, 37, 0 );
    add_method( "Byte[]", "init_object", 0, 0, 0, "Byte[]", code, 39, 0 );
    add_method( "Value", "init_object", 0, 0, 0, "Value", code, 41, 0 );
    add_method( "Global", "init_object", 0, 0, 0, "Global", code, 43, 0 );
    add_method( "Collection", "init_object", 0, 0, 0, "Collection", code, 45, 0 );
    add_method( "List", "init_object", 0, 0, 0, "List", code, 47, 0 );
    add_method( "Table", "init_object", 0, 0, 0, "Table", code, 49, 0 );
    add_method( "Main", "init_object", 0, 0, 0, "Main", code, 51, 0 );
    add_method( "Main", "init", 0, 0, 0, "Main", code, 53, 1 );
    add_String_literal( 0, vm.create_String("Hello World") );
    working_method.Real_literals = to_double_array( new long[]{ 0x4014000000000000L, 0x4014000000000000L } );
    add_method( "Number<<Real,Integer>>", "init_object", 0, 0, 0, "Number<<Real,Integer>>", code, 105, 0 );
    add_method( "Number<<Real,Integer>>", "init", 0, 1, 1, "Number<<Real,Integer>>", code, 107, 0 );
    add_parameter( 0, "_value", "Real" );
    add_method( "Number<<Real,Integer>>", "test", 0, 0, 0, "Integer", code, 114, 0 );
    add_method( "Number<<Real,Integer>>", "Integer", 0, 0, 0, "Integer", code, 120, 0 );
    add_method( "Number<<Real,Integer>>", "Real", 0, 0, 0, "Real", code, 126, 0 );
    add_method( "Number<<Integer,Integer>>", "init_object", 0, 0, 0, "Number<<Integer,Integer>>", code, 131, 0 );
    add_method( "Number<<Integer,Integer>>", "init", 0, 1, 1, "Number<<Integer,Integer>>", code, 133, 0 );
    add_parameter( 0, "_value", "Integer" );
    add_method( "Number<<Integer,Integer>>", "test", 0, 0, 0, "Integer", code, 140, 0 );
    add_method( "Number<<Integer,Integer>>", "Integer", 0, 0, 0, "Integer", code, 145, 0 );
    add_method( "Number<<Integer,Integer>>", "Real", 0, 0, 0, "Real", code, 150, 0 );
    add_method( "Number<<Integer,Real>>", "init_object", 0, 0, 0, "Number<<Integer,Real>>", code, 156, 0 );
    add_method( "Number<<Integer,Real>>", "init", 0, 1, 1, "Number<<Integer,Real>>", code, 158, 0 );
    add_parameter( 0, "_value", "Integer" );
    add_method( "Number<<Integer,Real>>", "test", 0, 0, 0, "Real", code, 165, 0 );
    add_method( "Number<<Integer,Real>>", "Integer", 0, 0, 0, "Integer", code, 171, 0 );
    add_method( "Number<<Integer,Real>>", "Real", 0, 0, 0, "Real", code, 176, 0 );
  }

  public TestProgram init()
  {
    BardMethod.use_return_types_in_method_signatures = true;
    vm = create_vm();
    set_up_code( code_strings, 182 );
    define_id_table();
    define_types();
    define_methods();
    return this;
  }

  public String[] code_strings = new String[]
  {
    "+\013+\013+\013\200\231+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013+\013-\000\200\224\020\200\202\031/\000\200\204\001\033q\000\200\225\020\200\202\031/\001\200\204\001\033\200\204\000\034\200\226\020\200\202\0323\005\200\204\001 r\000\200\226\020\200\202\0333\005\200\204\001\"\200\204\000#\200\225\020+\013+\013+b\377w\000+\013+q\000)\r\200\231+q\000)\r\200\231+q\000\014\200\231+\013+c\377x\000+\013+r\000\r\200\231+r\000\r\200\231+r\000(\014\200\231+\013+c\377x\000+\013+r\000(\014\200\231+r\000\r\200\231+r\000(\014\200\231"
  };
}
