## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPPList.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPPList.h
  </td></tr>
</table>

## Properties
<table>
  <tr><td>
    <dl><dt>
      capacity : int
    </dt><dd>
      The number of additional items that can be added to this list before it automatically increases the size of its data buffer.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      count : int
    </dt><dd>
      The number of values currently stored in this list.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      data : DataType*
    </dt><dd>
      An array containing this list's stored values.
    </dd></dl>
  </td></tr>
</table>

## Methods
<table>
  <tr><td>
    <dl><dt>
      List&lt;DataType&gt;( initial_capacity=10:int )
    </dt><dd>
      Constructor.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      add( value:DataType )→this&
    </dt><dd>
      Adds the given integer to the end of the list, expanding the capacity of the list if necessary.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      add( array:DataType*, array_count:int )→this&
    </dt><dd>
      Adds all of the values in the given array to this list.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      adopt( new_data:DataType*, new_count:int )→this&
    </dt><dd>
      Adopts the given array as this list's backing data.  Any existing list data is freed.  This list is now responsible for eventually deleting the array.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      clear()→this&
    </dt><dd>
      Removes all values in the given list while retaining the current capacity.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      first()→DataType
    </dt><dd>
      Returns the first value in the list.  The behavior is undefined if the list is empty.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      insert( value:DataType, before_index:int )→this&
    </dt><dd>
      Inserts the given integer in the list before the given index, expanding the capacity of the list if necessary.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      last()→DataType
    </dt><dd>
      Returns the last value in the list.  The behavior is undefined if the list is empty.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      locate( value:DataType )→int
    </dt><dd>
      Returns the index of the given value or -1 if the value doesn't exist.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      operator[]( index:int )→DataType&
    </dt><dd>
      Returns the value at the given index.  No bounds checking is performed.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remove_at( index:int )→DataType
    </dt><dd>
      Removes and returns the value at the given index.  The remaining elements are shifted down to close the gap.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remove_first()→DataType
    </dt><dd>
      Removes and returns the first element from the list.  The behavior is undefined when the list is empty.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remove_last()→DataType
    </dt><dd>
      Removes and returns the last integer from the given list.  The behavior is undefined when the list is empty.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      reserve( n:int )→this&
    </dt><dd>
      Increases the capacity if necessary to make room for n additional values.  Not necessary to call directly but can be a useful optimization. 
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      set( index:int, value:DataType )→this&
    </dt><dd>
      Stores the given value at the given non-negative index, automatically expanding the list capacity and count if necessary.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      to_array( array=0:DataType*, array_count=-1:int )→DataType*
    </dt><dd>
      Copies the the elements of this list into an array, returning the array.  If an array is passed in then it is used as the destination and min(count,array_count) elements are copied into it (or just <code>count</code> items if <code>array_count</code> is <code>-1</code>).  If no array is passed in then an array of exactly <code>count</code> items is dynamically allocated and returned.  In this case it is up to the caller to eventually delete the array.
    </dd></dl>
  </td></tr>
</table>

