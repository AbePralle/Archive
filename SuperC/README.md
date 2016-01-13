# SuperC Library

<table>
  <tr>
    <td>Purpose</td>
    <td>A lightweight set of straight C (and C++ compatible) "classes", typedefs, and utility functions.  Incorporated into some other projects on this page.</td>
  </tr>
  <tr>
    <td>Current Version</td>
    <td>1.0.2 - January 1, 2016</td>
  </tr>
  <tr>
    <td>Language</td>
    <td>C (C++ Compatible)</td>
  </tr>
</table>

## Compiling

Add Source/SuperC.c and .h to your project.

## API

### Conditional Compilation

Including `SuperC.h` defines the following preprocessor symbols based on the current platform.  The symbols are defined as the value `1`, allowing e.g. both `#if defined(C_PLATFORM_MAC)` and `#if C_PLATFORM_MAC` to work in project-specific conditional compilation directives.

Platform       | Defined Symbols
---------------|----------
Mac            | `C_PLATFORM_MAC`
iOS Simulator  | `C_PLATFORM_IOS`, `C_PLATFORM_IOS_SIMULATOR`
iOS Device     | `C_PLATFORM_IOS`, `C_PLATFORM_IOS_DEVICE`
Windows 32-bit | `C_PLATFORM_WINDOWS`, `C_PLATFORM_WINDOWS_32`
Windows 64-bit | `C_PLATFORM_WINDOWS`, `C_PLATFORM_WINDOWS_64`
Android        | `C_PLATFORM_ANDROID`
Unix           | `C_PLATFORM_UNIX`
Linux          | `C_PLATFORM_UNIX`, `C_PLATFORM_LINUX`
Posix          | `C_PLATFORM_UNIX`, `C_PLATFORM_POSIX`

### Memory Allocation
Function         |Description
-----------------|------------------------
`C_allocate(type)` |Allocates a new instance (via malloc), zeros out, and returns a correctly-typed pointer for the given type.  For instance, `C_allocate(int)` would return an `int*` of `sizeof(int)` bytes all cleared to `0`.
`C_allocate_array(type,count)`|Allocates, clears, and returns a linear block of memory as an array of size `count`.  For instance, `C_allocate_array(int,5)` returns an `int*` of `sizeof(int)*5` bytes all cleared to `0`.
`C_free(pointer)`|Checks to make sure `pointer` is non-null, free()s it if so, and in either case returns `(void*)0` (NULL) to allow one-step check/free/null statements such as `data = (int*) C_free(data);`.

### Utility Functions 
Function | Description
---------|--------------
`C_current_time_seconds()→double` | Returns the current system time in seconds.

### Data Types
SuperC.h defines the following data types:

Data Type  | Description
-----------|------------
CReal      | A 64-bit real number.
CFloat     | A 32-bit real number.
CLong      | A 64-bit integer.
CInteger   | A 32-bit integer.
CCharacter | An unsigned 16-bit integer.
CByte      | An unsigned 8-bit integer.
CLogical   | An 'int'.


### CIntList

CIntList is an `int` arraylist/vector.

#### Data Type

```C
struct CIntList
{
  int* data;
  int  count;
  int  capacity;
};
```

#### Functions
Function | Description
---------|--------------------------
`CIntList_create()→CIntList*`                 |Allocates a new CIntList, calls `CIntList_init()` on it, and returns it.
`CIntList_destroy( list:CIntList* )→CIntList*`|Calls `CIntList_retire()` on the given list and then frees the list.
`CIntList_init( list:CIntList* )→CIntList*`   |Initializes an existing (perhaps stack-based or embedded) CIntList object.
`CIntList_retire( list:CIntList* )`           |Frees the data in the given list but does not free the list object itself.
                                              |
`CIntList_add( list:CIntList* , value:int )`   |Adds the given integer to the end of the list, expanding the capacity of the list if necessary.
`CIntList_clear( list:CIntList* )→CIntList*`  |Removes all values in the given list while retaining the same capacity.
`CIntList_insert( list:CIntList* , value:int, before_index:int )`   |Inserts the given integer in the list before the given index, expanding the capacity of the list if necessary.
`CIntList_locate( list:CIntList*, value:int )→int` |Returns the index of the given value or `-1` if the value doesn't exist.
`CIntList_remove_at( list:CIntList*, index:int )→int` |Removes and returns the value at the given index.  The remaining elements are shifted down to close the gap.
`CIntList_remove_last( list:CIntList* )→int` |Removes and returns the last integer from the given list.  The return value is undefined when the list is empty.
`CIntList_reserve( list:CIntList*, n:int )` |Increases the capacity if necessary to make room for `n` additional values.  Not necessary to call directly but can be a useful optimization. 
`CIntList_set( list:CIntList*, index:int, value:int )` |Stores the given value at the given non-negative index, automatically expanding the list capacity and count if necessary.


### CPointerList

CPointerList is a `void*` arraylist/vector.

#### Data Type

```C
struct CPointerList
{
  int* data;
  int  count;
  int  capacity;
};
```

#### Functions
Function | Description
---------|--------------------------
`CPointerList_create()→CPointerList*`                 |Allocates a new CPointerList, calls `CPointerList_init()` on it, and returns it.
`CPointerList_destroy( list:CPointerList* )→CPointerList*`|Calls `CPointerList_retire()` on the given list and then frees the list.
`CPointerList_init( list:CPointerList* )→CPointerList*`   |Initializes an existing (perhaps stack-based or embedded) CPointerList object.
`CPointerList_retire( list:CPointerList* )`           |Frees the data in the given list but does not free the list object itself.
`CPointerList_add( list:CPointerList* , value:void* )`   |Adds the given pointer to the end of the list, expanding the capacity of the list if necessary.
`CPointerList_clear( list:CPointerList* )→CPointerList*`  |Removes all values in the given list while retaining the same capacity.
`CPointerList_free_elements( list:CPointerList* )→CPointerList*`  |Calls `C_free()` on each element in the list and then clears the list.
`CPointerist_insert( list:CPointerist* , value:void*, before_index:int )`   |Inserts the given integer in the list before the given index, expanding the capacity of the list if necessary.
`CPointerList_locate( list:CPointerList*, value:void* )→int` |Returns the index of the given value or `-1` if the value doesn't exist.
`CPointerList_remove_at( list:CPointerList*, index:int )→void*` |Removes and returns the pointer at the given index.  The remaining elements are shifted down to close the gap.
`CPointerList_remove_last( list:CPointerList* )→void*` |Removes and returns the last pointer from the given list.  The return value is undefined when the list is empty.
`CPointerList_reserve( list:CPointerList*, n:int )` |Increases the capacity if necessary to make room for `n` additional values.  Not necessary to call directly but can be a useful optimization. 
`CPointerList_set( list:CPointerList*, index:int, value:void* )` |Stores the given value at the given non-negative index, automatically expanding the list capacity and count if necessary.


### CResourceBank
CResourceBank provides a way to map arbitrary resource data pointers to integers.

Function | Description
---------|-------------
`CResourceBank_create()→CResourceBank*` | Allocates a new CResourceBank, calls `CResourceBank_init()` on it, and returns it.
`CResourceBank_destroy( bank:CResourceBank* )→CResourceBank*` | Calls `CResourceBank_retire()` on the given bank and then frees the bank.
`CResourceBank_init( bank:CResourceBank* )` | Initializes an existing (perhaps stack-based or embedded) resource bank.
`CResourceBank_retire( bank:CResourceBank* )` | Frees the data in the given bank but does not free the bank object itself.
`CResourceBank_add( bank:CResourceBank*, resource:void* )→int` | Adds the given resource to the bank and returns a non-zero integer id that maps to it.
`CResourceBank_clear( bank:CResourceBank* )` | Removes all of the resources in the bank.  Does not free the individual resources; call `CResourceBank_remove_another()` while `CResourceBank_count()` is greater than zero in order to manually retrieve and free each resource.  All previously used ids are recycled an may be returned again from future calls to `CResourceBank_add()`.
`CResourceBank_count( bank:CResourceBank* )→int` | Returns the number of resource mappings that the bank defines.
`CResourceBank_locate_resource( bank:CResourceBank* , resource:void* )→int` | Returns the integer id of the given resource or `-1` if not found. 
`CResourceBank_get( bank:CResourceBank*, id:int )→void*` | Returns the resource that the given id maps to or `(void*)0` if not found.
`CResourceBank_remove( bank:CResourceBank*, id:int )→void*` | Removes the given id/resource mapping from the bank and returns the resource pointer.  The given id is recycled and may be returned from a future call to `CResourceBank_add()`.
`CResourceBank_remove_another( bank:CResourceBank* )→void*` | Removes an arbitrary resource from the bank and returns it or else returns `(void*)0`.  Use in conjunction with `CResourceBank_count()` to delete resources one by one if necessary when finished with the bank or as an alternative to calling `clear()` on it.
`CResourceBank_set( bank:CResourceBank*, id:int, resource:void* )` | Reassigns the given resource id.  It is assumed that the id is valid.

## Change Log

### v1.0.2 - January 1, 2016
*  Added `CResourceBank_locate_first(CResourceBank*)`.
*  Fixed CResourceBank id bounds checking to exclude '0' as a valid id.

### v1.0.1 - December 20, 2015
*  Added additional typedefs: CReal64, CReal32, CInt64, CInt32, CInt16, CInt8 as well as CUInt64, CUInt32, CUInt16, and CUInt8.

### v1.0.0 - December 11, 2015
*  Initial release.
