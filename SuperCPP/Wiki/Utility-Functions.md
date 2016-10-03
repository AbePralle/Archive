## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPP.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPP.h<br>
    SuperCPP.cpp
  </td></tr>
</table>

## Description
<table>
  <tr><td>
    <dl><dt>
      current_time_seconds()→double
    </dt><dd>
      Returns the current system time in seconds.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      decoded_utf8_count( utf8_data:const char*, utf8_count=-1:int )→int
    </dt><dd>
      Returns the number of 16-bit characters that the given UTF-8 character data would decode to.
      Handles extended UTF8/Unicode above 0xFFFF (up to 21 bits), counting 2 spots for each such character.
      If <code>utf8_count</code> is <code>-1</code> then <code>strlen()</code> is used to calculate its length.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      decoded_utf8_count( utf8_data:Character*, utf8_count:int )→int
    </dt><dd>
      Returns the number of 16-bit characters that the given UTF-8 character data (also stored in 16-bit characters)
      would decode to.  Handles extended UTF8/Unicode above 0xFFFF (up to 21 bits), counting 2 spots for each such character.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      decode_utf8( utf8_data:const char*, utf8_count=-1:int, Character* dest_buffer )
    </dt><dd>
      Decodes 8-bit data in UTF-8 format into a 16-bit destination buffer.  The buffer must be at least
      <code>decoded_utf8_count(data,utf8_count)</code> characters in size.
      Handles extended UTF8/Unicode above 0xFFFF (up to 21 bits), decoding each such character into two spots.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      decode_utf8( utf8_data:Character*, utf8_count:int, Character* dest_buffer )
    </dt><dd>
      Decodes 16-bit data in UTF-8 format into a 16-bit destination buffer.  The buffer must be at least
      <code>decoded_utf8_count(data,utf8_count)</code> characters in size.
      Handles extended UTF8/Unicode above 0xFFFF (up to 21 bits), decoding each such character into two spots.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      double_to_c_string( value:double, buffer:char*, size:int )
    </dt><dd>
      Prints an accurate null-terminated string representation of the given double into the specified character buffer 
      with the given total size.  The string representation is as precise as possible; it can be scanned back
      in to yield the same original number.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      encoded_utf8_count( characters:Character*, count:int )→int
    </dt><dd>
      Returns the number of characters it will take to encode the given 16-bit characters in UTF-8.
      Surrogate pairs representing extended Unicode above 0xFFFF are accounted for.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      encode_utf8( characters:Character*, count:int, dest_buffer:char* )
    </dt><dd>
      UTF-8-encodes the given 16-bit characters into an 8-bit character buffer, which must be
      able to store at least <code>encoded_utf8_count(characters,count)</code> characters.
      Surrogate pairs representing extended Unicode above 0xFFFF are handled correctly.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      encode_utf8( characters:Character*, count:int, dest_buffer:Character* )
    </dt><dd>
      UTF-8-encodes the given 16-bit characters into an 16-bit character buffer, which must be
      able to store at least <code>encoded_utf8_count(characters,count)</code> characters.
      Surrogate pairs representing extended Unicode above 0xFFFF are handled correctly.
    </dd></dl>
  </td></tr>
</table>
