package alphavm;

public class BardProperty
{
  public String   name;
  public BardType type;
  public BardType type_context;

  public int      property_id;
  public int      index;

  public BardProperty( BardType type_context, String name )
  {
    this.type_context = type_context;
    this.name = name;
    property_id = type_context.vm.id_table.get_id( name );
  }

  public BardObject create_slot_value()
  {
    BardVM vm = type.vm;
    if (type.is_Real())         return vm.type_Real_wrapper.create_object();
    else if (type.is_Integer()) return vm.type_Integer_wrapper.create_object();
    else if (type.is_Character())    return vm.type_Character_wrapper.create_object();
    else if (type.is_Logical()) return vm.type_Logical_wrapper.create_object();
    else                        return null;
  }

}

