function BardMethod( vm )
{
  this.vm = vm;
}

BardMethod.prototype.init = function()
{
  this.call_signature = this.name + "(";
  for (var i=0; i<this.parameter_count; ++i)
  {
    if (i > 0) this.call_signature += ",";
    this.call_signature += this.local_types[i].name;
  }
  this.call_signature += ")";

  this.signature = this.call_signature;
  if (this.return_type)
  {
    this.signature += "->" + this.return_type.name;
  }
};

BardMethod.prototype.organize = function()
{
  this.parameter_slot_count = 0;
  this.local_slot_count = 0;

  for (var i=0; i<this.parameter_count; ++i) this.parameter_slot_count += this.local_types[i].stack_slots;
  // TODO
};

BardMethod.prototype.is_native = function()
{
  return (this.attributes & BardAttributes.NATIVE) != 0;
};

