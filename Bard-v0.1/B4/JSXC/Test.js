//-----------------------------------------------------------------------------
// Method Definitions
//-----------------------------------------------------------------------------
// NATIVE function BardObject__create_instance()

function BardObject__init()
{
  return this;
}

function BardObject__hash_code()
{
  BardSingletons.singleton_Global.m_println__String( "ERROR - hash_code not defined for type." );
  return 0;
}

function BardObject__equals__Object( local_other )
{
  return false;
}

function BardObject__to_Real()
{
  return 0.0;
}

function BardObject__to_Integer()
{
  return (this.m_to_Real()|0);
}

function BardObject__to_Character()
{
  return this.m_to_Integer();
}

function BardObject__to_Byte()
{
  return (this.m_to_Integer()&255);
}

function BardObject__to_Logical()
{
  return !!this.m_to_Integer();
}

function BardObject__to_String()
{
  return "(an object)";
}

function BardObject__init_settings()
{
}

function BardObject__init_defaults()
{
  return this;
}

function BardReal__init_settings( THIS )
{
}

function BardReal__init_defaults( THIS )
{
  return this;
}

function BardInteger__hash_code( THIS )
{
  return THIS;
}

function BardInteger__to_hex_string__Integer( THIS, local_digits )
{
  BardSingletons.singleton_Global
  BardGlobalSettings.s_global_string_buffer.m_clear();
  var local_value;
  local_value = THIS;
  while ((local_digits > 0))
  {
    var local_next_digit;
    local_next_digit = (local_value & 15);
    local_value = (local_value >>> 4);
    if ((local_next_digit < 10))
    {
      BardSingletons.singleton_Global
      BardGlobalSettings.s_global_string_buffer.m_print__Integer( local_next_digit );
    }
    else
    {
      BardSingletons.singleton_Global
      BardGlobalSettings.s_global_string_buffer.m_print__Character( ((local_next_digit - 10) + 65) );
    }

    --local_digits;
  }

  BardSingletons.singleton_Global
  BardGlobalSettings.s_global_string_buffer.m_reverse();
  return BardSingletons.singleton_Global
  BardGlobalSettings.s_global_string_buffer.m_to_String();
}

function BardInteger__init_settings( THIS )
{
}

function BardInteger__init_defaults( THIS )
{
  return this;
}

function BardCharacter__is_alphanumeric( THIS )
{
  return ((((THIS >= 97) && (THIS <= 122)) || ((THIS >= 65) && (THIS <= 90))) || ((THIS >= 48) && (THIS <= 57)));
}

function BardCharacter__is_letter( THIS )
{
  return (((THIS >= 97) && (THIS <= 122)) || ((THIS >= 65) && (THIS <= 90)));
}

function BardCharacter__to_value( THIS )
{
  if (((THIS >= 48) && (THIS <= 57)))
  {
    return (THIS - 48);
  }

  if (((THIS >= 97) && (THIS <= 122)))
  {
    return ((THIS - 97) + 10);
  }

  if (((THIS >= 65) && (THIS <= 90)))
  {
    return ((THIS - 65) + 10);
  }

  return -1;
}

function BardCharacter__init_settings( THIS )
{
}

function BardCharacter__init_defaults( THIS )
{
  return this;
}

function BardByte__init_settings( THIS )
{
}

function BardByte__init_defaults( THIS )
{
  return this;
}

function BardLogical__init_settings( THIS )
{
}

function BardLogical__init_defaults( THIS )
{
  return this;
}

// NATIVE function BardString__create( THIS )

// NATIVE function BardString__create__ByteList( THIS, local_bytes )

// NATIVE function BardString__create__CharacterList( THIS, local_characters )

function BardString__after_first__Character( THIS, local_ch )
{
  var local_i;
  local_i = BardString__find__Character( THIS, local_ch );
  if ((local_i == -1))
  {
    return "";
  }

  return BardString__substring__Integer( THIS, (local_i + 1) );
}

function BardString__after_first__String( THIS, local_st )
{
  var local_i;
  local_i = BardString__find__String( THIS, local_st );
  if ((local_i == -1))
  {
    return "";
  }

  return BardString__substring__Integer( THIS, (local_i + local_st.length) );
}

function BardString__before_first__Character( THIS, local_ch )
{
  var local_i;
  local_i = BardString__find__Character( THIS, local_ch );
  if ((local_i == -1))
  {
    return THIS;
  }

  return BardString__substring__Integer_Integer( THIS, 0, (local_i - 1) );
}

function BardString__before_first__String( THIS, local_st )
{
  var local_i;
  local_i = BardString__find__String( THIS, local_st );
  if ((local_i == -1))
  {
    return THIS;
  }

  return BardString__substring__Integer_Integer( THIS, 0, (local_i - 1) );
}

function BardString__begins_with__Character( THIS, local_ch )
{
  return (!!THIS.length && (THIS.charCodeAt( 0 ) == local_ch));
}

function BardString__begins_with__String( THIS, local_other )
{
  var local_other_count;
  local_other_count = local_other.length;
  return ((THIS.length >= local_other_count) && BardString__substring_equals__Integer_Integer_String( THIS, 0, (local_other_count - 1), local_other ));
}

function BardString__compare_to__String( THIS, local_other )
{
  if ((THIS == local_other))
  {
    return 0;
  }

  var local_limit;
  local_limit = local_other.length;
  if ((THIS.length < local_limit))
  {
    local_limit = THIS.length;
  }

  var local_i;
  local_i = 0;
  while ((local_i < local_limit))
  {
    var local_ch1;
    local_ch1 = THIS.charCodeAt( local_i );
    var local_ch2;
    local_ch2 = local_other.charCodeAt( local_i );
    if ((local_ch1 != local_ch2))
    {
      if ((local_ch1 < local_ch2))
      {
        return -1;
      }
      else
      {
        return 1;
      }

    }

    ++local_i;
  }

  if ((THIS.length == local_other.length))
  {
    return 0;
  }

  if ((THIS.length < local_other.length))
  {
    return -1;
  }

  return 1;
}

function BardString__clone( THIS )
{
  return THIS;
}

function BardString__contains__Character( THIS, local_ch )
{
  return (BardString__find__Character( THIS, local_ch ) != -1);
}

function BardString__contains__String( THIS, local_substr )
{
  return (BardString__find__String( THIS, local_substr ) != -1);
}

// NATIVE function BardString__count( THIS )

function BardString__equals__Object( THIS, local_other )
{
  return BardString__equals__String( THIS, local_other );
}

function BardString__equals__String( THIS, local_other )
{
  return (BardString__compare_to__String( THIS, local_other ) == 0);
}

function BardString__ends_with__Character( THIS, local_ch )
{
  var local_this_count;
  local_this_count = THIS.length;
  return (!!local_this_count && (THIS.charCodeAt( (local_this_count - 1) ) == local_ch));
}

function BardString__ends_with__String( THIS, local_other )
{
  var local_this_count;
  local_this_count = THIS.length;
  var local_other_count;
  local_other_count = local_other.length;
  if ((local_other_count > local_this_count))
  {
    return false;
  }

  return BardString__substring_equals__Integer_Integer_String( THIS, (local_this_count - local_other_count), (local_this_count - 1), local_other );
}

function BardString__find__Character( THIS, local_ch )
{
  return BardString__find__Character_Integer( THIS, local_ch, 0 );
}

function BardString__find__Character_Integer( THIS, local_ch, local_i1 )
{
  var local_i;
  local_i = local_i1;
  var local_len;
  local_len = THIS.length;
  while ((local_i < local_len))
  {
    if ((THIS.charCodeAt( local_i ) == local_ch))
    {
      return local_i;
    }

    ++local_i;
  }

  return -1;
}

function BardString__find__String( THIS, local_other )
{
  return BardString__find__String_Integer( THIS, local_other, 0 );
}

function BardString__find__String_Integer( THIS, local_other, local_i1 )
{
  var local_other_count;
  local_other_count = local_other.length;
  if ((local_other_count == 0))
  {
    return -1;
  }

  if ((local_other_count > THIS.length))
  {
    return -1;
  }

  var local_i;
  local_i = local_i1;
  var local_limit;
  local_limit = ((THIS.length - local_other_count) + 1);
  var local_first_ch;
  local_first_ch = local_other.charCodeAt( 0 );
  while ((local_i < local_limit))
  {
    if (((local_first_ch == THIS.charCodeAt( local_i )) && BardString__substring_equals__Integer_Integer_String( THIS, local_i, (local_i + (local_other_count - 1)), local_other )))
    {
      return local_i;
    }

    ++local_i;
  }

  return -1;
}

function BardString__find_last__Character( THIS, local_ch )
{
  return BardString__find_last__Character_Integer( THIS, local_ch, (THIS.length - 1) );
}

function BardString__find_last__Character_Integer( THIS, local_ch, local_starting_index )
{
  var local_i;
  local_i = local_starting_index;
  while ((local_i >= 0))
  {
    if ((THIS.charCodeAt( local_i ) == local_ch))
    {
      return local_i;
    }

    --local_i;
  }

  return -1;
}

function BardString__find_last__String( THIS, local_other )
{
  return BardString__find_last__String_Integer( THIS, local_other, (THIS.length - local_other.length) );
}

function BardString__find_last__String_Integer( THIS, local_other, local_starting_index )
{
  var local_i;
  local_i = local_starting_index;
  var local_end_offset;
  local_end_offset = (local_other.length - 1);
  while ((local_i >= 0))
  {
    if (BardString__substring_equals__Integer_Integer_String( THIS, local_i, (local_i + local_end_offset), local_other ))
    {
      return local_i;
    }

    --local_i;
  }

  return -1;
}

function BardString__from_first__Character( THIS, local_ch )
{
  var local_i;
  local_i = BardString__find__Character( THIS, local_ch );
  if ((local_i == -1))
  {
    return "";
  }

  return BardString__substring__Integer( THIS, local_i );
}

function BardString__from_first__String( THIS, local_st )
{
  var local_i;
  local_i = BardString__find__String( THIS, local_st );
  if ((local_i == -1))
  {
    return "";
  }

  return BardString__substring__Integer( THIS, local_i );
}

// NATIVE function BardString__get__Integer( THIS, local_index )

// NATIVE function BardString__hash_code( THIS )

function BardString__leftmost__Integer( THIS, local_n )
{
  if ((local_n >= 0))
  {
    return BardString__substring__Integer_Integer( THIS, 0, (local_n - 1) );
  }
  else
  {
    return BardString__substring__Integer_Integer( THIS, 0, ((THIS.length + local_n) - 1) );
  }

  Bard_throw_missing_return();
}

function BardString__operatorPLUSEQUALS__String( THIS, local_rhs )
{
  if ((local_rhs == null))
  {
    local_rhs = "null";
  }

  return BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), (THIS.length + local_rhs.length) ).m_print__String( THIS ).m_print__String( local_rhs ).m_to_String();
}

function BardString__operatorTIMESEQUALS__Integer( THIS, local_copies )
{
  return BardString__operatorTIMES__String_Integer( BardSingletons.singleton_String, THIS, local_copies );
}

function BardString__operatorTIMES__String_Integer( THIS, local_lhs, local_copies )
{
  if ((local_lhs == null))
  {
    local_lhs = "null";
  }

  if ((local_copies == 0))
  {
    return "";
  }

  if ((local_copies == 1))
  {
    return local_lhs;
  }

  var local_buffer;
  local_buffer = BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), (local_lhs.length * local_copies) );
  var local_i;
  local_i = local_copies;
  while ((local_i >= 1))
  {
    local_buffer.m_print__String( local_lhs );
    --local_i;
  }

  return local_buffer.m_to_String();
}

function BardString__print__Integer( THIS, local_value )
{
  return BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), (THIS.length + 11) ).m_print__String( THIS ).m_print__Integer( local_value ).m_to_String();
}

function BardString__print__Logical( THIS, local_value )
{
  return BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), (THIS.length + 5) ).m_print__String( THIS ).m_print__Logical( local_value ).m_to_String();
}

function BardString__print__Real( THIS, local_value )
{
  return BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), (THIS.length + 11) ).m_print__String( THIS ).m_print__Real( local_value ).m_to_String();
}

function BardString__print__Object( THIS, local_other )
{
  if ((local_other == null))
  {
    return "null";
  }
  else
  {
    return BardString__print__String( THIS, local_other.m_to_String() );
  }

  Bard_throw_missing_return();
}

function BardString__print__String( THIS, local_other )
{
  if ((local_other == null))
  {
    local_other = "null";
  }

  return BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), (THIS.length + local_other.length) ).m_print__String( THIS ).m_print__String( local_other ).m_to_String();
}

function BardString__reversed( THIS )
{
  var local_result;
  local_result = BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), THIS.length ).m_print__String( THIS );
  local_result.m_reverse();
  return local_result.m_to_String();
}

function BardString__rightmost__Integer( THIS, local_n )
{
  var local_this_count;
  local_this_count = THIS.length;
  if ((local_n < 0))
  {
    return BardString__substring__Integer_Integer( THIS, -local_n, (local_this_count - 1) );
  }
  else
  {
    return BardString__substring__Integer_Integer( THIS, (local_this_count - local_n), (local_this_count - 1) );
  }

  Bard_throw_missing_return();
}

function BardString__split__Character( THIS, local_separator )
{
  var local_result;
  local_result = BardStringList__init.call( new BardStringList().m_init_defaults() );
  var local_i1;
  local_i1 = 0;
  var local_i2;
  local_i2 = BardString__find__Character_Integer( THIS, local_separator, local_i1 );
  while ((local_i2 != -1))
  {
    local_result.m_add__String( BardString__substring__Integer_Integer( THIS, local_i1, (local_i2 - 1) ) );
    local_i1 = (local_i2 + 1);
    local_i2 = BardString__find__Character_Integer( THIS, local_separator, local_i1 );
  }

  local_result.m_add__String( BardString__substring__Integer( THIS, local_i1 ) );
  return local_result;
}

function BardString__substring__Integer( THIS, local_i1 )
{
  return BardString__substring__Integer_Integer( THIS, local_i1, (THIS.length - 1) );
}

function BardString__substring__Integer_Integer( THIS, local_i1, local_i2 )
{
  var local_this_count;
  local_this_count = THIS.length;
  if ((local_i1 < 0))
  {
    local_i1 = 0;
  }

  if ((local_i2 >= local_this_count))
  {
    local_i2 = (local_this_count - 1);
  }

  if ((local_i1 > local_i2))
  {
    return "";
  }

  if (((local_i1 == 0) && (local_i2 == (local_this_count - 1))))
  {
    return THIS;
  }

  var local_result;
  local_result = BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), ((local_i2 + 1) - local_i1) );
  var local_i;
  local_i = local_i1;
  while ((local_i <= local_i2))
  {
    local_result.m_print__Character( THIS.charCodeAt( local_i ) );
    ++local_i;
  }

  return local_result.m_to_String();
}

function BardString__substring_equals__Integer_Integer_String( THIS, local_i1, local_i2, local_other )
{
  var local_other_count;
  local_other_count = local_other.length;
  var local_this_count;
  local_this_count = THIS.length;
  if ((local_i1 < 0))
  {
    local_i1 = 0;
  }

  if ((local_i2 >= local_this_count))
  {
    local_i2 = (local_this_count - 1);
  }

  if ((local_i1 > local_i2))
  {
    return (local_other.length == 0);
  }

  var local_span;
  local_span = ((local_i2 - local_i1) + 1);
  if ((local_other_count != local_span))
  {
    return false;
  }

  var local_j;
  local_j = 0;
  while ((local_j < local_span))
  {
    if ((THIS.charCodeAt( (local_i1 + local_j) ) != local_other.charCodeAt( local_j )))
    {
      return false;
    }

    ++local_j;
  }

  return true;
}

function BardString__to_String( THIS )
{
  return THIS;
}

function BardString__to_Integer( THIS )
{
  var local_result;
  local_result = 0;
  var local_exp;
  local_exp = (THIS.length - 1);
  {
    var local__index_277;
    var local__collection_278;
    local__index_277 = 0;
    local__collection_278 = THIS;
    while ((local__index_277 < local__collection_278.length))
    {
      var local_digit;
      local_digit = local__collection_278.charCodeAt( local__index_277 );
      ++local__index_277;
      var local_actual_digit;
      local_actual_digit = (local_digit - 48);
      local_result += (local_actual_digit * Math.pow( 10, local_exp ));
      --local_exp;
    }

  }

  return local_result;
}

function BardString__to_uppercase( THIS )
{
  var local_found_lc;
  local_found_lc = false;
  {
    var local__index_281;
    var local__collection_282;
    local__index_281 = 0;
    local__collection_282 = THIS;
    label_283:
    while ((local__index_281 < local__collection_282.length))
    {
      var local_ch;
      local_ch = local__collection_282.charCodeAt( local__index_281 );
      ++local__index_281;
      if (((local_ch >= 97) && (local_ch <= 122)))
      {
        local_found_lc = true;
        break label_283;
      }

    }

  }

  if (!local_found_lc)
  {
    return THIS;
  }

  var local_result;
  local_result = BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), THIS.length );
  {
    var local__index_285;
    var local__collection_286;
    local__index_285 = 0;
    local__collection_286 = THIS;
    while ((local__index_285 < local__collection_286.length))
    {
      var local_ch;
      local_ch = local__collection_286.charCodeAt( local__index_285 );
      ++local__index_285;
      if (((local_ch >= 97) && (local_ch <= 122)))
      {
        local_result.m_print__Character( (local_ch + -32) );
      }
      else
      {
        local_result.m_print__Character( local_ch );
      }

    }

  }

  return local_result.m_to_String();
}

function BardString__to_lowercase( THIS )
{
  var local_found_uc;
  local_found_uc = false;
  {
    var local__index_289;
    var local__collection_290;
    local__index_289 = 0;
    local__collection_290 = THIS;
    label_291:
    while ((local__index_289 < local__collection_290.length))
    {
      var local_ch;
      local_ch = local__collection_290.charCodeAt( local__index_289 );
      ++local__index_289;
      if (((local_ch >= 65) && (local_ch <= 90)))
      {
        local_found_uc = true;
        break label_291;
      }

    }

  }

  if (!local_found_uc)
  {
    return THIS;
  }

  var local_result;
  local_result = BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), THIS.length );
  {
    var local__index_293;
    var local__collection_294;
    local__index_293 = 0;
    local__collection_294 = THIS;
    while ((local__index_293 < local__collection_294.length))
    {
      var local_ch;
      local_ch = local__collection_294.charCodeAt( local__index_293 );
      ++local__index_293;
      if (((local_ch >= 65) && (local_ch <= 90)))
      {
        local_result.m_print__Character( (local_ch + 32) );
      }
      else
      {
        local_result.m_print__Character( local_ch );
      }

    }

  }

  return local_result.m_to_String();
}

function BardString__trim( THIS )
{
  var local_i1;
  local_i1 = 0;
  var local_i2;
  local_i2 = (THIS.length - 1);
  while (((local_i1 <= local_i2) && (THIS.charCodeAt( local_i1 ) <= 32)))
  {
    ++local_i1;
  }

  while (((local_i1 <= local_i2) && (THIS.charCodeAt( local_i2 ) <= 32)))
  {
    --local_i2;
  }

  if ((local_i1 > local_i2))
  {
    return "";
  }

  if (((local_i1 == 0) && (local_i2 == (THIS.length - 1))))
  {
    return THIS;
  }

  return BardString__substring__Integer_Integer( THIS, local_i1, local_i2 );
}

function BardString__is_numeric__Character( THIS, local_ch )
{
  return ((local_ch >= 48) && (local_ch <= 57));
}

function BardString__to_ByteList( THIS )
{
  var local_non_ascii_count;
  local_non_ascii_count = 0;
  {
    var local__index_297;
    var local__collection_298;
    local__index_297 = 0;
    local__collection_298 = THIS;
    while ((local__index_297 < local__collection_298.length))
    {
      var local_ch;
      local_ch = local__collection_298.charCodeAt( local__index_297 );
      ++local__index_297;
      if ((local_ch > 127))
      {
        ++local_non_ascii_count;
      }

    }

  }

  var local_result;
  local_result = BardByteList__init__Integer.call( new BardByteList().m_init_defaults(), (THIS.length + (local_non_ascii_count * 2)) );
  {
    var local__index_309;
    var local__collection_310;
    local__index_309 = 0;
    local__collection_310 = THIS;
    while ((local__index_309 < local__collection_310.length))
    {
      var local_ch;
      local_ch = local__collection_310.charCodeAt( local__index_309 );
      ++local__index_309;
      local_result.m_add__Byte( (local_ch&255) );
    }

  }

  return local_result;
}

function BardString__init_settings( THIS )
{
  BardStringSettings.s_consolidation_table = BardTable_of_String_String__init.call( new BardTable_of_String_String().m_init_defaults() );
}

function BardString__init_defaults( THIS )
{
  return this;
}

function BardStringBuilder__init()
{
  this.m_init__Integer( 40 );
  return this;
}

function BardStringBuilder__to_String()
{
  return BardString__create__CharacterList( BardSingletons.singleton_String, this.p_characters );
}

function BardStringBuilder__init_settings()
{
}

function BardStringBuilder__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardStringBuilder__print__String( local_value )
{
  if ((local_value == null))
  {
    local_value = "null";
  }

  if ((local_value.length == 0))
  {
    return this;
  }

  if (!!this.m_indent())
  {
    {
      var local__index_245;
      var local__collection_246;
      local__index_245 = 0;
      local__collection_246 = local_value;
      while ((local__index_245 < local__collection_246.length))
      {
        var local_ch;
        local_ch = local__collection_246.charCodeAt( local__index_245 );
        ++local__index_245;
        this.m_print__Character( local_ch );
      }

    }

  }
  else
  {
    this.m_write__String( local_value );
  }

  return this;
}

function BardStringBuilder__print__Real( local_value )
{
  return this.m_print__Real_Integer( local_value, 4 );
}

function BardStringBuilder__print__Real_Integer( local_value, local_decimal_places )
{
  this.m_perform_indent();
  var local_i1;
  local_i1 = this.m_count();
  var local_is_negative;
  local_is_negative = false;
  if ((local_value < 0.0))
  {
    local_is_negative = true;
    local_value = -local_value;
  }

  local_value = (local_value * Math.pow( 10.0, local_decimal_places ));
  if ((Bard_mod_real( local_value, 1.0 ) >= 0.5))
  {
    ++local_value;
  }

  local_value = BardSingletons.singleton_Math.m_floor__Real( local_value );
  if ((local_decimal_places > 0))
  {
    var local_decimal_digits;
    local_decimal_digits = 1;
    while ((local_decimal_digits <= local_decimal_places))
    {
      this.m_write__Character( ((Bard_mod_real( local_value, 10.0 )|0) + 48) );
      local_value = BardSingletons.singleton_Math.m_floor__Real( (local_value / 10.0) );
      ++local_decimal_digits;
    }

    this.m_write__Character( 46 );
  }

  if ((local_value >= 1.0))
  {
    while ((local_value >= 1.0))
    {
      this.m_write__Character( ((Bard_mod_real( local_value, 10.0 )|0) + 48) );
      local_value = BardSingletons.singleton_Math.m_floor__Real( (local_value / 10.0) );
    }

  }
  else
  {
    this.m_write__Character( 48 );
  }

  if (local_is_negative)
  {
    this.m_write__Character( 45 );
  }

  this.m_reverse__Integer_Integer( local_i1, (this.m_count() - 1) );
  return this;
}

function BardStringBuilder__print__Integer( local_value )
{
  this.m_perform_indent();
  if ((local_value == 0))
  {
    this.m_write__Character( 48 );
  }
  else if ((local_value == -local_value))
  {
    return this.m_print__String( "-2147483648" );
  }
  else
  {
    var local_i1;
    local_i1 = this.m_count();
    var local_is_negative;
    local_is_negative = false;
    if ((local_value < 0))
    {
      local_is_negative = true;
      local_value = -local_value;
    }

    var local_first;
    local_first = true;
    while ((local_first || (local_value > 0)))
    {
      local_first = false;
      this.m_write__Character( (Bard_mod_integer( local_value, 10 ) + 48) );
      local_value = ((local_value / 10.0)|0);
    }

    if (local_is_negative)
    {
      this.m_write__Character( 45 );
    }

    this.m_reverse__Integer_Integer( local_i1, (this.m_count() - 1) );
  }

  return this;
}

function BardStringBuilder__print__Character( local_value )
{
  if ((local_value == 10))
  {
    this.m_write__Character( 10 );
  }
  else
  {
    this.m_perform_indent();
    this.m_write__Character( local_value );
  }

  return this;
}

function BardStringBuilder__print__Object( local_value )
{
  if (!!local_value)
  {
    return this.m_print__String( local_value.m_to_String() );
  }
  else
  {
    return this.m_print__String( "null" );
  }

  Bard_throw_missing_return();
}

function BardStringBuilder__print__Byte( local_value )
{
  return this.m_print__Integer( local_value );
}

function BardStringBuilder__print__Logical( local_value )
{
  if (local_value)
  {
    return this.m_print__String( "true" );
  }
  else
  {
    return this.m_print__String( "false" );
  }

  Bard_throw_missing_return();
}

function BardStringBuilder__println__Object( local_value )
{
  return this.m_print__Object( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println__String( local_value )
{
  return this.m_print__String( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println__Real( local_value )
{
  return this.m_print__Real( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println__Real_Integer( local_value, local_decimal_places )
{
  return this.m_print__Real( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println__Integer( local_value )
{
  return this.m_print__Integer( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println__Character( local_value )
{
  return this.m_print__Character( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println__Byte( local_value )
{
  return this.m_print__Byte( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println__Logical( local_value )
{
  return this.m_print__Logical( local_value ).m_print__Character( 10 );
}

function BardStringBuilder__println()
{
  return this.m_print__Character( 10 );
}

function BardStringBuilder__init__Integer( local_initial_capacity )
{
  this.p_characters = BardCharacterList__init__Integer.call( new BardCharacterList().m_init_defaults(), local_initial_capacity );
  return this;
}

function BardStringBuilder__init__String( local_initial_content )
{
  this.m_init();
  this.m_print__String( local_initial_content );
  return this;
}

function BardStringBuilder__clone()
{
  return BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), this.p_characters.p_count ).m_print__StringBuilder( this );
}

function BardStringBuilder__count()
{
  return this.p_characters.p_count;
}

function BardStringBuilder__at_newline()
{
  return ((this.p_characters.p_count == 0) || (this.m_get__Integer( -1 ) == 10));
}

function BardStringBuilder__get__Integer( local_index )
{
  return this.p_characters.m_get__Integer( local_index );
}

function BardStringBuilder__indent()
{
  return this.p_indent;
}

function BardStringBuilder__indent__Integer( local_new_indent )
{
  this.p_indent = local_new_indent;
  Bard_throw_missing_return();
}

function BardStringBuilder__pad_with__Character_Integer( local_fill_char, local_fill_count )
{
  var local_i;
  local_i = local_fill_count;
  while ((local_i > 0))
  {
    --local_i;
    this.m_write__Character( local_fill_char );
  }

  return this;
}

function BardStringBuilder__capacity()
{
  return this.p_characters.m_capacity();
}

function BardStringBuilder__clear()
{
  this.p_characters.m_clear();
  this.p_hash_code = 0;
  this.p_indent = 0;
  return this;
}

function BardStringBuilder__last()
{
  return this.m_get__Integer( (this.m_count() - 1) );
}

function BardStringBuilder__perform_indent()
{
  if ((!!this.m_indent() && ((this.p_characters.p_count == 0) || (this.m_get__Integer( -1 ) == 10))))
  {
    var local_i;
    local_i = 1;
    while ((local_i <= this.m_indent()))
    {
      this.p_characters.m_add__Character( 32 );
      ++local_i;
    }

  }

}

function BardStringBuilder__print__StringBuilder( local_value )
{
  if ((local_value == null))
  {
    this.m_print__String( "null" );
    return this;
  }

  if ((local_value.m_count() == 0))
  {
    return this;
  }

  if (!!this.m_indent())
  {
    {
      var local__index_249;
      var local__collection_250;
      local__index_249 = 0;
      local__collection_250 = local_value;
      while ((local__index_249 < local__collection_250.m_count()))
      {
        var local_ch;
        local_ch = local__collection_250.m_get__Integer( local__index_249 );
        ++local__index_249;
        this.m_print__Character( local_ch );
      }

    }

  }
  else
  {
    this.m_write__StringBuilder( local_value );
  }

  return this;
}

function BardStringBuilder__reverse()
{
  this.m_reverse__Integer_Integer( 0, (this.p_characters.p_count - 1) );
}

function BardStringBuilder__reverse__Integer_Integer( local_i1, local_i2 )
{
  var local_i2;
  local_i2 = (this.p_characters.p_count - 1);
  while ((local_i1 < local_i2))
  {
    var local_temp;
    local_temp = this.m_get__Integer( local_i1 );
    this.m_set__Integer_Character( local_i1, this.m_get__Integer( local_i2 ) );
    this.m_set__Integer_Character( local_i2, local_temp );
    ++local_i1;
    --local_i2;
  }

}

function BardStringBuilder__set__Integer_Character( local_index, local_ch )
{
  this.p_characters.m_set__Integer_Character( local_index, local_ch );
  this.m_update_hash_code();
}

function BardStringBuilder__update_hash_code()
{
  var local_code;
  local_code = 0;
  {
    var local__index_253;
    var local__collection_254;
    local__index_253 = 0;
    local__collection_254 = this.p_characters;
    while ((local__index_253 < local__collection_254.p_count))
    {
      var local_ch;
      local_ch = local__collection_254.m_get__Integer( local__index_253 );
      ++local__index_253;
      local_code = ((local_code * 7) + local_ch);
    }

  }

  this.p_hash_code = local_code;
}

function BardStringBuilder__write__Character( local_value )
{
  this.p_characters.m_add__Character( local_value );
  this.p_hash_code = ((this.p_hash_code * 7) + local_value);
  return this;
}

function BardStringBuilder__write__String( local_value )
{
  if ((local_value == null))
  {
    local_value = "null";
  }

  {
    var local__index_257;
    var local__collection_258;
    local__index_257 = 0;
    local__collection_258 = local_value;
    while ((local__index_257 < local__collection_258.length))
    {
      var local_ch;
      local_ch = local__collection_258.charCodeAt( local__index_257 );
      ++local__index_257;
      this.m_write__Character( local_ch );
    }

  }

  return this;
}

function BardStringBuilder__write__StringBuilder( local_value )
{
  if ((local_value == null))
  {
    this.m_write__String( "null" );
    return this;
  }

  {
    var local__index_261;
    var local__collection_262;
    local__index_261 = 0;
    local__collection_262 = local_value;
    while ((local__index_261 < local__collection_262.m_count()))
    {
      var local_ch;
      local_ch = local__collection_262.m_get__Integer( local__index_261 );
      ++local__index_261;
      this.m_write__Character( local_ch );
    }

  }

  return this;
}

// NATIVE function BardGenericArray__clear_references__Integer_Integer( THIS, local_i1, local_i2 )

// NATIVE function BardGenericArray__shift__Integer_Integer_Integer( THIS, local_i1, local_i2, local_delta )

function BardGenericArray__init_settings( THIS )
{
}

function BardGenericArray__init_defaults( THIS )
{
  return this;
}

function BardArray_of_Object__init_settings()
{
}

function BardArray_of_Object__init_defaults()
{
  BardGenericArray__init_defaults( this );
  return this;
}

function BardArray_of_Object__create__Integer( local_count )
{
  Bard_throw_missing_return();
}

function BardArray_of_Object__count()
{
  return this.length;
}

function BardArray_of_Object__get__Integer( local_index )
{
  return this[local_index];
}

function BardArray_of_Object__set__Integer_Object( local_index, local_value )
{
  this[local_index] = local_value;
  Bard_throw_missing_return();
}

function BardArray_of_Object__to_String()
{
  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_first;
  local_first = true;
  {
    var local__index_317;
    var local__collection_318;
    local__index_317 = 0;
    local__collection_318 = this;
    while ((local__index_317 < local__collection_318.m_count()))
    {
      var local_element;
      local_element = local__collection_318.m_get__Integer( local__index_317 );
      ++local__index_317;
      if (local_first)
      {
        local_first = false;
      }
      else
      {
        local_buffer.m_print__Character( 44 );
      }

      local_buffer.m_print__Object( local_element );
    }

  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardException__init()
{
  return this;
}

function BardException__to_String()
{
  return this.p_message;
}

function BardException__init_settings()
{
}

function BardException__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardException__init__String( local__message )
{
  this.p_message = local__message;
  return this;
}

function BardGlobal__init_settings()
{
  BardGlobalSettings.s_global_string_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
}

function BardGlobal__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardGlobal__print__String( local_value )
{
  return BardSingletons.singleton_Console.m_print__String( local_value );
}

function BardGlobal__print__Real( local_value )
{
  return BardSingletons.singleton_Console.m_print__Real( local_value );
}

function BardGlobal__print__Real_Integer( local_value, local_decimal_places )
{
  return BardSingletons.singleton_Console.m_print__Real_Integer( local_value, local_decimal_places );
}

function BardGlobal__print__Integer( local_value )
{
  return BardSingletons.singleton_Console.m_print__Integer( local_value );
}

function BardGlobal__print__Character( local_value )
{
  return BardSingletons.singleton_Console.m_print__Character( local_value );
}

function BardGlobal__print__Object( local_value )
{
  return BardSingletons.singleton_Console.m_print__Object( local_value );
}

function BardGlobal__print__Byte( local_value )
{
  return BardSingletons.singleton_Console.m_print__Byte( local_value );
}

function BardGlobal__print__Logical( local_value )
{
  return BardSingletons.singleton_Console.m_print__Logical( local_value );
}

function BardGlobal__println__Object( local_value )
{
  return BardSingletons.singleton_Console.m_println__Object( local_value );
}

function BardGlobal__println__String( local_value )
{
  return BardSingletons.singleton_Console.m_println__String( local_value );
}

function BardGlobal__println__Real( local_value )
{
  return BardSingletons.singleton_Console.m_println__Real( local_value );
}

function BardGlobal__println__Real_Integer( local_value, local_decimal_places )
{
  return BardSingletons.singleton_Console.m_println__Real_Integer( local_value, local_decimal_places );
}

function BardGlobal__println__Integer( local_value )
{
  return BardSingletons.singleton_Console.m_println__Integer( local_value );
}

function BardGlobal__println__Character( local_value )
{
  return BardSingletons.singleton_Console.m_println__Character( local_value );
}

function BardGlobal__println__Byte( local_value )
{
  return BardSingletons.singleton_Console.m_println__Byte( local_value );
}

function BardGlobal__println__Logical( local_value )
{
  return BardSingletons.singleton_Console.m_println__Logical( local_value );
}

function BardGlobal__println()
{
  return BardSingletons.singleton_Console.m_println();
}

function BardMain__init()
{
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "[Test.bard line 33, Main::init()->Main]  alpha:" ).m_print__Integer( BardMainSettings.s_alpha ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "[Test.bard line 34, Main::init()->Main]  beta:" ).m_print__Integer( BardMainSettings.s_beta ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "[Test.bard line 35, Main::init()->Main]  gamma:" ).m_print__Integer( BardMainSettings.s_gamma ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "[Test.bard line 36, Main::init()->Main]  psi:" ).m_print__Integer( BardMainSettings.s_psi ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "[Test.bard line 37, Main::init()->Main]  omega:" ).m_print__Integer( BardMainSettings.s_omega ).m_to_String() );
  return this;
}

function BardMain__init_settings()
{
  BardMainSettings.s_alpha = 0;
  BardMainSettings.s_beta = 1;
  BardMainSettings.s_gamma = 2;
  BardMainSettings.s_psi = 23;
  BardMainSettings.s_omega = 24;
}

function BardMain__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardMain__print_variations__String( local_st )
{
  var local_stopwatch;
  local_stopwatch = BardObject__init.call( new BardStopwatch().m_init_defaults() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "Here's a word: " ).m_print__String( local_st ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "In reverse: " ).m_print__String( BardString__reversed( local_st ) ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "In uppercase: " ).m_print__String( BardString__to_uppercase( local_st ) ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "In lowercase: " ).m_print__String( BardString__to_lowercase( local_st ) ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "In CrAzY CaPs: " ).m_print__String( this.m_crazy_caps__String( local_st ) ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "Elapsed time for printing: " ).m_print__Object( local_stopwatch ).m_to_String() );
  var local_i;
  local_i = 0;
  var local_sum;
  local_sum = 0;
  while ((local_i < 1000000))
  {
    local_sum += local_i;
    ++local_i;
  }

  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "Sum: " ).m_print__Integer( local_sum ).m_to_String() );
  BardSingletons.singleton_Global.m_println__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "Elapsed time for sum: " ).m_print__Object( local_stopwatch ).m_to_String() );
  local_stopwatch.m_restart();
  BardSingletons.singleton_Global.m_println__Real( 33.14 );
  this.p_position = {'p_x':3.0,'p_y':4.0};
  BardSingletons.singleton_Global.m_println__String( BardVector2__to_String( this.p_position ) );
}

function BardMain__crazy_caps__String( local_st )
{
  var local_uc;
  local_uc = BardString__to_uppercase( local_st );
  var local_lc;
  local_lc = BardString__to_lowercase( local_st );
  var local_result;
  local_result = BardStringBuilder__init__Integer.call( new BardStringBuilder().m_init_defaults(), local_st.length );
  {
    var local_i;
    var local__collection_321;
    local_i = -1;
    local__collection_321 = local_st;
    while ((local_i < (local__collection_321.length - 1)))
    {
      ++local_i;
      if (!!(local_i & 1))
      {
        local_result.m_print__Character( local_lc.charCodeAt( local_i ) );
      }
      else
      {
        local_result.m_print__Character( local_uc.charCodeAt( local_i ) );
      }

    }

  }

  return local_result.m_to_String();
}

function BardTable_of_String_String__init()
{
  this.m_init__Integer( 16 );
  return this;
}

function BardTable_of_String_String__to_String()
{
  return this.m_print_to__StringBuilder( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ) ).m_to_String();
}

function BardTable_of_String_String__init_settings()
{
}

function BardTable_of_String_String__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardTable_of_String_String__init__Integer( local_bin_count )
{
  var local_bins_power_of_2;
  local_bins_power_of_2 = 1;
  while ((local_bins_power_of_2 < local_bin_count))
  {
    local_bins_power_of_2 = (local_bins_power_of_2 << 1);
  }

  local_bin_count = local_bins_power_of_2;
  this.p_bin_mask = (local_bin_count - 1);
  this.p_bins = BardObjectList__init__Integer_TableBin_of_String_String.call( new BardObjectList().m_init_defaults(), local_bin_count, null );
  this.p_keys = BardStringList__init.call( new BardStringList().m_init_defaults() );
  return this;
}

function BardTable_of_String_String__at__Integer( local_index )
{
  return this.m_get__String( this.p_keys.m_get__Integer( local_index ) );
}

function BardTable_of_String_String__contains__String( local_key )
{
  return !!this.m_find__String( local_key );
}

function BardTable_of_String_String__count()
{
  return this.p_keys.p_count;
}

function BardTable_of_String_String__find__String( local_key )
{
  var local_hash;
  local_hash = BardString__hash_code( local_key );
  var local_bin;
  local_bin = this.p_bins.m_get__Integer( (local_hash & this.p_bin_mask) );
  if (!!local_bin)
  {
    return local_bin.m_find__String_Integer( local_key, local_hash );
  }
  else
  {
    return null;
  }

  Bard_throw_missing_return();
}

function BardTable_of_String_String__get__String( local_key )
{
  var local_entry;
  local_entry = this.m_find__String( local_key );
  if (!!local_entry)
  {
    return local_entry.p_value;
  }
  else
  {
    var local_default_value;
    local_default_value = null;
    return local_default_value;
  }

  Bard_throw_missing_return();
}

function BardTable_of_String_String__set__String_String( local_key, local_value )
{
  var local_hash;
  local_hash = BardString__hash_code( local_key );
  var local_index;
  local_index = (local_hash & this.p_bin_mask);
  var local_bin;
  local_bin = this.p_bins.m_get__Integer( local_index );
  if (!!local_bin)
  {
    var local_entry;
    local_entry = local_bin.m_find__String_Integer( local_key, local_hash );
    if (!!local_entry)
    {
      local_entry.p_value = local_value;
      return;
    }

  }
  else
  {
    local_bin = BardTableBin_of_String_String__init.call( new BardTableBin_of_String_String().m_init_defaults() );
    this.p_bins.m_set__Integer_TableBin_of_String_String( local_index, local_bin );
  }

  this.p_keys.m_add__String( local_key );
  local_bin.p_entries.m_add__TableEntry_of_String_String( BardTableEntry_of_String_String__init__String_String_Integer.call( new BardTableEntry_of_String_String().m_init_defaults(), local_key, local_value, local_hash ) );
}

function BardTable_of_String_String__print_to__StringBuilder( local_buffer )
{
  local_buffer.m_print__Character( 91 );
  var local_i;
  local_i = 0;
  while ((local_i < this.p_keys.p_count))
  {
    var local_key;
    local_key = this.p_keys.m_get__Integer( local_i );
    local_buffer.m_print__String( local_key );
    local_buffer.m_print__Character( 58 );
    local_buffer.m_print__String( this.m_get__String( local_key ) );
    ++local_i;
  }

  local_buffer.m_print__Character( 93 );
  return local_buffer;
}

function BardByteList__init()
{
  this.p_data = new Unit8Array( 10 );
  return this;
}

function BardByteList__to_String()
{
  if ((this.p_count == 0))
  {
    return "[]";
  }

  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_i;
  local_i = 0;
  var local_limit;
  local_limit = this.p_count;
  while ((local_i < local_limit))
  {
    if ((local_i > 0))
    {
      local_buffer.m_print__Character( 44 );
    }

    local_buffer.m_print__Byte( this.m_get__Integer( local_i ) );
    ++local_i;
  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardByteList__init_settings()
{
}

function BardByteList__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardByteList__init__Integer( local_initial_capacity )
{
  this.p_data = new Unit8Array( local_initial_capacity );
  return this;
}

function BardByteList__init__Integer_Byte( local_size, local_initial_value )
{
  this.m_init__Integer( local_size );
  while ((local_size > 0))
  {
    this.m_add__Byte( local_initial_value );
    --local_size;
  }

  return this;
}

function BardByteList__init__Array_of_Byte( local__data )
{
  this.p_data = local__data;
  this.p_count = this.p_data.length;
  return this;
}

function BardByteList__add__Byte( local_value )
{
  return this.m_insert__Byte_Integer( local_value, this.p_count );
}

function BardByteList__add__ByteList( local_list )
{
  return this.m_insert__ByteList_Integer( local_list, this.p_count );
}

function BardByteList__capacity()
{
  return this.p_data.length;
}

function BardByteList__clear()
{
  this.m_discard__Integer_Integer( 0, (this.p_count - 1) );
  return this;
}

function BardByteList__clone()
{
  var local_result;
  local_result = BardByteList__init__Integer.call( new BardByteList().m_init_defaults(), this.p_count );
  {
    var local__index_305;
    var local__collection_306;
    local__index_305 = 0;
    local__collection_306 = this;
    while ((local__index_305 < local__collection_306.p_count))
    {
      var local_value;
      local_value = local__collection_306.m_get__Integer( local__index_305 );
      ++local__index_305;
      local_result.m_add__Byte( local_value );
    }

  }

  return local_result;
}

function BardByteList__contains__Byte( local_value )
{
  return (this.m_find__Byte( local_value ) != -1);
}

function BardByteList__discard__Integer_Integer( local_i1, local_i2 )
{
  if ((local_i1 > local_i2))
  {
    return;
  }

  var local_original_count;
  local_original_count = this.p_count;
  var local_discard_count;
  local_discard_count = ((local_i2 - local_i1) + 1);
  BardGenericArray__shift__Integer_Integer_Integer( this.p_data, (local_i2 + 1), this.p_count, -local_discard_count );
  this.p_count -= local_discard_count;
  BardGenericArray__clear_references__Integer_Integer( this.p_data, this.p_count, (local_original_count - 1) );
}

function BardByteList__discard_from__Integer( local_i1 )
{
  this.m_discard__Integer_Integer( local_i1, (this.p_count - 1) );
}

function BardByteList__ensure_capacity__Integer( local_desired_capacity )
{
  var local_cur_capacity;
  local_cur_capacity = this.p_data.length;
  if ((local_desired_capacity > local_cur_capacity))
  {
    var local_new_capacity;
    local_new_capacity = (local_cur_capacity + local_cur_capacity);
    if ((local_desired_capacity > local_new_capacity))
    {
      local_new_capacity = local_desired_capacity;
    }

    var local_new_data;
    local_new_data = new Unit8Array( local_new_capacity );
    var local_i;
    local_i = 0;
    while ((local_i < this.p_count))
    {
      local_new_data[local_i] = this.p_data[local_i];
      ++local_i;
    }

    this.p_data = local_new_data;
  }

  return this;
}

function BardByteList__find__Byte( local_value )
{
  return this.m_find__Byte_Integer( local_value, 0 );
}

function BardByteList__find__Byte_Integer( local_value, local_i1 )
{
  var local_c;
  local_c = this.p_count;
  while ((local_i1 < this.p_count))
  {
    if ((this.m_get__Integer( local_i1 ) == local_value))
    {
      return local_i1;
    }

    ++local_i1;
  }

  return -1;
}

function BardByteList__first()
{
  return this.m_get__Integer( 0 );
}

function BardByteList__get__Integer( local_index )
{
  return this.p_data[local_index];
}

function BardByteList__indices()
{
  return BardListIndexReader__init__Integer.call( new BardListIndexReader().m_init_defaults(), this.p_count );
}

function BardByteList__insert__Byte( local_value )
{
  return this.m_insert__Byte_Integer( local_value, 0 );
}

function BardByteList__insert__ByteList( local_list )
{
  return this.m_insert__ByteList_Integer( local_list, 0 );
}

function BardByteList__insert__Byte_Integer( local_value, local_before_index )
{
  if ((local_before_index == this.p_count))
  {
    if ((this.p_data == null))
    {
      this.p_data = new Unit8Array( 10 );
    }
    else if ((this.p_count == this.p_data.length))
    {
      if ((this.p_count < 10))
      {
        this.m_ensure_capacity__Integer( 10 );
      }
      else
      {
        this.m_ensure_capacity__Integer( (this.p_count * 2) );
      }

    }

    this.p_data[this.p_count] = local_value;
    ++this.p_count;
    return this;
  }
  else
  {
    this.m_add__Byte( local_value );
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 2), 1 );
    this.p_data[local_before_index] = local_value;
    return this;
  }

  Bard_throw_missing_return();
}

function BardByteList__insert__ByteList_Integer( local_list, local_before_index )
{
  var local_required_capacity;
  local_required_capacity = (this.p_count + local_list.p_count);
  if ((this.p_data == null))
  {
    this.p_data = new Unit8Array( local_required_capacity );
  }
  else
  {
    if ((local_required_capacity < 10))
    {
      local_required_capacity = 10;
    }

  }

  this.m_ensure_capacity__Integer( local_required_capacity );
  var local_src_i;
  local_src_i = 0;
  var local_dest_i;
  local_dest_i = local_before_index;
  var local_other_count;
  local_other_count = local_list.p_count;
  var local_other_data;
  local_other_data = local_list.p_data;
  if ((local_before_index < this.p_count))
  {
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 1), local_other_count );
  }

  while ((local_src_i < local_other_count))
  {
    this.p_data[local_dest_i] = local_other_data[local_src_i];
    ++local_src_i;
    ++local_dest_i;
  }

  this.p_count += local_other_count;
  return this;
}

function BardByteList__last()
{
  return this.m_get__Integer( (this.p_count - 1) );
}

function BardByteList__random()
{
  return this.m_get__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardByteList__remove__Byte( local_value )
{
  var local_i;
  local_i = 0;
  var local_c;
  local_c = this.p_count;
  while ((local_i < local_c))
  {
    if ((this.m_get__Integer( local_i ) == local_value))
    {
      this.m_remove_at__Integer( local_i );
      return true;
    }

    ++local_i;
  }

  return false;
}

function BardByteList__remove_at__Integer( local_index )
{
  var local_result;
  local_result = this.m_get__Integer( local_index );
  this.m_discard__Integer_Integer( local_index, local_index );
  return local_result;
}

function BardByteList__remove_first()
{
  return this.m_remove_at__Integer( 0 );
}

function BardByteList__remove_last()
{
  return this.m_remove_at__Integer( (this.p_count - 1) );
}

function BardByteList__remove_random()
{
  return this.m_remove_at__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardByteList__reverse()
{
  this.m_reverse__Integer_Integer( 0, (this.p_count - 1) );
}

function BardByteList__reverse__Integer_Integer( local_i1, local_i2 )
{
  var local_i2;
  local_i2 = (this.p_count - 1);
  while ((local_i1 < local_i2))
  {
    var local_temp;
    local_temp = this.m_get__Integer( local_i1 );
    this.m_set__Integer_Byte( local_i1, this.m_get__Integer( local_i2 ) );
    this.m_set__Integer_Byte( local_i2, local_temp );
    ++local_i1;
    --local_i2;
  }

}

function BardByteList__reverse_order()
{
  return BardReverseOrderListReader_of_Byte__init__ByteList.call( new BardReverseOrderListReader_of_Byte().m_init_defaults(), this );
}

function BardByteList__set__Integer_Byte( local_index, local_value )
{
  this.p_data[local_index] = local_value;
}

function BardByteList__shuffle()
{
  var local_limit;
  local_limit = this.p_count;
  var local_i;
  local_i = (local_limit - 1);
  while ((local_i >= 0))
  {
    var local_r;
    local_r = BardSingletons.singleton_Random.m_to_Integer__Integer( local_limit );
    var local_temp;
    local_temp = this.m_get__Integer( local_i );
    this.m_set__Integer_Byte( local_i, this.m_get__Integer( local_r ) );
    this.m_set__Integer_Byte( local_r, local_temp );
    --local_i;
  }

}

function BardByteList__swap__Integer_Integer( local_i1, local_i2 )
{
  var local_temp;
  local_temp = this.m_get__Integer( local_i1 );
  this.m_set__Integer_Byte( local_i1, this.m_get__Integer( local_i2 ) );
  this.m_set__Integer_Byte( local_i2, local_temp );
}

function BardCharacterList__init()
{
  this.p_data = new Uint16Array( 10 );
  return this;
}

function BardCharacterList__to_String()
{
  if ((this.p_count == 0))
  {
    return "[]";
  }

  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_i;
  local_i = 0;
  var local_limit;
  local_limit = this.p_count;
  while ((local_i < local_limit))
  {
    if ((local_i > 0))
    {
      local_buffer.m_print__Character( 44 );
    }

    local_buffer.m_print__Character( this.m_get__Integer( local_i ) );
    ++local_i;
  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardCharacterList__init_settings()
{
}

function BardCharacterList__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardCharacterList__init__Integer( local_initial_capacity )
{
  this.p_data = new Uint16Array( local_initial_capacity );
  return this;
}

function BardCharacterList__init__Integer_Character( local_size, local_initial_value )
{
  this.m_init__Integer( local_size );
  while ((local_size > 0))
  {
    this.m_add__Character( local_initial_value );
    --local_size;
  }

  return this;
}

function BardCharacterList__init__Array_of_Character( local__data )
{
  this.p_data = local__data;
  this.p_count = this.p_data.length;
  return this;
}

function BardCharacterList__add__Character( local_value )
{
  return this.m_insert__Character_Integer( local_value, this.p_count );
}

function BardCharacterList__add__CharacterList( local_list )
{
  return this.m_insert__CharacterList_Integer( local_list, this.p_count );
}

function BardCharacterList__capacity()
{
  return this.p_data.length;
}

function BardCharacterList__clear()
{
  this.m_discard__Integer_Integer( 0, (this.p_count - 1) );
  return this;
}

function BardCharacterList__clone()
{
  var local_result;
  local_result = BardCharacterList__init__Integer.call( new BardCharacterList().m_init_defaults(), this.p_count );
  {
    var local__index_239;
    var local__collection_240;
    local__index_239 = 0;
    local__collection_240 = this;
    while ((local__index_239 < local__collection_240.p_count))
    {
      var local_value;
      local_value = local__collection_240.m_get__Integer( local__index_239 );
      ++local__index_239;
      local_result.m_add__Character( local_value );
    }

  }

  return local_result;
}

function BardCharacterList__contains__Character( local_value )
{
  return (this.m_find__Character( local_value ) != -1);
}

function BardCharacterList__discard__Integer_Integer( local_i1, local_i2 )
{
  if ((local_i1 > local_i2))
  {
    return;
  }

  var local_original_count;
  local_original_count = this.p_count;
  var local_discard_count;
  local_discard_count = ((local_i2 - local_i1) + 1);
  BardGenericArray__shift__Integer_Integer_Integer( this.p_data, (local_i2 + 1), this.p_count, -local_discard_count );
  this.p_count -= local_discard_count;
  BardGenericArray__clear_references__Integer_Integer( this.p_data, this.p_count, (local_original_count - 1) );
}

function BardCharacterList__discard_from__Integer( local_i1 )
{
  this.m_discard__Integer_Integer( local_i1, (this.p_count - 1) );
}

function BardCharacterList__ensure_capacity__Integer( local_desired_capacity )
{
  var local_cur_capacity;
  local_cur_capacity = this.p_data.length;
  if ((local_desired_capacity > local_cur_capacity))
  {
    var local_new_capacity;
    local_new_capacity = (local_cur_capacity + local_cur_capacity);
    if ((local_desired_capacity > local_new_capacity))
    {
      local_new_capacity = local_desired_capacity;
    }

    var local_new_data;
    local_new_data = new Uint16Array( local_new_capacity );
    var local_i;
    local_i = 0;
    while ((local_i < this.p_count))
    {
      local_new_data[local_i] = this.p_data[local_i];
      ++local_i;
    }

    this.p_data = local_new_data;
  }

  return this;
}

function BardCharacterList__find__Character( local_value )
{
  return this.m_find__Character_Integer( local_value, 0 );
}

function BardCharacterList__find__Character_Integer( local_value, local_i1 )
{
  var local_c;
  local_c = this.p_count;
  while ((local_i1 < this.p_count))
  {
    if ((this.m_get__Integer( local_i1 ) == local_value))
    {
      return local_i1;
    }

    ++local_i1;
  }

  return -1;
}

function BardCharacterList__first()
{
  return this.m_get__Integer( 0 );
}

function BardCharacterList__get__Integer( local_index )
{
  return this.p_data[local_index];
}

function BardCharacterList__indices()
{
  return BardListIndexReader__init__Integer.call( new BardListIndexReader().m_init_defaults(), this.p_count );
}

function BardCharacterList__insert__Character( local_value )
{
  return this.m_insert__Character_Integer( local_value, 0 );
}

function BardCharacterList__insert__CharacterList( local_list )
{
  return this.m_insert__CharacterList_Integer( local_list, 0 );
}

function BardCharacterList__insert__Character_Integer( local_value, local_before_index )
{
  if ((local_before_index == this.p_count))
  {
    if ((this.p_data == null))
    {
      this.p_data = new Uint16Array( 10 );
    }
    else if ((this.p_count == this.p_data.length))
    {
      if ((this.p_count < 10))
      {
        this.m_ensure_capacity__Integer( 10 );
      }
      else
      {
        this.m_ensure_capacity__Integer( (this.p_count * 2) );
      }

    }

    this.p_data[this.p_count] = local_value;
    ++this.p_count;
    return this;
  }
  else
  {
    this.m_add__Character( local_value );
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 2), 1 );
    this.p_data[local_before_index] = local_value;
    return this;
  }

  Bard_throw_missing_return();
}

function BardCharacterList__insert__CharacterList_Integer( local_list, local_before_index )
{
  var local_required_capacity;
  local_required_capacity = (this.p_count + local_list.p_count);
  if ((this.p_data == null))
  {
    this.p_data = new Uint16Array( local_required_capacity );
  }
  else
  {
    if ((local_required_capacity < 10))
    {
      local_required_capacity = 10;
    }

  }

  this.m_ensure_capacity__Integer( local_required_capacity );
  var local_src_i;
  local_src_i = 0;
  var local_dest_i;
  local_dest_i = local_before_index;
  var local_other_count;
  local_other_count = local_list.p_count;
  var local_other_data;
  local_other_data = local_list.p_data;
  if ((local_before_index < this.p_count))
  {
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 1), local_other_count );
  }

  while ((local_src_i < local_other_count))
  {
    this.p_data[local_dest_i] = local_other_data[local_src_i];
    ++local_src_i;
    ++local_dest_i;
  }

  this.p_count += local_other_count;
  return this;
}

function BardCharacterList__last()
{
  return this.m_get__Integer( (this.p_count - 1) );
}

function BardCharacterList__random()
{
  return this.m_get__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardCharacterList__remove__Character( local_value )
{
  var local_i;
  local_i = 0;
  var local_c;
  local_c = this.p_count;
  while ((local_i < local_c))
  {
    if ((this.m_get__Integer( local_i ) == local_value))
    {
      this.m_remove_at__Integer( local_i );
      return true;
    }

    ++local_i;
  }

  return false;
}

function BardCharacterList__remove_at__Integer( local_index )
{
  var local_result;
  local_result = this.m_get__Integer( local_index );
  this.m_discard__Integer_Integer( local_index, local_index );
  return local_result;
}

function BardCharacterList__remove_first()
{
  return this.m_remove_at__Integer( 0 );
}

function BardCharacterList__remove_last()
{
  return this.m_remove_at__Integer( (this.p_count - 1) );
}

function BardCharacterList__remove_random()
{
  return this.m_remove_at__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardCharacterList__reverse()
{
  this.m_reverse__Integer_Integer( 0, (this.p_count - 1) );
}

function BardCharacterList__reverse__Integer_Integer( local_i1, local_i2 )
{
  var local_i2;
  local_i2 = (this.p_count - 1);
  while ((local_i1 < local_i2))
  {
    var local_temp;
    local_temp = this.m_get__Integer( local_i1 );
    this.m_set__Integer_Character( local_i1, this.m_get__Integer( local_i2 ) );
    this.m_set__Integer_Character( local_i2, local_temp );
    ++local_i1;
    --local_i2;
  }

}

function BardCharacterList__reverse_order()
{
  return BardReverseOrderListReader_of_Character__init__CharacterList.call( new BardReverseOrderListReader_of_Character().m_init_defaults(), this );
}

function BardCharacterList__set__Integer_Character( local_index, local_value )
{
  this.p_data[local_index] = local_value;
}

function BardCharacterList__shuffle()
{
  var local_limit;
  local_limit = this.p_count;
  var local_i;
  local_i = (local_limit - 1);
  while ((local_i >= 0))
  {
    var local_r;
    local_r = BardSingletons.singleton_Random.m_to_Integer__Integer( local_limit );
    var local_temp;
    local_temp = this.m_get__Integer( local_i );
    this.m_set__Integer_Character( local_i, this.m_get__Integer( local_r ) );
    this.m_set__Integer_Character( local_r, local_temp );
    --local_i;
  }

}

function BardCharacterList__swap__Integer_Integer( local_i1, local_i2 )
{
  var local_temp;
  local_temp = this.m_get__Integer( local_i1 );
  this.m_set__Integer_Character( local_i1, this.m_get__Integer( local_i2 ) );
  this.m_set__Integer_Character( local_i2, local_temp );
}

function BardStringList__init()
{
  this.p_data = new Array( 10 );
  return this;
}

function BardStringList__to_String()
{
  if ((this.p_count == 0))
  {
    return "[]";
  }

  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_i;
  local_i = 0;
  var local_limit;
  local_limit = this.p_count;
  while ((local_i < local_limit))
  {
    if ((local_i > 0))
    {
      local_buffer.m_print__Character( 44 );
    }

    local_buffer.m_print__String( this.m_get__Integer( local_i ) );
    ++local_i;
  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardStringList__init_settings()
{
}

function BardStringList__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardStringList__init__Integer( local_initial_capacity )
{
  this.p_data = new Array( local_initial_capacity );
  return this;
}

function BardStringList__init__Integer_String( local_size, local_initial_value )
{
  this.m_init__Integer( local_size );
  while ((local_size > 0))
  {
    this.m_add__String( local_initial_value );
    --local_size;
  }

  return this;
}

function BardStringList__init__Array_of_String( local__data )
{
  this.p_data = local__data;
  this.p_count = this.p_data.length;
  return this;
}

function BardStringList__add__String( local_value )
{
  return this.m_insert__String_Integer( local_value, this.p_count );
}

function BardStringList__add__StringList( local_list )
{
  return this.m_insert__StringList_Integer( local_list, this.p_count );
}

function BardStringList__capacity()
{
  return this.p_data.length;
}

function BardStringList__clear()
{
  this.m_discard__Integer_Integer( 0, (this.p_count - 1) );
  return this;
}

function BardStringList__clone()
{
  var local_result;
  local_result = BardStringList__init__Integer.call( new BardStringList().m_init_defaults(), this.p_count );
  {
    var local__index_273;
    var local__collection_274;
    local__index_273 = 0;
    local__collection_274 = this;
    while ((local__index_273 < local__collection_274.p_count))
    {
      var local_value;
      local_value = local__collection_274.m_get__Integer( local__index_273 );
      ++local__index_273;
      local_result.m_add__String( local_value );
    }

  }

  return local_result;
}

function BardStringList__contains__String( local_value )
{
  return (this.m_find__String( local_value ) != -1);
}

function BardStringList__discard__Integer_Integer( local_i1, local_i2 )
{
  if ((local_i1 > local_i2))
  {
    return;
  }

  var local_original_count;
  local_original_count = this.p_count;
  var local_discard_count;
  local_discard_count = ((local_i2 - local_i1) + 1);
  BardGenericArray__shift__Integer_Integer_Integer( this.p_data, (local_i2 + 1), this.p_count, -local_discard_count );
  this.p_count -= local_discard_count;
  BardGenericArray__clear_references__Integer_Integer( this.p_data, this.p_count, (local_original_count - 1) );
}

function BardStringList__discard_from__Integer( local_i1 )
{
  this.m_discard__Integer_Integer( local_i1, (this.p_count - 1) );
}

function BardStringList__ensure_capacity__Integer( local_desired_capacity )
{
  var local_cur_capacity;
  local_cur_capacity = this.p_data.length;
  if ((local_desired_capacity > local_cur_capacity))
  {
    var local_new_capacity;
    local_new_capacity = (local_cur_capacity + local_cur_capacity);
    if ((local_desired_capacity > local_new_capacity))
    {
      local_new_capacity = local_desired_capacity;
    }

    var local_new_data;
    local_new_data = new Array( local_new_capacity );
    var local_i;
    local_i = 0;
    while ((local_i < this.p_count))
    {
      local_new_data[local_i] = this.p_data[local_i];
      ++local_i;
    }

    this.p_data = local_new_data;
  }

  return this;
}

function BardStringList__find__String( local_value )
{
  return this.m_find__String_Integer( local_value, 0 );
}

function BardStringList__find__String_Integer( local_value, local_i1 )
{
  var local_c;
  local_c = this.p_count;
  while ((local_i1 < this.p_count))
  {
    if (BardString__equals__String( this.m_get__Integer( local_i1 ), local_value ))
    {
      return local_i1;
    }

    ++local_i1;
  }

  return -1;
}

function BardStringList__first()
{
  return this.m_get__Integer( 0 );
}

function BardStringList__get__Integer( local_index )
{
  return this.p_data[local_index];
}

function BardStringList__indices()
{
  return BardListIndexReader__init__Integer.call( new BardListIndexReader().m_init_defaults(), this.p_count );
}

function BardStringList__insert__String( local_value )
{
  return this.m_insert__String_Integer( local_value, 0 );
}

function BardStringList__insert__StringList( local_list )
{
  return this.m_insert__StringList_Integer( local_list, 0 );
}

function BardStringList__insert__String_Integer( local_value, local_before_index )
{
  if ((local_before_index == this.p_count))
  {
    if ((this.p_data == null))
    {
      this.p_data = new Array( 10 );
    }
    else if ((this.p_count == this.p_data.length))
    {
      if ((this.p_count < 10))
      {
        this.m_ensure_capacity__Integer( 10 );
      }
      else
      {
        this.m_ensure_capacity__Integer( (this.p_count * 2) );
      }

    }

    this.p_data[this.p_count] = local_value;
    ++this.p_count;
    return this;
  }
  else
  {
    this.m_add__String( local_value );
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 2), 1 );
    this.p_data[local_before_index] = local_value;
    return this;
  }

  Bard_throw_missing_return();
}

function BardStringList__insert__StringList_Integer( local_list, local_before_index )
{
  var local_required_capacity;
  local_required_capacity = (this.p_count + local_list.p_count);
  if ((this.p_data == null))
  {
    this.p_data = new Array( local_required_capacity );
  }
  else
  {
    if ((local_required_capacity < 10))
    {
      local_required_capacity = 10;
    }

  }

  this.m_ensure_capacity__Integer( local_required_capacity );
  var local_src_i;
  local_src_i = 0;
  var local_dest_i;
  local_dest_i = local_before_index;
  var local_other_count;
  local_other_count = local_list.p_count;
  var local_other_data;
  local_other_data = local_list.p_data;
  if ((local_before_index < this.p_count))
  {
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 1), local_other_count );
  }

  while ((local_src_i < local_other_count))
  {
    this.p_data[local_dest_i] = local_other_data[local_src_i];
    ++local_src_i;
    ++local_dest_i;
  }

  this.p_count += local_other_count;
  return this;
}

function BardStringList__last()
{
  return this.m_get__Integer( (this.p_count - 1) );
}

function BardStringList__random()
{
  return this.m_get__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardStringList__remove__String( local_value )
{
  var local_i;
  local_i = 0;
  var local_c;
  local_c = this.p_count;
  while ((local_i < local_c))
  {
    if ((this.m_get__Integer( local_i ) == local_value))
    {
      this.m_remove_at__Integer( local_i );
      return true;
    }

    ++local_i;
  }

  return false;
}

function BardStringList__remove_at__Integer( local_index )
{
  var local_result;
  local_result = this.m_get__Integer( local_index );
  this.m_discard__Integer_Integer( local_index, local_index );
  return local_result;
}

function BardStringList__remove_first()
{
  return this.m_remove_at__Integer( 0 );
}

function BardStringList__remove_last()
{
  return this.m_remove_at__Integer( (this.p_count - 1) );
}

function BardStringList__remove_random()
{
  return this.m_remove_at__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardStringList__reverse()
{
  this.m_reverse__Integer_Integer( 0, (this.p_count - 1) );
}

function BardStringList__reverse__Integer_Integer( local_i1, local_i2 )
{
  var local_i2;
  local_i2 = (this.p_count - 1);
  while ((local_i1 < local_i2))
  {
    var local_temp;
    local_temp = this.m_get__Integer( local_i1 );
    this.m_set__Integer_String( local_i1, this.m_get__Integer( local_i2 ) );
    this.m_set__Integer_String( local_i2, local_temp );
    ++local_i1;
    --local_i2;
  }

}

function BardStringList__reverse_order()
{
  return BardReverseOrderListReader_of_String__init__StringList.call( new BardReverseOrderListReader_of_String().m_init_defaults(), this );
}

function BardStringList__set__Integer_String( local_index, local_value )
{
  this.p_data[local_index] = local_value;
}

function BardStringList__shuffle()
{
  var local_limit;
  local_limit = this.p_count;
  var local_i;
  local_i = (local_limit - 1);
  while ((local_i >= 0))
  {
    var local_r;
    local_r = BardSingletons.singleton_Random.m_to_Integer__Integer( local_limit );
    var local_temp;
    local_temp = this.m_get__Integer( local_i );
    this.m_set__Integer_String( local_i, this.m_get__Integer( local_r ) );
    this.m_set__Integer_String( local_r, local_temp );
    --local_i;
  }

}

function BardStringList__swap__Integer_Integer( local_i1, local_i2 )
{
  var local_temp;
  local_temp = this.m_get__Integer( local_i1 );
  this.m_set__Integer_String( local_i1, this.m_get__Integer( local_i2 ) );
  this.m_set__Integer_String( local_i2, local_temp );
}

function BardPrinter__init_settings()
{
}

function BardPrinter__print__String( local_value )
{
}

function BardPrinter__print__Real( local_value )
{
}

function BardPrinter__print__Real_Integer( local_value, local_decimal_places )
{
}

function BardPrinter__print__Integer( local_value )
{
}

function BardPrinter__print__Character( local_value )
{
}

function BardPrinter__print__Object( local_value )
{
}

function BardPrinter__print__Byte( local_value )
{
}

function BardPrinter__print__Logical( local_value )
{
}

function BardPrinter__println__Object( local_value )
{
}

function BardPrinter__println__String( local_value )
{
}

function BardPrinter__println__Real( local_value )
{
}

function BardPrinter__println__Real_Integer( local_value, local_decimal_places )
{
}

function BardPrinter__println__Integer( local_value )
{
}

function BardPrinter__println__Character( local_value )
{
}

function BardPrinter__println__Byte( local_value )
{
}

function BardPrinter__println__Logical( local_value )
{
}

function BardPrinter__println()
{
}

function BardConsole__init_settings()
{
}

function BardConsole__init_defaults()
{
  BardObject__init_defaults.call( this );
  this.p_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  return this;
}

function BardConsole__print__String( local_value )
{
  this.p_buffer.m_print__String( local_value );
  return this;
}

function BardConsole__print__Real( local_value )
{
  this.p_buffer.m_print__Real( local_value );
  return this;
}

function BardConsole__print__Real_Integer( local_value, local_decimal_places )
{
  this.p_buffer.m_print__Real_Integer( local_value, local_decimal_places );
  return this;
}

function BardConsole__print__Integer( local_value )
{
  this.p_buffer.m_print__Integer( local_value );
  return this;
}

function BardConsole__print__Character( local_value )
{
  this.p_buffer.m_print__Character( local_value );
  return this;
}

function BardConsole__print__Object( local_value )
{
  if (!!local_value)
  {
    this.p_buffer.m_print__String( local_value.m_to_String() );
  }
  else
  {
    this.p_buffer.m_print__String( "null" );
  }

  return this;
}

function BardConsole__print__Byte( local_value )
{
  this.p_buffer.m_print__Integer( local_value );
  return this;
}

function BardConsole__print__Logical( local_value )
{
  if (local_value)
  {
    this.p_buffer.m_print__String( "true" );
  }
  else
  {
    this.p_buffer.m_print__String( "false" );
  }

  return this;
}

function BardConsole__println__Object( local_value )
{
  return this.m_print__Object( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println__String( local_value )
{
  return this.m_print__String( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println__Real( local_value )
{
  return this.m_print__Real( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println__Real_Integer( local_value, local_decimal_places )
{
  return this.m_print__Real( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println__Integer( local_value )
{
  return this.m_print__Integer( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println__Character( local_value )
{
  return this.m_print__Character( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println__Byte( local_value )
{
  return this.m_print__Byte( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println__Logical( local_value )
{
  return this.m_print__Logical( local_value ).m_print__Character( 10 ).m_flush();
}

function BardConsole__println()
{
  return this.m_print__Character( 10 ).m_flush();
}

function BardConsole__flush()
{
  if (!!this.p_buffer.m_count())
  {
    this.m_write__StringBuilder( this.p_buffer );
    this.p_buffer.m_clear();
  }

  return this;
}

function BardConsole__indent()
{
  return this.p_buffer.m_indent();
}

function BardConsole__indent__Integer( local_new_indent )
{
  this.p_buffer.m_indent__Integer( local_new_indent );
  return this;
}

// NATIVE function BardConsole__write__Character( local_value )

// NATIVE function BardConsole__write__String( local_value )

function BardConsole__write__StringBuilder( local_value )
{
  {
    var local__index_265;
    var local__collection_266;
    local__index_265 = 0;
    local__collection_266 = local_value;
    while ((local__index_265 < local__collection_266.m_count()))
    {
      var local_ch;
      local_ch = local__collection_266.m_get__Integer( local__index_265 );
      ++local__index_265;
      this.m_write__Character( local_ch );
    }

  }

  return this;
}

function BardVector2__to_String( THIS )
{
  return BardString__print__String( BardString__print__Real( BardString__print__String( BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "(" ).m_print__Real( THIS.p_x ).m_to_String(), "," ), THIS.p_y ), ")" );
}

function BardVector2__init_settings( THIS )
{
}

function BardVector2__init_defaults( THIS )
{
  return this;
}

function BardStopwatch__to_String()
{
  return BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() ).m_print__String( "" ).m_print__Real( this.m_elapsed() ).m_to_String();
}

function BardStopwatch__init_settings()
{
}

function BardStopwatch__init_defaults()
{
  BardObject__init_defaults.call( this );
  this.p_start_time = BardSingletons.singleton_Time.m_current();
  return this;
}

function BardStopwatch__elapsed()
{
  return (BardSingletons.singleton_Time.m_current() - this.p_start_time);
}

function BardStopwatch__restart()
{
  this.p_start_time = BardSingletons.singleton_Time.m_current();
  return this;
}

function BardTableEntry_of_String_String__init_settings()
{
}

function BardTableEntry_of_String_String__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardTableEntry_of_String_String__init__String_String_Integer( local__key, local__value, local__hash )
{
  this.p_key = local__key;
  this.p_value = local__value;
  this.p_hash = local__hash;
  return this;
}

function BardListIndexReader__init_settings()
{
}

function BardListIndexReader__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardListIndexReader__init__Integer( local__count )
{
  this.p_count = local__count;
  return this;
}

function BardListIndexReader__get__Integer( local_index )
{
  return local_index;
}

function BardArray_of_Byte__init_settings()
{
}

function BardArray_of_Byte__init_defaults()
{
  BardGenericArray__init_defaults( this );
  return this;
}

function BardArray_of_Byte__create__Integer( local_count )
{
  Bard_throw_missing_return();
}

function BardArray_of_Byte__count()
{
  return this.length;
}

function BardArray_of_Byte__get__Integer( local_index )
{
  return this[local_index];
}

function BardArray_of_Byte__set__Integer_Byte( local_index, local_value )
{
  this[local_index] = local_value;
  Bard_throw_missing_return();
}

function BardArray_of_Byte__to_String()
{
  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_first;
  local_first = true;
  {
    var local__index_301;
    var local__collection_302;
    local__index_301 = 0;
    local__collection_302 = this;
    while ((local__index_301 < local__collection_302.m_count()))
    {
      var local_element;
      local_element = local__collection_302.m_get__Integer( local__index_301 );
      ++local__index_301;
      if (local_first)
      {
        local_first = false;
      }
      else
      {
        local_buffer.m_print__Character( 44 );
      }

      local_buffer.m_print__Byte( local_element );
    }

  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardReverseOrderListReader_of_Byte__init_settings()
{
}

function BardReverseOrderListReader_of_Byte__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardReverseOrderListReader_of_Byte__init__ByteList( local__list )
{
  this.p_list = local__list;
  this.p_count = this.p_list.p_count;
  this.p_highest_index = (this.p_count - 1);
  return this;
}

function BardReverseOrderListReader_of_Byte__get__Integer( local_index )
{
  return this.p_list.m_get__Integer( (this.p_highest_index - local_index) );
}

function BardArray_of_Character__init_settings()
{
}

function BardArray_of_Character__init_defaults()
{
  BardGenericArray__init_defaults( this );
  return this;
}

function BardArray_of_Character__create__Integer( local_count )
{
  Bard_throw_missing_return();
}

function BardArray_of_Character__count()
{
  return this.length;
}

function BardArray_of_Character__get__Integer( local_index )
{
  return this[local_index];
}

function BardArray_of_Character__set__Integer_Character( local_index, local_value )
{
  this[local_index] = local_value;
  Bard_throw_missing_return();
}

function BardArray_of_Character__to_String()
{
  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_first;
  local_first = true;
  {
    var local__index_235;
    var local__collection_236;
    local__index_235 = 0;
    local__collection_236 = this;
    while ((local__index_235 < local__collection_236.m_count()))
    {
      var local_element;
      local_element = local__collection_236.m_get__Integer( local__index_235 );
      ++local__index_235;
      if (local_first)
      {
        local_first = false;
      }
      else
      {
        local_buffer.m_print__Character( 44 );
      }

      local_buffer.m_print__Character( local_element );
    }

  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardReverseOrderListReader_of_Character__init_settings()
{
}

function BardReverseOrderListReader_of_Character__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardReverseOrderListReader_of_Character__init__CharacterList( local__list )
{
  this.p_list = local__list;
  this.p_count = this.p_list.p_count;
  this.p_highest_index = (this.p_count - 1);
  return this;
}

function BardReverseOrderListReader_of_Character__get__Integer( local_index )
{
  return this.p_list.m_get__Integer( (this.p_highest_index - local_index) );
}

function BardArray_of_String__init_settings()
{
}

function BardArray_of_String__init_defaults()
{
  BardGenericArray__init_defaults( this );
  return this;
}

function BardArray_of_String__create__Integer( local_count )
{
  Bard_throw_missing_return();
}

function BardArray_of_String__count()
{
  return this.length;
}

function BardArray_of_String__get__Integer( local_index )
{
  return this[local_index];
}

function BardArray_of_String__set__Integer_String( local_index, local_value )
{
  this[local_index] = local_value;
  Bard_throw_missing_return();
}

function BardArray_of_String__to_String()
{
  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_first;
  local_first = true;
  {
    var local__index_269;
    var local__collection_270;
    local__index_269 = 0;
    local__collection_270 = this;
    while ((local__index_269 < local__collection_270.m_count()))
    {
      var local_element;
      local_element = local__collection_270.m_get__Integer( local__index_269 );
      ++local__index_269;
      if (local_first)
      {
        local_first = false;
      }
      else
      {
        local_buffer.m_print__Character( 44 );
      }

      local_buffer.m_print__String( local_element );
    }

  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardReverseOrderListReader_of_String__init_settings()
{
}

function BardReverseOrderListReader_of_String__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardReverseOrderListReader_of_String__init__StringList( local__list )
{
  this.p_list = local__list;
  this.p_count = this.p_list.p_count;
  this.p_highest_index = (this.p_count - 1);
  return this;
}

function BardReverseOrderListReader_of_String__get__Integer( local_index )
{
  return this.p_list.m_get__Integer( (this.p_highest_index - local_index) );
}

function BardTableBin_of_String_String__init()
{
  this.p_entries = BardObjectList__init.call( new BardObjectList().m_init_defaults() );
  return this;
}

function BardTableBin_of_String_String__init_settings()
{
}

function BardTableBin_of_String_String__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardTableBin_of_String_String__find__String_Integer( local_key, local_hash )
{
  {
    var local__index_313;
    var local__collection_314;
    local__index_313 = 0;
    local__collection_314 = this.p_entries;
    while ((local__index_313 < local__collection_314.p_count))
    {
      var local_entry;
      local_entry = local__collection_314.m_get__Integer( local__index_313 );
      ++local__index_313;
      if (((local_entry.p_hash == local_hash) && BardString__equals__String( local_entry.p_key, local_key )))
      {
        return local_entry;
      }

    }

  }

  return null;
}

function BardObjectList__init()
{
  this.p_data = new Array( 10 );
  return this;
}

function BardObjectList__to_String()
{
  if ((this.p_count == 0))
  {
    return "[]";
  }

  var local_buffer;
  local_buffer = BardStringBuilder__init.call( new BardStringBuilder().m_init_defaults() );
  local_buffer.m_print__Character( 91 );
  var local_i;
  local_i = 0;
  var local_limit;
  local_limit = this.p_count;
  while ((local_i < local_limit))
  {
    if ((local_i > 0))
    {
      local_buffer.m_print__Character( 44 );
    }

    local_buffer.m_print__Object( this.m_get__Integer( local_i ) );
    ++local_i;
  }

  local_buffer.m_print__Character( 93 );
  return local_buffer.m_to_String();
}

function BardObjectList__init_settings()
{
}

function BardObjectList__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardObjectList__init__Integer( local_initial_capacity )
{
  this.p_data = new Array( local_initial_capacity );
  return this;
}

function BardObjectList__init__Integer_Object( local_size, local_initial_value )
{
  this.m_init__Integer( local_size );
  while ((local_size > 0))
  {
    this.m_add__Object( local_initial_value );
    --local_size;
  }

  return this;
}

function BardObjectList__init__Array_of_Object( local__data )
{
  this.p_data = local__data;
  this.p_count = this.p_data.length;
  return this;
}

function BardObjectList__add__Object( local_value )
{
  return this.m_insert__Object_Integer( local_value, this.p_count );
}

function BardObjectList__add__ObjectList( local_list )
{
  return this.m_insert__ObjectList_Integer( local_list, this.p_count );
}

function BardObjectList__capacity()
{
  return this.p_data.length;
}

function BardObjectList__clear()
{
  this.m_discard__Integer_Integer( 0, (this.p_count - 1) );
  return this;
}

function BardObjectList__clone()
{
  var local_result;
  local_result = BardObjectList__init__Integer.call( new BardObjectList().m_init_defaults(), this.p_count );
  {
    var local__index_324;
    var local__collection_325;
    local__index_324 = 0;
    local__collection_325 = this;
    while ((local__index_324 < local__collection_325.p_count))
    {
      var local_value;
      local_value = local__collection_325.m_get__Integer( local__index_324 );
      ++local__index_324;
      local_result.m_add__Object( local_value );
    }

  }

  return local_result;
}

function BardObjectList__contains__Object( local_value )
{
  return (this.m_find__Object( local_value ) != -1);
}

function BardObjectList__discard__Integer_Integer( local_i1, local_i2 )
{
  if ((local_i1 > local_i2))
  {
    return;
  }

  var local_original_count;
  local_original_count = this.p_count;
  var local_discard_count;
  local_discard_count = ((local_i2 - local_i1) + 1);
  BardGenericArray__shift__Integer_Integer_Integer( this.p_data, (local_i2 + 1), this.p_count, -local_discard_count );
  this.p_count -= local_discard_count;
  BardGenericArray__clear_references__Integer_Integer( this.p_data, this.p_count, (local_original_count - 1) );
}

function BardObjectList__discard_from__Integer( local_i1 )
{
  this.m_discard__Integer_Integer( local_i1, (this.p_count - 1) );
}

function BardObjectList__ensure_capacity__Integer( local_desired_capacity )
{
  var local_cur_capacity;
  local_cur_capacity = this.p_data.length;
  if ((local_desired_capacity > local_cur_capacity))
  {
    var local_new_capacity;
    local_new_capacity = (local_cur_capacity + local_cur_capacity);
    if ((local_desired_capacity > local_new_capacity))
    {
      local_new_capacity = local_desired_capacity;
    }

    var local_new_data;
    local_new_data = new Array( local_new_capacity );
    var local_i;
    local_i = 0;
    while ((local_i < this.p_count))
    {
      local_new_data[local_i] = this.p_data[local_i];
      ++local_i;
    }

    this.p_data = local_new_data;
  }

  return this;
}

function BardObjectList__find__Object( local_value )
{
  return this.m_find__Object_Integer( local_value, 0 );
}

function BardObjectList__find__Object_Integer( local_value, local_i1 )
{
  var local_c;
  local_c = this.p_count;
  while ((local_i1 < this.p_count))
  {
    if (this.m_get__Integer( local_i1 ).m_equals__Object( local_value ))
    {
      return local_i1;
    }

    ++local_i1;
  }

  return -1;
}

function BardObjectList__first()
{
  return this.m_get__Integer( 0 );
}

function BardObjectList__get__Integer( local_index )
{
  return this.p_data[local_index];
}

function BardObjectList__indices()
{
  return BardListIndexReader__init__Integer.call( new BardListIndexReader().m_init_defaults(), this.p_count );
}

function BardObjectList__insert__Object( local_value )
{
  return this.m_insert__Object_Integer( local_value, 0 );
}

function BardObjectList__insert__ObjectList( local_list )
{
  return this.m_insert__ObjectList_Integer( local_list, 0 );
}

function BardObjectList__insert__Object_Integer( local_value, local_before_index )
{
  if ((local_before_index == this.p_count))
  {
    if ((this.p_data == null))
    {
      this.p_data = new Array( 10 );
    }
    else if ((this.p_count == this.p_data.length))
    {
      if ((this.p_count < 10))
      {
        this.m_ensure_capacity__Integer( 10 );
      }
      else
      {
        this.m_ensure_capacity__Integer( (this.p_count * 2) );
      }

    }

    this.p_data[this.p_count] = local_value;
    ++this.p_count;
    return this;
  }
  else
  {
    this.m_add__Object( local_value );
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 2), 1 );
    this.p_data[local_before_index] = local_value;
    return this;
  }

  Bard_throw_missing_return();
}

function BardObjectList__insert__ObjectList_Integer( local_list, local_before_index )
{
  var local_required_capacity;
  local_required_capacity = (this.p_count + local_list.p_count);
  if ((this.p_data == null))
  {
    this.p_data = new Array( local_required_capacity );
  }
  else
  {
    if ((local_required_capacity < 10))
    {
      local_required_capacity = 10;
    }

  }

  this.m_ensure_capacity__Integer( local_required_capacity );
  var local_src_i;
  local_src_i = 0;
  var local_dest_i;
  local_dest_i = local_before_index;
  var local_other_count;
  local_other_count = local_list.p_count;
  var local_other_data;
  local_other_data = local_list.p_data;
  if ((local_before_index < this.p_count))
  {
    BardGenericArray__shift__Integer_Integer_Integer( this.p_data, local_before_index, (this.p_count - 1), local_other_count );
  }

  while ((local_src_i < local_other_count))
  {
    this.p_data[local_dest_i] = local_other_data[local_src_i];
    ++local_src_i;
    ++local_dest_i;
  }

  this.p_count += local_other_count;
  return this;
}

function BardObjectList__last()
{
  return this.m_get__Integer( (this.p_count - 1) );
}

function BardObjectList__random()
{
  return this.m_get__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardObjectList__remove__Object( local_value )
{
  var local_i;
  local_i = 0;
  var local_c;
  local_c = this.p_count;
  while ((local_i < local_c))
  {
    if ((this.m_get__Integer( local_i ) == local_value))
    {
      this.m_remove_at__Integer( local_i );
      return true;
    }

    ++local_i;
  }

  return false;
}

function BardObjectList__remove_at__Integer( local_index )
{
  var local_result;
  local_result = this.m_get__Integer( local_index );
  this.m_discard__Integer_Integer( local_index, local_index );
  return local_result;
}

function BardObjectList__remove_first()
{
  return this.m_remove_at__Integer( 0 );
}

function BardObjectList__remove_last()
{
  return this.m_remove_at__Integer( (this.p_count - 1) );
}

function BardObjectList__remove_random()
{
  return this.m_remove_at__Integer( BardSingletons.singleton_Random.m_to_Integer__Integer( this.p_count ) );
}

function BardObjectList__reverse()
{
  this.m_reverse__Integer_Integer( 0, (this.p_count - 1) );
}

function BardObjectList__reverse__Integer_Integer( local_i1, local_i2 )
{
  var local_i2;
  local_i2 = (this.p_count - 1);
  while ((local_i1 < local_i2))
  {
    var local_temp;
    local_temp = this.m_get__Integer( local_i1 );
    this.m_set__Integer_Object( local_i1, this.m_get__Integer( local_i2 ) );
    this.m_set__Integer_Object( local_i2, local_temp );
    ++local_i1;
    --local_i2;
  }

}

function BardObjectList__reverse_order()
{
  return BardReverseOrderListReader_of_Object__init__ObjectList.call( new BardReverseOrderListReader_of_Object().m_init_defaults(), this );
}

function BardObjectList__set__Integer_Object( local_index, local_value )
{
  this.p_data[local_index] = local_value;
}

function BardObjectList__shuffle()
{
  var local_limit;
  local_limit = this.p_count;
  var local_i;
  local_i = (local_limit - 1);
  while ((local_i >= 0))
  {
    var local_r;
    local_r = BardSingletons.singleton_Random.m_to_Integer__Integer( local_limit );
    var local_temp;
    local_temp = this.m_get__Integer( local_i );
    this.m_set__Integer_Object( local_i, this.m_get__Integer( local_r ) );
    this.m_set__Integer_Object( local_r, local_temp );
    --local_i;
  }

}

function BardObjectList__swap__Integer_Integer( local_i1, local_i2 )
{
  var local_temp;
  local_temp = this.m_get__Integer( local_i1 );
  this.m_set__Integer_Object( local_i1, this.m_get__Integer( local_i2 ) );
  this.m_set__Integer_Object( local_i2, local_temp );
}

function BardReverseOrderListReader_of_Object__init_settings()
{
}

function BardReverseOrderListReader_of_Object__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardReverseOrderListReader_of_Object__init__ObjectList( local__list )
{
  this.p_list = local__list;
  this.p_count = this.p_list.p_count;
  this.p_highest_index = (this.p_count - 1);
  return this;
}

function BardReverseOrderListReader_of_Object__get__Integer( local_index )
{
  return this.p_list.m_get__Integer( (this.p_highest_index - local_index) );
}

function BardRandom__init()
{
  this.m_reset__Real( (BardSingletons.singleton_Time.m_current() * 1000.0) );
  return this;
}

function BardRandom__to_Real()
{
  this.p_state = this.m_next_state__Real( this.p_state );
  return ((this.p_state + 1.0) / 281474976710657.0);
}

function BardRandom__to_Integer()
{
  return ((this.m_to_Real() * 2147483648.0)|0);
}

function BardRandom__to_Logical()
{
  return (this.m_to_Real() > 0.5);
}

function BardRandom__init_settings()
{
}

function BardRandom__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

function BardRandom__init__Real( local_seed )
{
  this.m_reset__Real( local_seed );
  return this;
}

// NATIVE function BardRandom__next_state__Real( local_state )

function BardRandom__reset__Real( local__state )
{
  this.p_state = local__state;
}

function BardRandom__to_Real__Real_Real( local_low, local_high )
{
  return ((this.m_to_Real() * (local_high - local_low)) + local_low);
}

function BardRandom__to_Integer__Integer( local_limit )
{
  if ((local_limit == 0))
  {
    return 0;
  }

  return Bard_mod_integer( this.m_to_Integer(), local_limit );
}

function BardRandom__to_Integer__Integer_Integer( local_low, local_high )
{
  return (this.m_to_Integer__Integer( ((local_high - local_low) + 1) ) + local_low);
}

function BardTime__init_settings()
{
}

function BardTime__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

// NATIVE function BardTime__current()

function BardTime__update()
{
  if ((this.p_previous_time == 0.0))
  {
    this.p_previous_time = this.m_current();
  }

  var local_current_time;
  local_current_time = this.m_current();
  this.p_elapsed = (local_current_time - this.p_previous_time);
  this.p_previous_time = local_current_time;
}

function BardMath__init_settings()
{
}

function BardMath__init_defaults()
{
  BardObject__init_defaults.call( this );
  return this;
}

// NATIVE function BardMath__floor__Real( local_value )

// NATIVE function BardMath__sqrt__Real( local_value )

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
function BardObject()
{
}

BardObject.prototype.m_create_instance = BardObject__create_instance;
BardObject.prototype.m_init = BardObject__init;
BardObject.prototype.m_hash_code = BardObject__hash_code;
BardObject.prototype.m_equals__Object = BardObject__equals__Object;
BardObject.prototype.m_to_Real = BardObject__to_Real;
BardObject.prototype.m_to_Integer = BardObject__to_Integer;
BardObject.prototype.m_to_Character = BardObject__to_Character;
BardObject.prototype.m_to_Byte = BardObject__to_Byte;
BardObject.prototype.m_to_Logical = BardObject__to_Logical;
BardObject.prototype.m_to_String = BardObject__to_String;
BardObject.prototype.m_init_settings = BardObject__init_settings;
BardObject.prototype.m_init_defaults = BardObject__init_defaults;

function BardString()
{
}

BardString.prototype.m_create = BardString__create;
BardString.prototype.m_create__ByteList = BardString__create__ByteList;
BardString.prototype.m_create__CharacterList = BardString__create__CharacterList;
BardString.prototype.m_after_first__Character = BardString__after_first__Character;
BardString.prototype.m_after_first__String = BardString__after_first__String;
BardString.prototype.m_before_first__Character = BardString__before_first__Character;
BardString.prototype.m_before_first__String = BardString__before_first__String;
BardString.prototype.m_begins_with__Character = BardString__begins_with__Character;
BardString.prototype.m_begins_with__String = BardString__begins_with__String;
BardString.prototype.m_compare_to__String = BardString__compare_to__String;
BardString.prototype.m_clone = BardString__clone;
BardString.prototype.m_contains__Character = BardString__contains__Character;
BardString.prototype.m_contains__String = BardString__contains__String;
BardString.prototype.m_count = BardString__count;
BardString.prototype.m_equals__Object = BardString__equals__Object;
BardString.prototype.m_equals__String = BardString__equals__String;
BardString.prototype.m_ends_with__Character = BardString__ends_with__Character;
BardString.prototype.m_ends_with__String = BardString__ends_with__String;
BardString.prototype.m_find__Character = BardString__find__Character;
BardString.prototype.m_find__Character_Integer = BardString__find__Character_Integer;
BardString.prototype.m_find__String = BardString__find__String;
BardString.prototype.m_find__String_Integer = BardString__find__String_Integer;
BardString.prototype.m_find_last__Character = BardString__find_last__Character;
BardString.prototype.m_find_last__Character_Integer = BardString__find_last__Character_Integer;
BardString.prototype.m_find_last__String = BardString__find_last__String;
BardString.prototype.m_find_last__String_Integer = BardString__find_last__String_Integer;
BardString.prototype.m_from_first__Character = BardString__from_first__Character;
BardString.prototype.m_from_first__String = BardString__from_first__String;
BardString.prototype.m_get__Integer = BardString__get__Integer;
BardString.prototype.m_hash_code = BardString__hash_code;
BardString.prototype.m_leftmost__Integer = BardString__leftmost__Integer;
BardString.prototype.m_operatorPLUSEQUALS__String = BardString__operatorPLUSEQUALS__String;
BardString.prototype.m_operatorTIMESEQUALS__Integer = BardString__operatorTIMESEQUALS__Integer;
BardString.prototype.m_operatorTIMES__String_Integer = BardString__operatorTIMES__String_Integer;
BardString.prototype.m_print__Integer = BardString__print__Integer;
BardString.prototype.m_print__Logical = BardString__print__Logical;
BardString.prototype.m_print__Real = BardString__print__Real;
BardString.prototype.m_print__Object = BardString__print__Object;
BardString.prototype.m_print__String = BardString__print__String;
BardString.prototype.m_reversed = BardString__reversed;
BardString.prototype.m_rightmost__Integer = BardString__rightmost__Integer;
BardString.prototype.m_split__Character = BardString__split__Character;
BardString.prototype.m_substring__Integer = BardString__substring__Integer;
BardString.prototype.m_substring__Integer_Integer = BardString__substring__Integer_Integer;
BardString.prototype.m_substring_equals__Integer_Integer_String = BardString__substring_equals__Integer_Integer_String;
BardString.prototype.m_to_String = BardString__to_String;
BardString.prototype.m_to_Integer = BardString__to_Integer;
BardString.prototype.m_to_uppercase = BardString__to_uppercase;
BardString.prototype.m_to_lowercase = BardString__to_lowercase;
BardString.prototype.m_trim = BardString__trim;
BardString.prototype.m_is_numeric__Character = BardString__is_numeric__Character;
BardString.prototype.m_to_ByteList = BardString__to_ByteList;
BardString.prototype.m_init_settings = BardString__init_settings;
BardString.prototype.m_init_defaults = BardString__init_defaults;

function BardStringBuilder()
{
  this.p_characters = null;
  this.p_indent = 0;
  this.p_hash_code = 0;

}

BardStringBuilder.prototype.m_create_instance = BardObject__create_instance;
BardStringBuilder.prototype.m_init = BardStringBuilder__init;
BardStringBuilder.prototype.m_hash_code = BardObject__hash_code;
BardStringBuilder.prototype.m_equals__Object = BardObject__equals__Object;
BardStringBuilder.prototype.m_to_Real = BardObject__to_Real;
BardStringBuilder.prototype.m_to_Integer = BardObject__to_Integer;
BardStringBuilder.prototype.m_to_Character = BardObject__to_Character;
BardStringBuilder.prototype.m_to_Byte = BardObject__to_Byte;
BardStringBuilder.prototype.m_to_Logical = BardObject__to_Logical;
BardStringBuilder.prototype.m_to_String = BardStringBuilder__to_String;
BardStringBuilder.prototype.m_init_settings = BardStringBuilder__init_settings;
BardStringBuilder.prototype.m_init_defaults = BardStringBuilder__init_defaults;
BardStringBuilder.prototype.m_print__String = BardStringBuilder__print__String;
BardStringBuilder.prototype.m_print__Real = BardStringBuilder__print__Real;
BardStringBuilder.prototype.m_print__Real_Integer = BardStringBuilder__print__Real_Integer;
BardStringBuilder.prototype.m_print__Integer = BardStringBuilder__print__Integer;
BardStringBuilder.prototype.m_print__Character = BardStringBuilder__print__Character;
BardStringBuilder.prototype.m_print__Object = BardStringBuilder__print__Object;
BardStringBuilder.prototype.m_print__Byte = BardStringBuilder__print__Byte;
BardStringBuilder.prototype.m_print__Logical = BardStringBuilder__print__Logical;
BardStringBuilder.prototype.m_println__Object = BardStringBuilder__println__Object;
BardStringBuilder.prototype.m_println__String = BardStringBuilder__println__String;
BardStringBuilder.prototype.m_println__Real = BardStringBuilder__println__Real;
BardStringBuilder.prototype.m_println__Real_Integer = BardStringBuilder__println__Real_Integer;
BardStringBuilder.prototype.m_println__Integer = BardStringBuilder__println__Integer;
BardStringBuilder.prototype.m_println__Character = BardStringBuilder__println__Character;
BardStringBuilder.prototype.m_println__Byte = BardStringBuilder__println__Byte;
BardStringBuilder.prototype.m_println__Logical = BardStringBuilder__println__Logical;
BardStringBuilder.prototype.m_println = BardStringBuilder__println;
BardStringBuilder.prototype.m_init__Integer = BardStringBuilder__init__Integer;
BardStringBuilder.prototype.m_init__String = BardStringBuilder__init__String;
BardStringBuilder.prototype.m_clone = BardStringBuilder__clone;
BardStringBuilder.prototype.m_count = BardStringBuilder__count;
BardStringBuilder.prototype.m_at_newline = BardStringBuilder__at_newline;
BardStringBuilder.prototype.m_get__Integer = BardStringBuilder__get__Integer;
BardStringBuilder.prototype.m_indent = BardStringBuilder__indent;
BardStringBuilder.prototype.m_indent__Integer = BardStringBuilder__indent__Integer;
BardStringBuilder.prototype.m_pad_with__Character_Integer = BardStringBuilder__pad_with__Character_Integer;
BardStringBuilder.prototype.m_capacity = BardStringBuilder__capacity;
BardStringBuilder.prototype.m_clear = BardStringBuilder__clear;
BardStringBuilder.prototype.m_last = BardStringBuilder__last;
BardStringBuilder.prototype.m_perform_indent = BardStringBuilder__perform_indent;
BardStringBuilder.prototype.m_print__StringBuilder = BardStringBuilder__print__StringBuilder;
BardStringBuilder.prototype.m_reverse = BardStringBuilder__reverse;
BardStringBuilder.prototype.m_reverse__Integer_Integer = BardStringBuilder__reverse__Integer_Integer;
BardStringBuilder.prototype.m_set__Integer_Character = BardStringBuilder__set__Integer_Character;
BardStringBuilder.prototype.m_update_hash_code = BardStringBuilder__update_hash_code;
BardStringBuilder.prototype.m_write__Character = BardStringBuilder__write__Character;
BardStringBuilder.prototype.m_write__String = BardStringBuilder__write__String;
BardStringBuilder.prototype.m_write__StringBuilder = BardStringBuilder__write__StringBuilder;

function BardGenericArray()
{
}

BardGenericArray.prototype.m_clear_references__Integer_Integer = BardGenericArray__clear_references__Integer_Integer;
BardGenericArray.prototype.m_shift__Integer_Integer_Integer = BardGenericArray__shift__Integer_Integer_Integer;
BardGenericArray.prototype.m_init_settings = BardGenericArray__init_settings;
BardGenericArray.prototype.m_init_defaults = BardGenericArray__init_defaults;

function BardArray_of_Object()
{
}

BardArray_of_Object.prototype.m_clear_references__Integer_Integer = BardGenericArray__clear_references__Integer_Integer;
BardArray_of_Object.prototype.m_shift__Integer_Integer_Integer = BardGenericArray__shift__Integer_Integer_Integer;
BardArray_of_Object.prototype.m_init_settings = BardArray_of_Object__init_settings;
BardArray_of_Object.prototype.m_init_defaults = BardArray_of_Object__init_defaults;
BardArray_of_Object.prototype.m_create__Integer = BardArray_of_Object__create__Integer;
BardArray_of_Object.prototype.m_count = BardArray_of_Object__count;
BardArray_of_Object.prototype.m_get__Integer = BardArray_of_Object__get__Integer;
BardArray_of_Object.prototype.m_set__Integer_Object = BardArray_of_Object__set__Integer_Object;
BardArray_of_Object.prototype.m_to_String = BardArray_of_Object__to_String;

function BardException()
{
  this.p_message = null;

}

BardException.prototype.m_create_instance = BardObject__create_instance;
BardException.prototype.m_init = BardException__init;
BardException.prototype.m_hash_code = BardObject__hash_code;
BardException.prototype.m_equals__Object = BardObject__equals__Object;
BardException.prototype.m_to_Real = BardObject__to_Real;
BardException.prototype.m_to_Integer = BardObject__to_Integer;
BardException.prototype.m_to_Character = BardObject__to_Character;
BardException.prototype.m_to_Byte = BardObject__to_Byte;
BardException.prototype.m_to_Logical = BardObject__to_Logical;
BardException.prototype.m_to_String = BardException__to_String;
BardException.prototype.m_init_settings = BardException__init_settings;
BardException.prototype.m_init_defaults = BardException__init_defaults;
BardException.prototype.m_init__String = BardException__init__String;

function BardGlobal()
{
}

BardGlobal.prototype.m_create_instance = BardObject__create_instance;
BardGlobal.prototype.m_init = BardObject__init;
BardGlobal.prototype.m_hash_code = BardObject__hash_code;
BardGlobal.prototype.m_equals__Object = BardObject__equals__Object;
BardGlobal.prototype.m_to_Real = BardObject__to_Real;
BardGlobal.prototype.m_to_Integer = BardObject__to_Integer;
BardGlobal.prototype.m_to_Character = BardObject__to_Character;
BardGlobal.prototype.m_to_Byte = BardObject__to_Byte;
BardGlobal.prototype.m_to_Logical = BardObject__to_Logical;
BardGlobal.prototype.m_to_String = BardObject__to_String;
BardGlobal.prototype.m_init_settings = BardGlobal__init_settings;
BardGlobal.prototype.m_init_defaults = BardGlobal__init_defaults;
BardGlobal.prototype.m_print__String = BardGlobal__print__String;
BardGlobal.prototype.m_print__Real = BardGlobal__print__Real;
BardGlobal.prototype.m_print__Real_Integer = BardGlobal__print__Real_Integer;
BardGlobal.prototype.m_print__Integer = BardGlobal__print__Integer;
BardGlobal.prototype.m_print__Character = BardGlobal__print__Character;
BardGlobal.prototype.m_print__Object = BardGlobal__print__Object;
BardGlobal.prototype.m_print__Byte = BardGlobal__print__Byte;
BardGlobal.prototype.m_print__Logical = BardGlobal__print__Logical;
BardGlobal.prototype.m_println__Object = BardGlobal__println__Object;
BardGlobal.prototype.m_println__String = BardGlobal__println__String;
BardGlobal.prototype.m_println__Real = BardGlobal__println__Real;
BardGlobal.prototype.m_println__Real_Integer = BardGlobal__println__Real_Integer;
BardGlobal.prototype.m_println__Integer = BardGlobal__println__Integer;
BardGlobal.prototype.m_println__Character = BardGlobal__println__Character;
BardGlobal.prototype.m_println__Byte = BardGlobal__println__Byte;
BardGlobal.prototype.m_println__Logical = BardGlobal__println__Logical;
BardGlobal.prototype.m_println = BardGlobal__println;

function BardMain()
{
  this.p_position = {'p_x':0,'p_y':0};

}

BardMain.prototype.m_create_instance = BardObject__create_instance;
BardMain.prototype.m_init = BardMain__init;
BardMain.prototype.m_hash_code = BardObject__hash_code;
BardMain.prototype.m_equals__Object = BardObject__equals__Object;
BardMain.prototype.m_to_Real = BardObject__to_Real;
BardMain.prototype.m_to_Integer = BardObject__to_Integer;
BardMain.prototype.m_to_Character = BardObject__to_Character;
BardMain.prototype.m_to_Byte = BardObject__to_Byte;
BardMain.prototype.m_to_Logical = BardObject__to_Logical;
BardMain.prototype.m_to_String = BardObject__to_String;
BardMain.prototype.m_init_settings = BardMain__init_settings;
BardMain.prototype.m_init_defaults = BardMain__init_defaults;
BardMain.prototype.m_print_variations__String = BardMain__print_variations__String;
BardMain.prototype.m_crazy_caps__String = BardMain__crazy_caps__String;

function BardTable_of_String_String()
{
  this.p_bin_mask = 0;
  this.p_bins = null;
  this.p_keys = null;

}

BardTable_of_String_String.prototype.m_create_instance = BardObject__create_instance;
BardTable_of_String_String.prototype.m_init = BardTable_of_String_String__init;
BardTable_of_String_String.prototype.m_hash_code = BardObject__hash_code;
BardTable_of_String_String.prototype.m_equals__Object = BardObject__equals__Object;
BardTable_of_String_String.prototype.m_to_Real = BardObject__to_Real;
BardTable_of_String_String.prototype.m_to_Integer = BardObject__to_Integer;
BardTable_of_String_String.prototype.m_to_Character = BardObject__to_Character;
BardTable_of_String_String.prototype.m_to_Byte = BardObject__to_Byte;
BardTable_of_String_String.prototype.m_to_Logical = BardObject__to_Logical;
BardTable_of_String_String.prototype.m_to_String = BardTable_of_String_String__to_String;
BardTable_of_String_String.prototype.m_init_settings = BardTable_of_String_String__init_settings;
BardTable_of_String_String.prototype.m_init_defaults = BardTable_of_String_String__init_defaults;
BardTable_of_String_String.prototype.m_init__Integer = BardTable_of_String_String__init__Integer;
BardTable_of_String_String.prototype.m_at__Integer = BardTable_of_String_String__at__Integer;
BardTable_of_String_String.prototype.m_contains__String = BardTable_of_String_String__contains__String;
BardTable_of_String_String.prototype.m_count = BardTable_of_String_String__count;
BardTable_of_String_String.prototype.m_find__String = BardTable_of_String_String__find__String;
BardTable_of_String_String.prototype.m_get__String = BardTable_of_String_String__get__String;
BardTable_of_String_String.prototype.m_set__String_String = BardTable_of_String_String__set__String_String;
BardTable_of_String_String.prototype.m_print_to__StringBuilder = BardTable_of_String_String__print_to__StringBuilder;

function BardByteList()
{
  this.p_data = null;
  this.p_count = 0;

}

BardByteList.prototype.m_create_instance = BardObject__create_instance;
BardByteList.prototype.m_init = BardByteList__init;
BardByteList.prototype.m_hash_code = BardObject__hash_code;
BardByteList.prototype.m_equals__Object = BardObject__equals__Object;
BardByteList.prototype.m_to_Real = BardObject__to_Real;
BardByteList.prototype.m_to_Integer = BardObject__to_Integer;
BardByteList.prototype.m_to_Character = BardObject__to_Character;
BardByteList.prototype.m_to_Byte = BardObject__to_Byte;
BardByteList.prototype.m_to_Logical = BardObject__to_Logical;
BardByteList.prototype.m_to_String = BardByteList__to_String;
BardByteList.prototype.m_init_settings = BardByteList__init_settings;
BardByteList.prototype.m_init_defaults = BardByteList__init_defaults;
BardByteList.prototype.m_init__Integer = BardByteList__init__Integer;
BardByteList.prototype.m_init__Integer_Byte = BardByteList__init__Integer_Byte;
BardByteList.prototype.m_init__Array_of_Byte = BardByteList__init__Array_of_Byte;
BardByteList.prototype.m_add__Byte = BardByteList__add__Byte;
BardByteList.prototype.m_add__ByteList = BardByteList__add__ByteList;
BardByteList.prototype.m_capacity = BardByteList__capacity;
BardByteList.prototype.m_clear = BardByteList__clear;
BardByteList.prototype.m_clone = BardByteList__clone;
BardByteList.prototype.m_contains__Byte = BardByteList__contains__Byte;
BardByteList.prototype.m_discard__Integer_Integer = BardByteList__discard__Integer_Integer;
BardByteList.prototype.m_discard_from__Integer = BardByteList__discard_from__Integer;
BardByteList.prototype.m_ensure_capacity__Integer = BardByteList__ensure_capacity__Integer;
BardByteList.prototype.m_find__Byte = BardByteList__find__Byte;
BardByteList.prototype.m_find__Byte_Integer = BardByteList__find__Byte_Integer;
BardByteList.prototype.m_first = BardByteList__first;
BardByteList.prototype.m_get__Integer = BardByteList__get__Integer;
BardByteList.prototype.m_indices = BardByteList__indices;
BardByteList.prototype.m_insert__Byte = BardByteList__insert__Byte;
BardByteList.prototype.m_insert__ByteList = BardByteList__insert__ByteList;
BardByteList.prototype.m_insert__Byte_Integer = BardByteList__insert__Byte_Integer;
BardByteList.prototype.m_insert__ByteList_Integer = BardByteList__insert__ByteList_Integer;
BardByteList.prototype.m_last = BardByteList__last;
BardByteList.prototype.m_random = BardByteList__random;
BardByteList.prototype.m_remove__Byte = BardByteList__remove__Byte;
BardByteList.prototype.m_remove_at__Integer = BardByteList__remove_at__Integer;
BardByteList.prototype.m_remove_first = BardByteList__remove_first;
BardByteList.prototype.m_remove_last = BardByteList__remove_last;
BardByteList.prototype.m_remove_random = BardByteList__remove_random;
BardByteList.prototype.m_reverse = BardByteList__reverse;
BardByteList.prototype.m_reverse__Integer_Integer = BardByteList__reverse__Integer_Integer;
BardByteList.prototype.m_reverse_order = BardByteList__reverse_order;
BardByteList.prototype.m_set__Integer_Byte = BardByteList__set__Integer_Byte;
BardByteList.prototype.m_shuffle = BardByteList__shuffle;
BardByteList.prototype.m_swap__Integer_Integer = BardByteList__swap__Integer_Integer;

function BardCharacterList()
{
  this.p_data = null;
  this.p_count = 0;

}

BardCharacterList.prototype.m_create_instance = BardObject__create_instance;
BardCharacterList.prototype.m_init = BardCharacterList__init;
BardCharacterList.prototype.m_hash_code = BardObject__hash_code;
BardCharacterList.prototype.m_equals__Object = BardObject__equals__Object;
BardCharacterList.prototype.m_to_Real = BardObject__to_Real;
BardCharacterList.prototype.m_to_Integer = BardObject__to_Integer;
BardCharacterList.prototype.m_to_Character = BardObject__to_Character;
BardCharacterList.prototype.m_to_Byte = BardObject__to_Byte;
BardCharacterList.prototype.m_to_Logical = BardObject__to_Logical;
BardCharacterList.prototype.m_to_String = BardCharacterList__to_String;
BardCharacterList.prototype.m_init_settings = BardCharacterList__init_settings;
BardCharacterList.prototype.m_init_defaults = BardCharacterList__init_defaults;
BardCharacterList.prototype.m_init__Integer = BardCharacterList__init__Integer;
BardCharacterList.prototype.m_init__Integer_Character = BardCharacterList__init__Integer_Character;
BardCharacterList.prototype.m_init__Array_of_Character = BardCharacterList__init__Array_of_Character;
BardCharacterList.prototype.m_add__Character = BardCharacterList__add__Character;
BardCharacterList.prototype.m_add__CharacterList = BardCharacterList__add__CharacterList;
BardCharacterList.prototype.m_capacity = BardCharacterList__capacity;
BardCharacterList.prototype.m_clear = BardCharacterList__clear;
BardCharacterList.prototype.m_clone = BardCharacterList__clone;
BardCharacterList.prototype.m_contains__Character = BardCharacterList__contains__Character;
BardCharacterList.prototype.m_discard__Integer_Integer = BardCharacterList__discard__Integer_Integer;
BardCharacterList.prototype.m_discard_from__Integer = BardCharacterList__discard_from__Integer;
BardCharacterList.prototype.m_ensure_capacity__Integer = BardCharacterList__ensure_capacity__Integer;
BardCharacterList.prototype.m_find__Character = BardCharacterList__find__Character;
BardCharacterList.prototype.m_find__Character_Integer = BardCharacterList__find__Character_Integer;
BardCharacterList.prototype.m_first = BardCharacterList__first;
BardCharacterList.prototype.m_get__Integer = BardCharacterList__get__Integer;
BardCharacterList.prototype.m_indices = BardCharacterList__indices;
BardCharacterList.prototype.m_insert__Character = BardCharacterList__insert__Character;
BardCharacterList.prototype.m_insert__CharacterList = BardCharacterList__insert__CharacterList;
BardCharacterList.prototype.m_insert__Character_Integer = BardCharacterList__insert__Character_Integer;
BardCharacterList.prototype.m_insert__CharacterList_Integer = BardCharacterList__insert__CharacterList_Integer;
BardCharacterList.prototype.m_last = BardCharacterList__last;
BardCharacterList.prototype.m_random = BardCharacterList__random;
BardCharacterList.prototype.m_remove__Character = BardCharacterList__remove__Character;
BardCharacterList.prototype.m_remove_at__Integer = BardCharacterList__remove_at__Integer;
BardCharacterList.prototype.m_remove_first = BardCharacterList__remove_first;
BardCharacterList.prototype.m_remove_last = BardCharacterList__remove_last;
BardCharacterList.prototype.m_remove_random = BardCharacterList__remove_random;
BardCharacterList.prototype.m_reverse = BardCharacterList__reverse;
BardCharacterList.prototype.m_reverse__Integer_Integer = BardCharacterList__reverse__Integer_Integer;
BardCharacterList.prototype.m_reverse_order = BardCharacterList__reverse_order;
BardCharacterList.prototype.m_set__Integer_Character = BardCharacterList__set__Integer_Character;
BardCharacterList.prototype.m_shuffle = BardCharacterList__shuffle;
BardCharacterList.prototype.m_swap__Integer_Integer = BardCharacterList__swap__Integer_Integer;

function BardStringList()
{
  this.p_data = null;
  this.p_count = 0;

}

BardStringList.prototype.m_create_instance = BardObject__create_instance;
BardStringList.prototype.m_init = BardStringList__init;
BardStringList.prototype.m_hash_code = BardObject__hash_code;
BardStringList.prototype.m_equals__Object = BardObject__equals__Object;
BardStringList.prototype.m_to_Real = BardObject__to_Real;
BardStringList.prototype.m_to_Integer = BardObject__to_Integer;
BardStringList.prototype.m_to_Character = BardObject__to_Character;
BardStringList.prototype.m_to_Byte = BardObject__to_Byte;
BardStringList.prototype.m_to_Logical = BardObject__to_Logical;
BardStringList.prototype.m_to_String = BardStringList__to_String;
BardStringList.prototype.m_init_settings = BardStringList__init_settings;
BardStringList.prototype.m_init_defaults = BardStringList__init_defaults;
BardStringList.prototype.m_init__Integer = BardStringList__init__Integer;
BardStringList.prototype.m_init__Integer_String = BardStringList__init__Integer_String;
BardStringList.prototype.m_init__Array_of_String = BardStringList__init__Array_of_String;
BardStringList.prototype.m_add__String = BardStringList__add__String;
BardStringList.prototype.m_add__StringList = BardStringList__add__StringList;
BardStringList.prototype.m_capacity = BardStringList__capacity;
BardStringList.prototype.m_clear = BardStringList__clear;
BardStringList.prototype.m_clone = BardStringList__clone;
BardStringList.prototype.m_contains__String = BardStringList__contains__String;
BardStringList.prototype.m_discard__Integer_Integer = BardStringList__discard__Integer_Integer;
BardStringList.prototype.m_discard_from__Integer = BardStringList__discard_from__Integer;
BardStringList.prototype.m_ensure_capacity__Integer = BardStringList__ensure_capacity__Integer;
BardStringList.prototype.m_find__String = BardStringList__find__String;
BardStringList.prototype.m_find__String_Integer = BardStringList__find__String_Integer;
BardStringList.prototype.m_first = BardStringList__first;
BardStringList.prototype.m_get__Integer = BardStringList__get__Integer;
BardStringList.prototype.m_indices = BardStringList__indices;
BardStringList.prototype.m_insert__String = BardStringList__insert__String;
BardStringList.prototype.m_insert__StringList = BardStringList__insert__StringList;
BardStringList.prototype.m_insert__String_Integer = BardStringList__insert__String_Integer;
BardStringList.prototype.m_insert__StringList_Integer = BardStringList__insert__StringList_Integer;
BardStringList.prototype.m_last = BardStringList__last;
BardStringList.prototype.m_random = BardStringList__random;
BardStringList.prototype.m_remove__String = BardStringList__remove__String;
BardStringList.prototype.m_remove_at__Integer = BardStringList__remove_at__Integer;
BardStringList.prototype.m_remove_first = BardStringList__remove_first;
BardStringList.prototype.m_remove_last = BardStringList__remove_last;
BardStringList.prototype.m_remove_random = BardStringList__remove_random;
BardStringList.prototype.m_reverse = BardStringList__reverse;
BardStringList.prototype.m_reverse__Integer_Integer = BardStringList__reverse__Integer_Integer;
BardStringList.prototype.m_reverse_order = BardStringList__reverse_order;
BardStringList.prototype.m_set__Integer_String = BardStringList__set__Integer_String;
BardStringList.prototype.m_shuffle = BardStringList__shuffle;
BardStringList.prototype.m_swap__Integer_Integer = BardStringList__swap__Integer_Integer;

function BardPrinter()
{
}

BardPrinter.prototype.m_create_instance = BardObject__create_instance;
BardPrinter.prototype.m_init = BardObject__init;
BardPrinter.prototype.m_hash_code = BardObject__hash_code;
BardPrinter.prototype.m_equals__Object = BardObject__equals__Object;
BardPrinter.prototype.m_to_Real = BardObject__to_Real;
BardPrinter.prototype.m_to_Integer = BardObject__to_Integer;
BardPrinter.prototype.m_to_Character = BardObject__to_Character;
BardPrinter.prototype.m_to_Byte = BardObject__to_Byte;
BardPrinter.prototype.m_to_Logical = BardObject__to_Logical;
BardPrinter.prototype.m_to_String = BardObject__to_String;
BardPrinter.prototype.m_init_settings = BardPrinter__init_settings;
BardPrinter.prototype.m_init_defaults = BardObject__init_defaults;
BardPrinter.prototype.m_print__String = BardPrinter__print__String;
BardPrinter.prototype.m_print__Real = BardPrinter__print__Real;
BardPrinter.prototype.m_print__Real_Integer = BardPrinter__print__Real_Integer;
BardPrinter.prototype.m_print__Integer = BardPrinter__print__Integer;
BardPrinter.prototype.m_print__Character = BardPrinter__print__Character;
BardPrinter.prototype.m_print__Object = BardPrinter__print__Object;
BardPrinter.prototype.m_print__Byte = BardPrinter__print__Byte;
BardPrinter.prototype.m_print__Logical = BardPrinter__print__Logical;
BardPrinter.prototype.m_println__Object = BardPrinter__println__Object;
BardPrinter.prototype.m_println__String = BardPrinter__println__String;
BardPrinter.prototype.m_println__Real = BardPrinter__println__Real;
BardPrinter.prototype.m_println__Real_Integer = BardPrinter__println__Real_Integer;
BardPrinter.prototype.m_println__Integer = BardPrinter__println__Integer;
BardPrinter.prototype.m_println__Character = BardPrinter__println__Character;
BardPrinter.prototype.m_println__Byte = BardPrinter__println__Byte;
BardPrinter.prototype.m_println__Logical = BardPrinter__println__Logical;
BardPrinter.prototype.m_println = BardPrinter__println;

function BardConsole()
{
  this.p_buffer = null;

}

BardConsole.prototype.m_create_instance = BardObject__create_instance;
BardConsole.prototype.m_init = BardObject__init;
BardConsole.prototype.m_hash_code = BardObject__hash_code;
BardConsole.prototype.m_equals__Object = BardObject__equals__Object;
BardConsole.prototype.m_to_Real = BardObject__to_Real;
BardConsole.prototype.m_to_Integer = BardObject__to_Integer;
BardConsole.prototype.m_to_Character = BardObject__to_Character;
BardConsole.prototype.m_to_Byte = BardObject__to_Byte;
BardConsole.prototype.m_to_Logical = BardObject__to_Logical;
BardConsole.prototype.m_to_String = BardObject__to_String;
BardConsole.prototype.m_init_settings = BardConsole__init_settings;
BardConsole.prototype.m_init_defaults = BardConsole__init_defaults;
BardConsole.prototype.m_print__String = BardConsole__print__String;
BardConsole.prototype.m_print__Real = BardConsole__print__Real;
BardConsole.prototype.m_print__Real_Integer = BardConsole__print__Real_Integer;
BardConsole.prototype.m_print__Integer = BardConsole__print__Integer;
BardConsole.prototype.m_print__Character = BardConsole__print__Character;
BardConsole.prototype.m_print__Object = BardConsole__print__Object;
BardConsole.prototype.m_print__Byte = BardConsole__print__Byte;
BardConsole.prototype.m_print__Logical = BardConsole__print__Logical;
BardConsole.prototype.m_println__Object = BardConsole__println__Object;
BardConsole.prototype.m_println__String = BardConsole__println__String;
BardConsole.prototype.m_println__Real = BardConsole__println__Real;
BardConsole.prototype.m_println__Real_Integer = BardConsole__println__Real_Integer;
BardConsole.prototype.m_println__Integer = BardConsole__println__Integer;
BardConsole.prototype.m_println__Character = BardConsole__println__Character;
BardConsole.prototype.m_println__Byte = BardConsole__println__Byte;
BardConsole.prototype.m_println__Logical = BardConsole__println__Logical;
BardConsole.prototype.m_println = BardConsole__println;
BardConsole.prototype.m_flush = BardConsole__flush;
BardConsole.prototype.m_indent = BardConsole__indent;
BardConsole.prototype.m_indent__Integer = BardConsole__indent__Integer;
BardConsole.prototype.m_write__Character = BardConsole__write__Character;
BardConsole.prototype.m_write__String = BardConsole__write__String;
BardConsole.prototype.m_write__StringBuilder = BardConsole__write__StringBuilder;

function BardStopwatch()
{
  this.p_start_time = 0;

}

BardStopwatch.prototype.m_create_instance = BardObject__create_instance;
BardStopwatch.prototype.m_init = BardObject__init;
BardStopwatch.prototype.m_hash_code = BardObject__hash_code;
BardStopwatch.prototype.m_equals__Object = BardObject__equals__Object;
BardStopwatch.prototype.m_to_Real = BardObject__to_Real;
BardStopwatch.prototype.m_to_Integer = BardObject__to_Integer;
BardStopwatch.prototype.m_to_Character = BardObject__to_Character;
BardStopwatch.prototype.m_to_Byte = BardObject__to_Byte;
BardStopwatch.prototype.m_to_Logical = BardObject__to_Logical;
BardStopwatch.prototype.m_to_String = BardStopwatch__to_String;
BardStopwatch.prototype.m_init_settings = BardStopwatch__init_settings;
BardStopwatch.prototype.m_init_defaults = BardStopwatch__init_defaults;
BardStopwatch.prototype.m_elapsed = BardStopwatch__elapsed;
BardStopwatch.prototype.m_restart = BardStopwatch__restart;

function BardTableEntry_of_String_String()
{
  this.p_key = null;
  this.p_value = null;
  this.p_hash = 0;

}

BardTableEntry_of_String_String.prototype.m_create_instance = BardObject__create_instance;
BardTableEntry_of_String_String.prototype.m_init = BardObject__init;
BardTableEntry_of_String_String.prototype.m_hash_code = BardObject__hash_code;
BardTableEntry_of_String_String.prototype.m_equals__Object = BardObject__equals__Object;
BardTableEntry_of_String_String.prototype.m_to_Real = BardObject__to_Real;
BardTableEntry_of_String_String.prototype.m_to_Integer = BardObject__to_Integer;
BardTableEntry_of_String_String.prototype.m_to_Character = BardObject__to_Character;
BardTableEntry_of_String_String.prototype.m_to_Byte = BardObject__to_Byte;
BardTableEntry_of_String_String.prototype.m_to_Logical = BardObject__to_Logical;
BardTableEntry_of_String_String.prototype.m_to_String = BardObject__to_String;
BardTableEntry_of_String_String.prototype.m_init_settings = BardTableEntry_of_String_String__init_settings;
BardTableEntry_of_String_String.prototype.m_init_defaults = BardTableEntry_of_String_String__init_defaults;
BardTableEntry_of_String_String.prototype.m_init__String_String_Integer = BardTableEntry_of_String_String__init__String_String_Integer;

function BardListIndexReader()
{
  this.p_count = 0;

}

BardListIndexReader.prototype.m_create_instance = BardObject__create_instance;
BardListIndexReader.prototype.m_init = BardObject__init;
BardListIndexReader.prototype.m_hash_code = BardObject__hash_code;
BardListIndexReader.prototype.m_equals__Object = BardObject__equals__Object;
BardListIndexReader.prototype.m_to_Real = BardObject__to_Real;
BardListIndexReader.prototype.m_to_Integer = BardObject__to_Integer;
BardListIndexReader.prototype.m_to_Character = BardObject__to_Character;
BardListIndexReader.prototype.m_to_Byte = BardObject__to_Byte;
BardListIndexReader.prototype.m_to_Logical = BardObject__to_Logical;
BardListIndexReader.prototype.m_to_String = BardObject__to_String;
BardListIndexReader.prototype.m_init_settings = BardListIndexReader__init_settings;
BardListIndexReader.prototype.m_init_defaults = BardListIndexReader__init_defaults;
BardListIndexReader.prototype.m_init__Integer = BardListIndexReader__init__Integer;
BardListIndexReader.prototype.m_get__Integer = BardListIndexReader__get__Integer;

function BardArray_of_Byte()
{
}

BardArray_of_Byte.prototype.m_clear_references__Integer_Integer = BardGenericArray__clear_references__Integer_Integer;
BardArray_of_Byte.prototype.m_shift__Integer_Integer_Integer = BardGenericArray__shift__Integer_Integer_Integer;
BardArray_of_Byte.prototype.m_init_settings = BardArray_of_Byte__init_settings;
BardArray_of_Byte.prototype.m_init_defaults = BardArray_of_Byte__init_defaults;
BardArray_of_Byte.prototype.m_create__Integer = BardArray_of_Byte__create__Integer;
BardArray_of_Byte.prototype.m_count = BardArray_of_Byte__count;
BardArray_of_Byte.prototype.m_get__Integer = BardArray_of_Byte__get__Integer;
BardArray_of_Byte.prototype.m_set__Integer_Byte = BardArray_of_Byte__set__Integer_Byte;
BardArray_of_Byte.prototype.m_to_String = BardArray_of_Byte__to_String;

function BardReverseOrderListReader_of_Byte()
{
  this.p_list = null;
  this.p_count = 0;
  this.p_highest_index = 0;

}

BardReverseOrderListReader_of_Byte.prototype.m_create_instance = BardObject__create_instance;
BardReverseOrderListReader_of_Byte.prototype.m_init = BardObject__init;
BardReverseOrderListReader_of_Byte.prototype.m_hash_code = BardObject__hash_code;
BardReverseOrderListReader_of_Byte.prototype.m_equals__Object = BardObject__equals__Object;
BardReverseOrderListReader_of_Byte.prototype.m_to_Real = BardObject__to_Real;
BardReverseOrderListReader_of_Byte.prototype.m_to_Integer = BardObject__to_Integer;
BardReverseOrderListReader_of_Byte.prototype.m_to_Character = BardObject__to_Character;
BardReverseOrderListReader_of_Byte.prototype.m_to_Byte = BardObject__to_Byte;
BardReverseOrderListReader_of_Byte.prototype.m_to_Logical = BardObject__to_Logical;
BardReverseOrderListReader_of_Byte.prototype.m_to_String = BardObject__to_String;
BardReverseOrderListReader_of_Byte.prototype.m_init_settings = BardReverseOrderListReader_of_Byte__init_settings;
BardReverseOrderListReader_of_Byte.prototype.m_init_defaults = BardReverseOrderListReader_of_Byte__init_defaults;
BardReverseOrderListReader_of_Byte.prototype.m_init__ByteList = BardReverseOrderListReader_of_Byte__init__ByteList;
BardReverseOrderListReader_of_Byte.prototype.m_get__Integer = BardReverseOrderListReader_of_Byte__get__Integer;

function BardArray_of_Character()
{
}

BardArray_of_Character.prototype.m_clear_references__Integer_Integer = BardGenericArray__clear_references__Integer_Integer;
BardArray_of_Character.prototype.m_shift__Integer_Integer_Integer = BardGenericArray__shift__Integer_Integer_Integer;
BardArray_of_Character.prototype.m_init_settings = BardArray_of_Character__init_settings;
BardArray_of_Character.prototype.m_init_defaults = BardArray_of_Character__init_defaults;
BardArray_of_Character.prototype.m_create__Integer = BardArray_of_Character__create__Integer;
BardArray_of_Character.prototype.m_count = BardArray_of_Character__count;
BardArray_of_Character.prototype.m_get__Integer = BardArray_of_Character__get__Integer;
BardArray_of_Character.prototype.m_set__Integer_Character = BardArray_of_Character__set__Integer_Character;
BardArray_of_Character.prototype.m_to_String = BardArray_of_Character__to_String;

function BardReverseOrderListReader_of_Character()
{
  this.p_list = null;
  this.p_count = 0;
  this.p_highest_index = 0;

}

BardReverseOrderListReader_of_Character.prototype.m_create_instance = BardObject__create_instance;
BardReverseOrderListReader_of_Character.prototype.m_init = BardObject__init;
BardReverseOrderListReader_of_Character.prototype.m_hash_code = BardObject__hash_code;
BardReverseOrderListReader_of_Character.prototype.m_equals__Object = BardObject__equals__Object;
BardReverseOrderListReader_of_Character.prototype.m_to_Real = BardObject__to_Real;
BardReverseOrderListReader_of_Character.prototype.m_to_Integer = BardObject__to_Integer;
BardReverseOrderListReader_of_Character.prototype.m_to_Character = BardObject__to_Character;
BardReverseOrderListReader_of_Character.prototype.m_to_Byte = BardObject__to_Byte;
BardReverseOrderListReader_of_Character.prototype.m_to_Logical = BardObject__to_Logical;
BardReverseOrderListReader_of_Character.prototype.m_to_String = BardObject__to_String;
BardReverseOrderListReader_of_Character.prototype.m_init_settings = BardReverseOrderListReader_of_Character__init_settings;
BardReverseOrderListReader_of_Character.prototype.m_init_defaults = BardReverseOrderListReader_of_Character__init_defaults;
BardReverseOrderListReader_of_Character.prototype.m_init__CharacterList = BardReverseOrderListReader_of_Character__init__CharacterList;
BardReverseOrderListReader_of_Character.prototype.m_get__Integer = BardReverseOrderListReader_of_Character__get__Integer;

function BardArray_of_String()
{
}

BardArray_of_String.prototype.m_clear_references__Integer_Integer = BardGenericArray__clear_references__Integer_Integer;
BardArray_of_String.prototype.m_shift__Integer_Integer_Integer = BardGenericArray__shift__Integer_Integer_Integer;
BardArray_of_String.prototype.m_init_settings = BardArray_of_String__init_settings;
BardArray_of_String.prototype.m_init_defaults = BardArray_of_String__init_defaults;
BardArray_of_String.prototype.m_create__Integer = BardArray_of_String__create__Integer;
BardArray_of_String.prototype.m_count = BardArray_of_String__count;
BardArray_of_String.prototype.m_get__Integer = BardArray_of_String__get__Integer;
BardArray_of_String.prototype.m_set__Integer_String = BardArray_of_String__set__Integer_String;
BardArray_of_String.prototype.m_to_String = BardArray_of_String__to_String;

function BardReverseOrderListReader_of_String()
{
  this.p_list = null;
  this.p_count = 0;
  this.p_highest_index = 0;

}

BardReverseOrderListReader_of_String.prototype.m_create_instance = BardObject__create_instance;
BardReverseOrderListReader_of_String.prototype.m_init = BardObject__init;
BardReverseOrderListReader_of_String.prototype.m_hash_code = BardObject__hash_code;
BardReverseOrderListReader_of_String.prototype.m_equals__Object = BardObject__equals__Object;
BardReverseOrderListReader_of_String.prototype.m_to_Real = BardObject__to_Real;
BardReverseOrderListReader_of_String.prototype.m_to_Integer = BardObject__to_Integer;
BardReverseOrderListReader_of_String.prototype.m_to_Character = BardObject__to_Character;
BardReverseOrderListReader_of_String.prototype.m_to_Byte = BardObject__to_Byte;
BardReverseOrderListReader_of_String.prototype.m_to_Logical = BardObject__to_Logical;
BardReverseOrderListReader_of_String.prototype.m_to_String = BardObject__to_String;
BardReverseOrderListReader_of_String.prototype.m_init_settings = BardReverseOrderListReader_of_String__init_settings;
BardReverseOrderListReader_of_String.prototype.m_init_defaults = BardReverseOrderListReader_of_String__init_defaults;
BardReverseOrderListReader_of_String.prototype.m_init__StringList = BardReverseOrderListReader_of_String__init__StringList;
BardReverseOrderListReader_of_String.prototype.m_get__Integer = BardReverseOrderListReader_of_String__get__Integer;

function BardTableBin_of_String_String()
{
  this.p_entries = null;

}

BardTableBin_of_String_String.prototype.m_create_instance = BardObject__create_instance;
BardTableBin_of_String_String.prototype.m_init = BardTableBin_of_String_String__init;
BardTableBin_of_String_String.prototype.m_hash_code = BardObject__hash_code;
BardTableBin_of_String_String.prototype.m_equals__Object = BardObject__equals__Object;
BardTableBin_of_String_String.prototype.m_to_Real = BardObject__to_Real;
BardTableBin_of_String_String.prototype.m_to_Integer = BardObject__to_Integer;
BardTableBin_of_String_String.prototype.m_to_Character = BardObject__to_Character;
BardTableBin_of_String_String.prototype.m_to_Byte = BardObject__to_Byte;
BardTableBin_of_String_String.prototype.m_to_Logical = BardObject__to_Logical;
BardTableBin_of_String_String.prototype.m_to_String = BardObject__to_String;
BardTableBin_of_String_String.prototype.m_init_settings = BardTableBin_of_String_String__init_settings;
BardTableBin_of_String_String.prototype.m_init_defaults = BardTableBin_of_String_String__init_defaults;
BardTableBin_of_String_String.prototype.m_find__String_Integer = BardTableBin_of_String_String__find__String_Integer;

function BardObjectList()
{
  this.p_data = null;
  this.p_count = 0;

}

BardObjectList.prototype.m_create_instance = BardObject__create_instance;
BardObjectList.prototype.m_init = BardObjectList__init;
BardObjectList.prototype.m_hash_code = BardObject__hash_code;
BardObjectList.prototype.m_equals__Object = BardObject__equals__Object;
BardObjectList.prototype.m_to_Real = BardObject__to_Real;
BardObjectList.prototype.m_to_Integer = BardObject__to_Integer;
BardObjectList.prototype.m_to_Character = BardObject__to_Character;
BardObjectList.prototype.m_to_Byte = BardObject__to_Byte;
BardObjectList.prototype.m_to_Logical = BardObject__to_Logical;
BardObjectList.prototype.m_to_String = BardObjectList__to_String;
BardObjectList.prototype.m_init_settings = BardObjectList__init_settings;
BardObjectList.prototype.m_init_defaults = BardObjectList__init_defaults;
BardObjectList.prototype.m_init__Integer = BardObjectList__init__Integer;
BardObjectList.prototype.m_init__Integer_Object = BardObjectList__init__Integer_Object;
BardObjectList.prototype.m_init__Array_of_Object = BardObjectList__init__Array_of_Object;
BardObjectList.prototype.m_add__Object = BardObjectList__add__Object;
BardObjectList.prototype.m_add__ObjectList = BardObjectList__add__ObjectList;
BardObjectList.prototype.m_capacity = BardObjectList__capacity;
BardObjectList.prototype.m_clear = BardObjectList__clear;
BardObjectList.prototype.m_clone = BardObjectList__clone;
BardObjectList.prototype.m_contains__Object = BardObjectList__contains__Object;
BardObjectList.prototype.m_discard__Integer_Integer = BardObjectList__discard__Integer_Integer;
BardObjectList.prototype.m_discard_from__Integer = BardObjectList__discard_from__Integer;
BardObjectList.prototype.m_ensure_capacity__Integer = BardObjectList__ensure_capacity__Integer;
BardObjectList.prototype.m_find__Object = BardObjectList__find__Object;
BardObjectList.prototype.m_find__Object_Integer = BardObjectList__find__Object_Integer;
BardObjectList.prototype.m_first = BardObjectList__first;
BardObjectList.prototype.m_get__Integer = BardObjectList__get__Integer;
BardObjectList.prototype.m_indices = BardObjectList__indices;
BardObjectList.prototype.m_insert__Object = BardObjectList__insert__Object;
BardObjectList.prototype.m_insert__ObjectList = BardObjectList__insert__ObjectList;
BardObjectList.prototype.m_insert__Object_Integer = BardObjectList__insert__Object_Integer;
BardObjectList.prototype.m_insert__ObjectList_Integer = BardObjectList__insert__ObjectList_Integer;
BardObjectList.prototype.m_last = BardObjectList__last;
BardObjectList.prototype.m_random = BardObjectList__random;
BardObjectList.prototype.m_remove__Object = BardObjectList__remove__Object;
BardObjectList.prototype.m_remove_at__Integer = BardObjectList__remove_at__Integer;
BardObjectList.prototype.m_remove_first = BardObjectList__remove_first;
BardObjectList.prototype.m_remove_last = BardObjectList__remove_last;
BardObjectList.prototype.m_remove_random = BardObjectList__remove_random;
BardObjectList.prototype.m_reverse = BardObjectList__reverse;
BardObjectList.prototype.m_reverse__Integer_Integer = BardObjectList__reverse__Integer_Integer;
BardObjectList.prototype.m_reverse_order = BardObjectList__reverse_order;
BardObjectList.prototype.m_set__Integer_Object = BardObjectList__set__Integer_Object;
BardObjectList.prototype.m_shuffle = BardObjectList__shuffle;
BardObjectList.prototype.m_swap__Integer_Integer = BardObjectList__swap__Integer_Integer;

function BardReverseOrderListReader_of_Object()
{
  this.p_list = null;
  this.p_count = 0;
  this.p_highest_index = 0;

}

BardReverseOrderListReader_of_Object.prototype.m_create_instance = BardObject__create_instance;
BardReverseOrderListReader_of_Object.prototype.m_init = BardObject__init;
BardReverseOrderListReader_of_Object.prototype.m_hash_code = BardObject__hash_code;
BardReverseOrderListReader_of_Object.prototype.m_equals__Object = BardObject__equals__Object;
BardReverseOrderListReader_of_Object.prototype.m_to_Real = BardObject__to_Real;
BardReverseOrderListReader_of_Object.prototype.m_to_Integer = BardObject__to_Integer;
BardReverseOrderListReader_of_Object.prototype.m_to_Character = BardObject__to_Character;
BardReverseOrderListReader_of_Object.prototype.m_to_Byte = BardObject__to_Byte;
BardReverseOrderListReader_of_Object.prototype.m_to_Logical = BardObject__to_Logical;
BardReverseOrderListReader_of_Object.prototype.m_to_String = BardObject__to_String;
BardReverseOrderListReader_of_Object.prototype.m_init_settings = BardReverseOrderListReader_of_Object__init_settings;
BardReverseOrderListReader_of_Object.prototype.m_init_defaults = BardReverseOrderListReader_of_Object__init_defaults;
BardReverseOrderListReader_of_Object.prototype.m_init__ObjectList = BardReverseOrderListReader_of_Object__init__ObjectList;
BardReverseOrderListReader_of_Object.prototype.m_get__Integer = BardReverseOrderListReader_of_Object__get__Integer;

function BardRandom()
{
  this.p_state = 0;

}

BardRandom.prototype.m_create_instance = BardObject__create_instance;
BardRandom.prototype.m_init = BardRandom__init;
BardRandom.prototype.m_hash_code = BardObject__hash_code;
BardRandom.prototype.m_equals__Object = BardObject__equals__Object;
BardRandom.prototype.m_to_Real = BardRandom__to_Real;
BardRandom.prototype.m_to_Integer = BardRandom__to_Integer;
BardRandom.prototype.m_to_Character = BardObject__to_Character;
BardRandom.prototype.m_to_Byte = BardObject__to_Byte;
BardRandom.prototype.m_to_Logical = BardRandom__to_Logical;
BardRandom.prototype.m_to_String = BardObject__to_String;
BardRandom.prototype.m_init_settings = BardRandom__init_settings;
BardRandom.prototype.m_init_defaults = BardRandom__init_defaults;
BardRandom.prototype.m_init__Real = BardRandom__init__Real;
BardRandom.prototype.m_next_state__Real = BardRandom__next_state__Real;
BardRandom.prototype.m_reset__Real = BardRandom__reset__Real;
BardRandom.prototype.m_to_Real__Real_Real = BardRandom__to_Real__Real_Real;
BardRandom.prototype.m_to_Integer__Integer = BardRandom__to_Integer__Integer;
BardRandom.prototype.m_to_Integer__Integer_Integer = BardRandom__to_Integer__Integer_Integer;

function BardTime()
{
  this.p_previous_time = 0;
  this.p_elapsed = 0;

}

BardTime.prototype.m_create_instance = BardObject__create_instance;
BardTime.prototype.m_init = BardObject__init;
BardTime.prototype.m_hash_code = BardObject__hash_code;
BardTime.prototype.m_equals__Object = BardObject__equals__Object;
BardTime.prototype.m_to_Real = BardObject__to_Real;
BardTime.prototype.m_to_Integer = BardObject__to_Integer;
BardTime.prototype.m_to_Character = BardObject__to_Character;
BardTime.prototype.m_to_Byte = BardObject__to_Byte;
BardTime.prototype.m_to_Logical = BardObject__to_Logical;
BardTime.prototype.m_to_String = BardObject__to_String;
BardTime.prototype.m_init_settings = BardTime__init_settings;
BardTime.prototype.m_init_defaults = BardTime__init_defaults;
BardTime.prototype.m_current = BardTime__current;
BardTime.prototype.m_update = BardTime__update;

function BardMath()
{
}

BardMath.prototype.m_create_instance = BardObject__create_instance;
BardMath.prototype.m_init = BardObject__init;
BardMath.prototype.m_hash_code = BardObject__hash_code;
BardMath.prototype.m_equals__Object = BardObject__equals__Object;
BardMath.prototype.m_to_Real = BardObject__to_Real;
BardMath.prototype.m_to_Integer = BardObject__to_Integer;
BardMath.prototype.m_to_Character = BardObject__to_Character;
BardMath.prototype.m_to_Byte = BardObject__to_Byte;
BardMath.prototype.m_to_Logical = BardObject__to_Logical;
BardMath.prototype.m_to_String = BardObject__to_String;
BardMath.prototype.m_init_settings = BardMath__init_settings;
BardMath.prototype.m_init_defaults = BardMath__init_defaults;
BardMath.prototype.m_floor__Real = BardMath__floor__Real;
BardMath.prototype.m_sqrt__Real = BardMath__sqrt__Real;

//-----------------------------------------------------------------------------
// Singletons
//-----------------------------------------------------------------------------
var BardSingletons = {};

// Create singleton objects
BardSingletons.singleton_Object =  new BardObject();
BardSingletons.singleton_Real = 0;
BardSingletons.singleton_Integer = 0;
BardSingletons.singleton_Character = 0;
BardSingletons.singleton_Byte = 0;
BardSingletons.singleton_Logical = false;
BardSingletons.singleton_String =  new BardString();
BardSingletons.singleton_StringBuilder =  new BardStringBuilder();
BardSingletons.singleton_GenericArray =  new BardGenericArray();
BardSingletons.singleton_Array_of_Object =  new BardArray_of_Object();
BardSingletons.singleton_Exception =  new BardException();
BardSingletons.singleton_Global =  new BardGlobal();
BardSingletons.singleton_Main =  new BardMain();
BardSingletons.singleton_Table_of_String_String =  new BardTable_of_String_String();
BardSingletons.singleton_ByteList =  new BardByteList();
BardSingletons.singleton_CharacterList =  new BardCharacterList();
BardSingletons.singleton_StringList =  new BardStringList();
BardSingletons.singleton_Printer =  new BardPrinter();
BardSingletons.singleton_Console =  new BardConsole();
BardSingletons.singleton_Vector2 = {'p_x':0,'p_y':0};
BardSingletons.singleton_Stopwatch =  new BardStopwatch();
BardSingletons.singleton_TableEntry_of_String_String =  new BardTableEntry_of_String_String();
BardSingletons.singleton_ListIndexReader =  new BardListIndexReader();
BardSingletons.singleton_Array_of_Byte =  new BardArray_of_Byte();
BardSingletons.singleton_ReverseOrderListReader_of_Byte =  new BardReverseOrderListReader_of_Byte();
BardSingletons.singleton_Array_of_Character =  new BardArray_of_Character();
BardSingletons.singleton_ReverseOrderListReader_of_Character =  new BardReverseOrderListReader_of_Character();
BardSingletons.singleton_Array_of_String =  new BardArray_of_String();
BardSingletons.singleton_ReverseOrderListReader_of_String =  new BardReverseOrderListReader_of_String();
BardSingletons.singleton_TableBin_of_String_String =  new BardTableBin_of_String_String();
BardSingletons.singleton_ObjectList =  new BardObjectList();
BardSingletons.singleton_ReverseOrderListReader_of_Object =  new BardReverseOrderListReader_of_Object();
BardSingletons.singleton_Random =  new BardRandom();
BardSingletons.singleton_Time =  new BardTime();
BardSingletons.singleton_Math =  new BardMath();

// Init settings
BardObject__init_settings();
BardReal__init_settings();
BardInteger__init_settings();
BardCharacter__init_settings();
BardByte__init_settings();
BardLogical__init_settings();
BardString__init_settings();
BardStringBuilder__init_settings();
BardGenericArray__init_settings();
BardArray_of_Object__init_settings();
BardException__init_settings();
BardGlobal__init_settings();
BardMain__init_settings();
BardTable_of_String_String__init_settings();
BardByteList__init_settings();
BardCharacterList__init_settings();
BardStringList__init_settings();
BardPrinter__init_settings();
BardConsole__init_settings();
BardVector2__init_settings();
BardStopwatch__init_settings();
BardTableEntry_of_String_String__init_settings();
BardListIndexReader__init_settings();
BardArray_of_Byte__init_settings();
BardReverseOrderListReader_of_Byte__init_settings();
BardArray_of_Character__init_settings();
BardReverseOrderListReader_of_Character__init_settings();
BardArray_of_String__init_settings();
BardReverseOrderListReader_of_String__init_settings();
BardTableBin_of_String_String__init_settings();
BardObjectList__init_settings();
BardReverseOrderListReader_of_Object__init_settings();
BardRandom__init_settings();
BardTime__init_settings();
BardMath__init_settings();

// Init defaults on full singletons
BardSingletons.singleton_Global.m_init_defaults();
BardSingletons.singleton_Console.m_init_defaults();
BardSingletons.singleton_Random.m_init_defaults();
BardSingletons.singleton_Time.m_init_defaults();
BardSingletons.singleton_Math.m_init_defaults();

// Init main class
BardSingletons.singleton_Main.m_init_defaults().m_init();

