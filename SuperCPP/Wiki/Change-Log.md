## v1.1

### v1.1.3 - March 5, 2016
- Added `CString`.
- Added `DataReader.remaining()` and `reset()`.
- Added `reset()` to all builders that frees any dynamically allocated buffer and resumes using the internal buffer as well as clearing the builder.
- Template datatype fixes.

### v1.1.0 - February 12, 2016
*  Added class `Builder` that is like a List but starts out using an internal 1k buffer before expanding using dynamic allocation.  Great for efficient stack-based data building.
*  Added class `StringTable<DataType>` for mapping string-based keys to values of an arbitrary datatype.
*  Added class `IntTable<DataType>` for mapping int-based keys to values of an arbitrary datatype.
*  Added class `StringBuilder` for creating strings piecemeal.
*  Added class `DataBuilder` for encoding primitives and strings in binary form.
*  Added class `DataReader` for decoding primitives and strings from binary data.
*  Added `add(DataType*,int)`, `adopt(DataType*,int)`, and `operator[](int)` to List and Builder.
*  Changed all List methods returning `this` to return references (`List<DataType>&`) instead of pointers (`List<DataType>*`).
*  Fixed error in List where list expands too early while still having one space left.

## v1.0

### v1.0.4 - January 15, 2016
*  Split up List and ResourceBank templates into independent header files.
*  Converted namespace to use `PROJECT_NAMESPACE` trick.

### v1.0.3 - January 14, 2016
*  Converted project from C to C++.

### v1.0.2 - January 1, 2016
*  Added `CResourceBank_locate_first(CResourceBank*)`.
*  Fixed CResourceBank id bounds checking to exclude '0' as a valid id.

### v1.0.1 - December 20, 2015
*  Added additional typedefs: CReal64, CReal32, CInt64, CInt32, CInt16, CInt8 as well as CUInt64, CUInt32, CUInt16, and CUInt8.

### v1.0.0 - December 11, 2015
*  Initial release.
