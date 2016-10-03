## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPPDataReader.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPP.h<br>
    SuperCPPBuilder.h<br>
    SuperCPPDataReader.h<br>
    SuperCPPStringBuilder.h<br>
    SuperCPPDataReader.cpp
    SuperCPPStringBuilder.cpp<br>
  </td></tr>
</table>

## Overview
Provides methods to decode primitives and strings from a binary data buffer.

## Properties
<table>
  <tr><td>
    <dl><dt>
      position : int
    </dt><dd>
      The data index of the next read.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      count : int
    </dt><dd>
      The total number of bytes pointed to by <code>data</code>.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      data : Byte*
    </dt><dd>
      An pointer to a byte array to read from.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      should_free_data : bool
    </dt><dd>
      <code>true</code> if this datareader should delete the <code>data</code> array when finished with it.
    </dd></dl>
  </td></tr>
</table>

## Methods
<table>
  <tr><td>
    <dl><dt>
      DataReader( data:Byte*, count:int, should_free=false:bool )
    </dt><dd>
      Constructor.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      has_another()→bool
    </dt><dd>
      Returns <code>true</code> if at least one more byte can be read.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      read_byte()→int
    </dt><dd>
      Reads and returns the next byte of input or <code>0</code> if no more bytes are left.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      read_int64()→Int64
    </dt><dd>
      Reads and returns an Int64 by assembling the next 8 bytes of input in high-low order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      read_int64x()→Int64
    </dt><dd>
      Reads an returns an Int64 by decoding and assembling the next two compressed Int32X
    </dd></dl>
      values (2-10 bytes total) in high-low order.
  </td></tr>
  <tr><td>
    <dl><dt>
      read_int32()→Int32
    </dt><dd>
      Reads and returns an Int32 by assembling the next 4 bytes of input in high-low order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      read_int32x()→Int32
    </dt><dd>
      Reads an returns an Int32 by decoding and assembling the next 2-5 bytes that were stored
    </dd></dl>
      in the "Int32X" compression format.
  </td></tr>
  <tr><td>
    <dl><dt>
      read_logical()→bool
    </dt><dd>
      Reads the next byte and returns <code>true</code> if it's non-zero.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      read_real64( value:Real64 )→Real64
    </dt><dd>
      Reads and returns a Real64 (AKA double) by assembling the next 8 bytes of input in high-low order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      read_real32()→Real32
    </dt><dd>
      Reads and returns a Real32 (AKA float) by assembling the next 4 bytes of input in high-low order.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      read_string( buffer:StringBuilder& )→int
    </dt><dd>
      Reads a string from the input, storing it into the given StringBuilder.  The first Int32X is the
      number of Int32X characters following.  The number of characters read is returned.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remaining()→int
    </dt><dd>
      Returns the number of bytes remaining in this data reader.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      reset( data:Byte*, count:int, should_free=false:bool )
    </dt><dd>
      Frees existing data if necessary and resets the reader to read from a new set of data.
    </dd></dl>
  </td></tr>
</table>
