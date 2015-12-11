function BardProperty( name, type )
{
  this.name = name;
  this.type = type;
}

BardProperty.prototype.init_default = function( data, index )
{
  var this_type = this.type;
  if (this_type.is_reference())
  {
    data[index] = null;
  }
  else if (this_type == this_type.vm.type_Logical)
  {
    data[index] = false;
  }
  else if (this_type.is_primitive())
  {
    data[index] = 0;
  }
  else
  {
    // compound
    for (var i=0; i<this_type.properties.length; ++i)
    {
      var element = this_type.properties[i];
      element.init_default( data, index );
      index += element.type.slot_count;
    }
  }
}
