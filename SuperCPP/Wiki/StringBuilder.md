## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPPStringBuilder.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPP.h<br>
    SuperCPPBuilder.h<br>
    SuperCPPStringBuilder.h<br>
    SuperCPP.cpp
  </td></tr>
</table>

## Overview
Builds and manages a 16-bit string out via repeated `print()` and `println()` calls.  As a `Builder`, `StringBuilder` starts out using an internal 1k buffer and only switches to dynamic memory allocation when it runs out of room.  Builders are very efficient for stack-based string-building operations and don't require dynamic allocation in most cases.

## Properties
<table>
  <tr><td>
    <dl><dt>
      capacity : int
    </dt><dd>
      The number of additional characters that can be added to this stringbuilder before it automatically increases the size of its data buffer.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      count : int
    </dt><dd>
      The number of characters currently stored in this stringbuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      data : Character*
    </dt><dd>
      An array containing this stringbuilder's stored characters.
    </dd></dl>
  </td></tr>
</table>

## Methods
<table>
  <tr><td>
    <dl><dt>
      StringBuilder( initial_capacity=10:int )
    </dt><dd>
      Constructor.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      as_c_string()→const char*
    </dt><dd>
      Returns a pointer to an 8-bit C string that will be valid until the StringBuilder next modification.  The caller should not free the return value.  The existing characters are truncated to 8 bits each; they are not automatically UTF-8 encoded.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      as_utf8_string()→const char*
    </dt><dd>
      Returns a pointer to an 8-bit C string encoded in UTF-8 format that will be valid until the next StringBuilder modification.  The caller should not free the return value.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      clear()→this&
    </dt><dd>
      Removes all characters in this builder while retaining the current capacity.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      decode_utf8()→this&
    </dt><dd>
      Decodes this stringbuilder's characters from UTF-8 in-place.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      encode_utf8()→this&
    </dt><dd>
      Encodes this stringbuilder's characters in-place into UTF-8 values.
    </dd></dl>
  </td></tr>

  <tr><td>
    <dl><dt>
      print( value:bool )→this&
    </dt><dd>
      Prints a string representation of the given logical value to this stringbuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print( value:char )→this&
    </dt><dd>
      Prints a string representation of the given 8-bit character value to this stringbuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print( value:const char* )→this&
    </dt><dd>
      Prints the given C string to this stringbuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print( value:Character )→this&
    </dt><dd>
      Prints a string representation of the given 16-bit character value to this stringbuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print( value:double )→this&
    </dt><dd>
      Prints a string representation of the given double value to this stringbuilder using a "natural" number of digits to where the representation is as accurate as possible.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print( value:double, decimal_digits:int )→this&
    </dt><dd>
      Prints a string representation of the given double value to this stringbuilder with the specified number of digits after the decimal point.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print( value:int )→this&
    </dt><dd>
      Prints a string representation of the given integer value to this stringbuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print( value:Int64 )→this&
    </dt><dd>
      Prints a string representation of the given 64-bit integer value to this stringbuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      print_character32( value:int )→this&
    </dt><dd>
      Prints a string representation of the given character code to this stringbuilder, allowing extended Unicode values up to 21 bits.  Values over 0xFFFF are printed using a surrogate pair of two characters.
    </dd></dl>
  </td></tr>

  <tr><td>
    <dl><dt>
      println( value:bool )→this&
    </dt><dd>
      Prints a string representation of the given logical value to this stringbuilder followed by a newline.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println( value:char )→this&
    </dt><dd>
      Prints a string representation of the given 8-bit character value to this stringbuilder followed by a newline.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println( value:const char* )→this&
    </dt><dd>
      Prints the given C string to this stringbuilder followed by a newline.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println( value:Character )→this&
    </dt><dd>
      Prints a string representation of the given 16-bit character value to this stringbuilder followed by a newline.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println( value:double )→this&
    </dt><dd>
      Prints a string representation of the given double value to this stringbuilder followed by a newline.  Uses a "natural" number of digits to where the representation is as accurate as possible.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println( value:double, decimal_digits:int )→this&
    </dt><dd>
      Prints a string representation of the given double value to this stringbuilder with the specified number of digits after the decimal point followed by a newline.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println( value:int )→this&
    </dt><dd>
      Prints a string representation of the given integer value to this stringbuilder followed by a newline.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println( value:Int64 )→this&
    </dt><dd>
      Prints a string representation of the given 64-bit integer value to this stringbuilder followed by a newline.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      println_character32( value:int )→this&
    </dt><dd>
      Prints a string representation of the given character code to this stringbuilder followed by a newline, allowing extended Unicode values up to 21 bits.  Values over 0xFFFF are printed using a surrogate pair of two characters.
    </dd></dl>
  </td></tr>
  <tr><td>
      See also the inherited methods from [[Builder&lt;Character&gt;|Builder]].
  </td></tr>

</table>
