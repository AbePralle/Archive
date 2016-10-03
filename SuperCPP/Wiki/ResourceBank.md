## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPPResourceBank.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>
    SuperCPPResourceBank.h<br>
    SuperCPPList.h
  </td></tr>
</table>

## Methods
<table>
  <tr><td>
    <dl><dt>
      ResourceBank&lt;DataType&gt;()
    </dt><dd>
      Constructor.
  </td></tr>
  <tr><td>
    <dl><dt>
      add( resource:DataType )→int
    </dt><dd>
      Adds the given resource to the bank and returns a positive integer id that maps to it.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      clear()→this
    </dt><dd>
      Removes all of the resources in the bank.  Does not free the individual resources; call remove_another() while count() > 0 in order to manually retrieve and free each resource.  All previously used ids are recycled and may be returned again from future calls to add().
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      count()→int
    </dt><dd>
      Returns the number of resource mappings that the bank defines.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      locate_resource( resource:DataType )→int
    </dt><dd>
      Returns the positive integer id of the given resource or -1 if not found. 
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      get( id:int )→DataType
    </dt><dd>
      Returns the resource that the given id maps to or (DataType)0 if not found.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remove( id:int )→DataType
    </dt><dd>
      Removes the given id/resource mapping from the bank and returns the resource pointer.  The given id is recycled and may be returned from a future call to add().
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      remove_another()→DataType
    </dt><dd>
      Removes an arbitrary resource from the bank and returns it or else returns (DataType)0.  Use in conjunction with count() to delete resources one by one if necessary when finished with the bank as an alternative to calling clear() on it.
    </dd></dl>
  </td></tr>
  <tr><td>
    <dl><dt>
      set( id:int, resource:DataType )
    </dt><dd>
      Reassigns the given resource id.  It is assumed that the id is valid.
    </dd></dl>
  </td></tr>
</table>
