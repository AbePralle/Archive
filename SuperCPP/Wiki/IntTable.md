## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPPIntTable.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPPIntTable.h<br>
    SuperCPPIntTable.cpp
  </td></tr>
</table>

## Overview
A templated hash table (AKA Dictionary, Map) that maps `int` keys to values of an arbitrary datatype.

## Properties
<table>
  <tr><td>
    <dl><dt>
      count : int
    </dt><dd>
      The total number of entries or mappings.
    </dd></dl>
  </td></tr>
</table>

## Methods
<table>
  <tr><td>
    <dl><dt>
      IntTable&lt;DataType&gt;( initial_bin_count=16:int )
    </dt><dd>
      Constructor.  The table will automatically double its internal bin count once
      it passes a load factor of 4 (meaning there are an average of 4 entries per hash bin) to
      maintain a load factor between 2 and 4.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      begin()→IntTable&lt;DataType&gt;::Reader
    </dt><dd>
      Returns a reader that will return all the key-value entries in the table one by one.
      This method and the reader object conform to the C++ iterator API; alternately you can obtain a
      <code>reader()</code> and call <code>has_another()</code> and </code>read()</code> to obtain
      each key-value entry.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      end()→IntTable&lt;DataType&gt;::Reader
    </dt><dd>
      Returns a reader that is set to the "end" of the table entries and has no entries available.
      This method and the reader object conform to the C++ iterator API; alternately you can obtain a
      <code>reader()</code> and call <code>has_another()</code> and </code>read()</code> to obtain
      each key-value entry.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      clear()
    </dt><dd>
      Removes all mappings from the table.  If the values you passed into the table were dynamically
      allocated and require deleting then you should call <code>remove_another()</code> on each
      returned value as long as <code>count</code> is greater than zero.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      contains( key:int )→bool
    </dt><dd>
      Returns <code>true</code> if the given key is defined in this table.  <code>find()</code> may
      be preferable to avoid two consectutive lookups.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      find( key:int )→IntTableEntry<DataType>*
    </dt><dd>
      Returns the table entry for the given key or <code>NULL</code> if no mapping exists.
      The entry has properties <code>key</code> and <code>value</code>.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      operator[]( key:int )→DataType&
    </dt><dd>
      Returns a reference to the value that the given key maps to.  Creates a new mapping to
      the default DataType value if no mapping exists, necessary to support the
      syntax <code>table[ key ] = value;</code>.  Keep in mind that you must use <code>contains()</code>
      or <code>find()</code> rather than <code>table[ key ]</code> to check for the existence of a
      key mapping.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      reader()→IntTable&lt;DataType&gt;::Reader
    </dt><dd>
      Returns a reader that will return all the key-value entries in the table one by one.
      Calling <code>has_another()</code> on the reader returns true if there are more entries
      and calling </code>read()</code> on the reader obtains a (possibly NULL) key-value entry
      pointer with properties <code>key:int<code> and <code>value:DataType</code>.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remove( key:int )→bool
    </dt><dd>
      Removes the mapping for the given key if it exists.  Returns <code>true</code> if it did exist
      and was removed or <code>false</code> if the key did not exist.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remove_another()→DataType
    </dt><dd>
      Removes and returns the value matching the given key.  Use this when you need to delete
      each value in a table during clean-up (continue looping while <code>count</code> is greater than zero).
    </dd></dl>
  </td></tr>

</table>
