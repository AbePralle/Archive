# Cargo
**A C++ JSON-compatible data modeling library.**

<table>
<tr>
  <th>Current Version</th>
  <th>Release Date</th>
  <th>Notes</th>
</tr>
<tr>
  <td>v1.0.1</td>
  <td>June 13, 2015</td>
  <td><b>Additional filesystem commands</b>
    <ul>
      <li>Added File::listing()->Value that returns a list of files contained in the folder at this file's filepath.</li>
      <li>Added File::absolute_filepath()->Value.</li>
      <li>Added File::extension()->Value.</li>
      <li>Added File::filepath()->Value.</li>
      <li>Added File::is_folder()->Logical.</li>
      <li>Added File::path()->Value.</li>
      <li>Added File::size()->Long.</li>
      <li>Added static method File::home_folder()->Value.</li>
      <li>Added static method versions of all File methods - for example, File::is_folder(filepath) is equivalent to File(filepath).is_folder().</li>
      <li>Renamed File::load() to load_json().</li>
      <li>A tilde (~) in a filepath now automatically expands to be the path to the user's home folder.
    </ul>

    <p><b>Expanded string support</b>
    <ul>
      <li>Added Value(char), Value(Character), and Value::character_at(index:Integer)->Character.</li>
      <li>Added locate(substring:Value,[i1:Integer])->Value and locate_last(substring:Value,[i1:Integer])->Value.</li>
    </ul>

    <p><b>Samples Folder</b>
    <ul>
      <li>Added `Samples/` folder in project.</li>
    </ul>
  </td>
</tr>
</table>

Platform | Support
---------|----------
Mac OS X | Fully supported.
Linux    | Untested; should be fully supported.
Windows  | Untested although code has Windows support originating from earlier projects.


### Key Features
1.  Uses a smart pointer-based, reference counted system to automatically manage memory.
2.  Can store native data pointers with optional on-release callbacks.
3.  Can load and save both UTF-8 and Unicode-escaped plain ASCII.
4.  JSON I/O uses simple Readers and Writers that can be extended and adapted to a custom data source or sink.
5.  Does not use STL, C++ I/O Streams, or other high-overhead libraries (for mobile etc.).
6.  Likewise Cargo is entirely self-contained with no dependencies outside of the standard C library.
7.  Cargo does not throw exceptions.  JSON parsing errors will result in either a null value or an empty list or table.
8.  For convenience when working with literal JSON strings, Cargo accepts JSON with unquoted table keys and the option to use single quotes to bound strings.
9.  Cargo is free and unencumbered software released into the public domain under the terms of the UNLICENSE.


### Adding Cargo To Your Project
1.  Clone or download the latest Cargo library from: https://github.com/Plasmaworks/Cargo
2.  Add `Cargo.h` and `Cargo.cpp` into your project.
3.  `#include "Cargo.h"` in any file you wish to use Cargo from.
4.  Optionally write `using namespace Cargo;` to be able to write `Value` instead of `Cargo::Value` and so forth.

### Cargo Data Types
Variables of type `Value` (`Cargo::Value`) can store and manipulate values of any of the following 7 Cargo Data Types:

Data Type  | Examples                      | Notes
-----------|-------------------------------|------------------
null       | `null`                        |
Logical    | `true, false`                 | AKA Boolean, bool
Number     | `0, 3, 5.9, -2.3, 1e100`      |
String     | `"I❤NY", ""`                  |
List       | `[3,"four",true,null,7], []`  | AKA Array
Table      | `{"name":"Abe","born":1973}`  | AKA Object, Dictionary, Map, Hashtable
NativeData | `(any C/C++ pointer)`         | Accepts `void*`.

The C++ code to create each of the above data types is shown here:

Data Type  | Examples
-----------|--------------------------------------------------
null       | `Value x;`<br>`Value x = Value();`<br>`Value x = Null;`
Logical    | `Value x = true;`<br>`Value x = Value(true);`<br>`Value x(true);`
Number     | `Value x = 5.9;`<br>`Value x = Value(1e100);`
String     | `Value x = "I\u2764NY";`
List       | `Value x = List(); x.add(3);`<br>`Value x = List().add("four").add(true);`
Table      | `Value x = Table().set("name","Abe").set("born",1973);`
NativeData | `// Cargo will free the allocation below`<br>`// when it's no longer referenced.`<br>`Value x = Value(malloc(128),FreeOnRelease);`<br><br>`// Cargo will not free this allocation.`<br>`Value x((void*)some_ptr);`

### Sample Program
```C++
#include <cstdio>
using namespace std;

#include "Cargo.h"
using namespace Cargo;

int main()
{
  Value table = Table();
  table.set( "name", Table().set("first","Abe") );

  printf( "PRINTLN\n" );
  table.println();
  // prints: {name:{first:"Abe"}}

  printf( "\nTO JSON\n" );
  table.to_json().println();
  // prints: {"name":{"first":"Abe"}}

  table.set( "born", 1973 );
  table.get( "name" ).set( "last", "Pralle" );
  
  Value people = List().add( table );
  people.add( JSON("{name:{first:'Max',last:'Rockatansky'}}") );

  // Save the data as JSON.
  File( "People.json" ).save( people );

  // Load it right back up again - it will load an empty list on an error.
  people = File( "People.json" ).load_list();
  
  printf( "\nPEOPLE\n" );
  people.println();

  return 0;
}
```

### API
This is the practical subset of the Cargo API that omits some detail.  Examine the source code for additional implementation details and method attributes.

#### Primitive Typedefs
Cargo uses the following typedefs in its API (e.g. Cargo::Integer specifies int32_t):

Cargo Type          | C++ Type
--------------------|-----------
Character           | uint16_t
Integer             | int32_t
Logical             | bool
Long                | int64_t
Real                | double
NativeDataReleaseFn | void (\*)(void* data)

#### Value
Method | Description
-------|------------
`Value()` | Creates a `null` value.
`Value(value:char)`  | Creates a string.
`Value(value:char*)` | Creates a string.
`Value(value:Character)` | Creates a string.
`Value(value:Character*,character_count:Integer)` | Creates a string.
`Value(value:Logical)` | Creates a logical `true`/`false`.
`Value(value:Integer)` | Creates a number.
`Value(value:Real)`    | Creates a number.
`Value(value:void*,release_fn=0:NativeDataReleaseFn)` | Stores a native pointer as a value.  If a NativeDataReleaseFn is specified, that function is called with the original native pointer when this native data value becomes unreferenced.  You may pass the predefined function `FreeOnRelease` to have free() automatically called on the native pointer.
`add(value:Value)->this` | Appends the value to this list.
`add_all(list:Value)->this` | Appends all values in the other list to this list.
`at(index:Integer)->Value`  | Returns the list element, table key, or single character substring at this value's given 0-based index (`0..<count()`).
`character_at(index:Integer)->Character`  | Returns the Unicode character at the given index of this string.
`clear()->Value` | Removes all items in this list or table.
`clone()->Value` | Performs a deep clone of this list or table (table keys are uncloned).
`contains(value:Value)->Logical` | Returns `true` if `value` is an element of this list or a key of this table.
`count()->Integer` | Returns the element count of this list or table or the character count of this string.
`first()->Value`   | Returns the first element or character of this list, table, or string.
`from(i1:Integer)->Value` | Returns the subset of list elements or string characters starting at `i1`.
`from(i1:Integer,i2:Integer)->Value` | Returns the subset of list elements or string characters in the range `i1..i2` inclusive.
`get(index:Integer)->Value` | Returns the list element or single character substring at the given index.  Operator[] is equivalent to 'get'.
`get(key:Value)->Value` | Returns the table value with the given key.
`get_integer(key:Value,default_value=0:Integer)->Integer` | Returns the table value with the given key as an integer.  Returns the given default value (or 0) if the key doesn't exist.
`get_list(key:Value)->Value` | Returns the table value with the given key as a list.  Returns an empty list if the key doesn't exist or if the associated value is not a list.
`get_logical(key:Value,default_value=false:Logical)->Logical` | Returns the table value with the given key as a logical.  Returns the given default value (or `false`) if the key doesn't exist.
`get_real(key:Value,default_value=0.0:Real)->Real` | Returns the table value with the given key as a real.  Returns the given default real (or 0.0) if the key doesn't exist.
`get_string(key:Value,default_value="":Value)->Value` | Returns the table value with the given key as a string.  Returns the given default value (or "") if the key doesn't exist.
`get_table(key:Value)->Value` | Returns the table value with the given key as a table.  Returns an empty table if the key doesn't exist or if the associated value is not a table.
`hash_code()->Integer` | Returns a hash code for this value that can be used to bin values.  Primarily used internally to implement tables.
`insert(value:Value)->this` | Inserts the given value into the beginning of this list.
`is_null()->Logical` | Returns `true` if this is a null value.
`is_logical()->Logical` | Returns `true` if this is a logical value.
`is_integer()->Logical` | Returns `true` if this is an integer value (a whole number).
`is_number()->Logical` | Returns `true` if this is a number value.
`is_string()->Logical` | Returns `true` if this is a string value.
`is_list()->Logical` | Returns `true` if this is a list value.
`is_table()->Logical` | Returns `true` if this is a table value.
`is_native_data()->Logical` | Returns `true` if this is a native data value.
`last()->Value` | Returns the last element of this list, table, or string.
`locate(value:Value,[i1:Integer])->Integer` | Returns the first index of the given element in this list or character or substring in this string or -1 if not found.  Starts looking forwards at `i1`.
`locate_last(value:Value,[i1:Integer])->Integer` | Returns the last index of the given element in this list or character or substring in this string or -1 if not found.  Starts looking backwards at `i1`.
`native_data()->void*` | Returns the native `(void*)` pointer stored in this native data value.
`native_data()->void*` | Returns the native `(void*)` pointer stored in this native data value.
`operator +,-,*,/,%` | Values can be combined using basic math operators.
`operator +=,-=,*=,/=,%=` | Values can be combined using shorthand math operators.
`operator ==,!=,<=,>=,<,>` | Values can be compared using comparison operators.
`print()` | Prints a simplified (non-JSON) version of this value to stdout for use in logging or debugging.
`println()` | Prints a simplified (non-JSON) version of this value to stdout for use in logging or debugging.
`remove(value:Value)->Value` | Removes the given list element or table key and returns the stored value.
`remove_at(index:Integer)->Value` | Removes the list element or table key at the given index.
`remove_first()->Value` | Removes the first value in this list or table.
`remove_last()->Value` | Removes the last value in this list or table.
`set(key:Value,value:Value)->Value` | Maps `key` to `value` in this table.
`to_c_string(buffer=0:char*,buffer_size=0:Integer)->char*` | Prints a string representation of this value to the given character buffer with the given total size (a size of 100 would allow 99 characters before the null terminator).  If no buffer is given then a buffer of the correct size is allocated and returned.  It is the caller's responsibility to free() the buffer in the latter case.
`to_integer()->Integer` | Returns an integer representation of this value.
`to_json()->Value` | Returns a JSON string representation of this value.
`to_logical()->Logical` | Returns a logical `true`/`false` representation of this value.
`to_real()->Real` | Returns a real number representation of this value.
`to_string()->Value` | Returns a simplified (non-JSON) string representation of this value.

#### JSON
Function                                     | Description
---------------------------------------------|--------------------------------------------------
`JSON(json:char*)->Value`                    | Converts the given JSON string into a data model.
`JSON(json:Character*,count:Integer)->Value` | Converts the given JSON string into a data model.

#### List, Table
Function                                     | Description
---------------------------------------------|--------------------------------------------------
`List()->Value`                              | Creates an empty list.
`Table()->Value`                             | Creates an empty table.

#### File
Method                       | Description
-----------------------------|--------------------------------------------------
`File(filepath:Value)`       | Creates a file referencing the given filepath.  A leading tilde (~) is expanded to the be the path of the user's home folder.
`File(path:Value,filename:Value)` | Creates a file referencing the given path and filename.
`absolute_filepath()->Value` | Returns the file's potentially relative filepath as an absolute filepath.
`base_filename()->Value`     | Returns the filename without the extension.
`exists()->Logical`          | Returns `true` if the file exists.
`extension()->Value`         | Returns the extension of this file's filepath.
`filename()->Value`          | Return the filename after the last '/' or '\' path separator.
`home_folder()->Value`       | This static method returns the path of the user's home folder, e.g. "/Users/abe".
`is_folder()->Logical`       | Returns `true` if the file is a folder.
`listing()->Value`           | Returns a list of files contained in the folder at this file's filepath.
`load()->Value`              | Loads a data model from this JSON file.  Returns a null value on failure.
`load_list()->Value`         | Loads a list from this JSON file.  Returns an empty list if an error occurs.
`load_table()->Value`        | Loads a table from this JSON file.  Returns an empty table if an error occurs.
`path()->Value`              | Returns the path component that precedes the final `/` or `\` separator or else `""`.
`save(value:Value,force_ascii=false:Logical)->Value` | Saves the given value as a JSON file.  If `force_ascii` is `true` then all Unicode string characters are escaped into plain ASCII - '☺' becomes "\u263a" for instance.
`size()->Long` | Returns the file size in bytes.

Note: static versions of File methods are available.  For example, File::is_folder(filepath) is equivalent to File(filepath).is_folder().</li>

### Change Log
<table>
<tr>
  <th>Version</th>
  <th>Release Date</th>
  <th>Notes</th>
</tr>
<tr>
  <td>v1.0.1</td>
  <td>June 13, 2015</td>
  <td><b>Additional filesystem commands</b>
    <ul>
      <li>Added File::listing()->Value that returns a list of files contained in the folder at this file's filepath.</li>
      <li>Added File::absolute_filepath()->Value.</li>
      <li>Added File::extension()->Value.</li>
      <li>Added File::filepath()->Value.</li>
      <li>Added File::is_folder()->Logical.</li>
      <li>Added File::path()->Value.</li>
      <li>Added File::size()->Long.</li>
      <li>Added static method File::home_folder()->Value.</li>
      <li>Added static method versions of all File methods - for example, File::is_folder(filepath) is equivalent to File(filepath).is_folder().</li>
      <li>Renamed File::load() to load_json().</li>
      <li>A tilde (~) in a filepath now automatically expands to be the path to the user's home folder.
    </ul>

    <p><b>Expanded string support</b>
    <ul>
      <li>Added Value(char), Value(Character), and Value::character_at(index:Integer)->Character.</li>
      <li>Added locate(substring:Value,[i1:Integer])->Value and locate_last(substring:Value,[i1:Integer])->Value.</li>
    </ul>

    <p><b>Samples Folder</b>
    <ul>
      <li>Added `Samples/` folder in project.</li>
    </ul>
  </td>
</tr>
<tr>
  <td>v1.0.0</td>
  <td>June 11, 2015</td>
  <td>Initial release</td>
</tr>
</table>

