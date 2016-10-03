## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPPDataBuilder.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPP.h<br>
    SuperCPPBuilder.h<br>
    SuperCPPDataBuilder.h<br>
    SuperCPPDataBuilder.cpp
  </td></tr>
</table>

## Overview
Provides methods to encode primitives and strings into a binary data buffer.

## Properties
<table>
  <tr><td>
    <dl><dt>
      capacity : int
    </dt><dd>
      The number of additional bytes that can be added to this databuilder before it automatically increases the size of its data buffer.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      count : int
    </dt><dd>
      The number of bytes currently stored in this databuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      data : Byte*
    </dt><dd>
      An array containing this databuilder's stored bytes.
    </dd></dl>
  </td></tr>
</table>

## Methods
<table>
  <tr><td>
    <dl><dt>
      DataBuilder( initial_capacity=10:int )
    </dt><dd>
      Constructor.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_byte( int:value )→this&
    </dt><dd>
      Writes the given byte value (0-255) to this databuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_int64( value:Int64 )→this&
    </dt><dd>
      Writes the given Int64 value to this databuilder in high-low byte order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_int64x( value:Int64 )→this&
    </dt><dd>
      Writes the given Int64 value to this databuilder as a series of two Int32X values.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_int32( value:Int32 )→this&
    </dt><dd>
      Writes the given Int32 value to this databuilder in high-low byte order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_int32x( value:Int32 )→this&
    </dt><dd>
      Writes the given Int32 value to this databuilder as 1-5 bytes using a compression encoding.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_logical( value:double )→this&
    </dt><dd>
      Writes the given logical value to this databuilder as a single byte.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_real64( value:Real64 )→this&
    </dt><dd>
      Writes the given Real64 value to this databuilder as 8 bytes in high-low order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_real32( value:Real32 )→this&
    </dt><dd>
      Writes the given Real32 value to this databuilder as 4 bytes in high-low order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_string( characters:const char*, character_count=-1:int )→this&
    </dt><dd>
      Writes the IntX character count followed by the IntX encoding of each character to this databuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      write_string( characters:Character*, character_count:int )→this&
    </dt><dd>
      Writes the IntX character count followed by the IntX encoding of each character to this databuilder.
    </dd></dl>
  </td></tr>
  <tr><td>
      See also the inherited methods from [[Builder&lt;Byte&gt;|Builder]].
  </td></tr>

</table>
