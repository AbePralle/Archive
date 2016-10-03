## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPPCString.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPPCString.h<br/>
    SuperCPPCString.cpp
  </td></tr>
</table>

## Properties
<table>
  <tr><td>
    <dl><dt>
      characters : char*
    </dt><dd>
      A buffer containing null-terminated C string data.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      count : int
    </dt><dd>
      The number of characters in this C string.
    </dd></dl>
  </td></tr>
</table>

## Methods
<table>
  <tr><td>
    <dl><dt>
      CString()
    </dt><dd>
      Default constructor.  Does not actually allocate data but will return empty string when cast to <code>char*</code>.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      CString( value:const char* )
    </dt><dd>
      Makes a copy of the given string, accessible via <code>characters</code> or by casting this object to a <code>char*</code> or <code>const char*</code>.  The string data will be automatically freed when this object goes out of scope or is assigned a new value.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      CString( existing:const CString& )
    </dt><dd>
      Makes a copy of the given CString's data.  The copy is accessible via <code>characters</code> or by casting this object to a <code>char*</code> or <code>const char*</code>.  The string data will be automatically freed when this object goes out of scope or is assigned a new value.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      operator=( value:const char* )→this&
    </dt><dd>
      Frees any existing string data and stores a copy of the given string.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      operator=( other:const CString& )→this&
    </dt><dd>
      Frees any existing string data and stores a copy of the given string.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      operator[]( index:int )→char
    </dt><dd>
      Returns the character at the specified index.  No bounds checking is performed.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      operator char*()
    </dt><dd>
      Returns this CString's string data cast to <code>char*</code>.  Returns empty string rather than <code>NULL</code>; <code>NULL</code> is never returned.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      operator const char*()
    </dt><dd>
      Returns this CString's string data cast to <code>const char*</code>.  Returns empty string rather than <code>NULL</code>; <code>NULL</code> is never returned.
    </dd></dl>
  </td></tr>
</table>

