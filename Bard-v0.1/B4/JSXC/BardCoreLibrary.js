//var Bard_compound_stack = new Array(512);
//var Bard_compound_sp = 512;

function Bard_mod_real( a, b )
{
  var q = a / b;
  return a - (Math.floor(q) * b);
}

function Bard_mod_integer( a, b )
{
  if (a == 0 || b == 0) return 0;

  if (b == 1) return 0;

  if ((a ^ b) < 0)
  {
    var r = a % b;
    return (!!r) ? (r+b) : r;
  }
  else
  {
    return a % b;
  }
}

function Bard_throw_missing_return()
{
  alert( "Missing 'return'." );
}

function BardObject__create_instance()
{
  alert( "TODO: BardObject__create_instance() " );
}

function BardString__create()
{
  return "";
}

function BardString__create__List_of_Character( THIS, local_characters )
{
  var st = "";
  var count = local_characters.p_count;
  var data  = local_characters.p_data;
  for (var i=0; i<count; ++i) st += String.fromCharCode( data[i] );
  return st;
}

function BardString__count()
{
  alert( "String count" );
}

function BardString__get__Integer( local_index )
{
  alert( "String get" );
}

function BardString__hash_code( THIS )
{
  var count = THIS.length;
  var hash_code = 0;
  for (var i=0; i<count; ++i)
  {
    hash_code = hash_code*7 + THIS.charCodeAt(i);
  }
  return hash_code;
}

function BardGenericArray__clear_references__Integer_Integer( THIS, i1, i2 )
{
  if (THIS.type && THIS.type.name == "Array<<Object>>")
  {
    for (var i=i1; i<=i2; ++i) THIS[i] = null;
  }
}

function BardGenericArray__shift__Integer_Integer_Integer( THIS, i1, i2, delta )
{
  var endex = THIS.length - 1;
  if (i1 < 0) i1 = 0;
  else if (i1 > endex) i1 = endex;
  if (i2 < 0) i2 = 0;
  else if (i2 > endex) i2 = endex;

  var j1 = i1 + delta;
  var j2 = i2 + delta;
  if (j1 < 0) j1 = 0;
  else if (j1 > endex) j1 = endex;
  if (j2 < 0) j2 = 0;
  else if (j2 > endex) j2 = endex;

  if (j1 > j2) return;

  i1 = j1 - delta;
  i2 = j2 - delta;

  var count = (j2 - j1) + 1;

  if (delta > 0)
  {
    ++j2;
    ++i2;
    while (--count >= 0)
    {
      THIS[--j2] = THIS[--i2];
    }
  }
  else if (delta < 0)
  {
    --j1;
    --i1;
    while (--count >= 0)
    {
      THIS[++j1] = THIS[++i1];
    }
  }
}

var BardConsole_string_buffer = "";

function BardConsole__write__Character( local_value )
{
  if (local_value == 10)
  {
    window.console.log( BardConsole_string_buffer );
    BardConsole_string_buffer = "";
  }
  else
  {
    BardConsole_string_buffer += String.fromCharCode( local_value );
  }
}

function BardConsole__write__String( local_value )
{
  BardConsole_string_buffer += local_value;
  window.console.log( BardConsole_string_buffer );
  BardConsole_string_buffer = "";
}

function BardMath__floor__Real( local_value )
{
  return Math.floor( local_value )
}

function BardMath__sqrt__Real( local_value )
{
  return Math.sqrt( local_value )
}

function BardTime__current()
{
  return Date.now() / 1000.0;
}

