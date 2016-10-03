# SuperCPP Library

<table>
  <tr>
    <td>Purpose</td>
    <td>A lightweight set of general-purpose C++ classes, typedefs, and utility functions.</td>
  </tr>
  <tr>
    <td>Current Version</td>
    <td>1.1.3 - March 5, 2016</td>
  </tr>
  <tr>
    <td>Language</td>
    <td>C++</td>
  </tr>
</table>

## Compiling
Include arbitrary .h and .cpp files from Source/SuperCPP/ into your project.  The utility classes in SuperCPP are largely independent of one another; you can pick and choose which ones to use.  Each class's [Wiki page](https://github.com/AbePralle/SuperCPP/wiki) lists what files are required.

## API
Additional details are available on the Wiki: [https://github.com/AbePralle/SuperCPP/wiki]()

## License
This SuperCPP library is released into the Public Domain under the terms of [The Unlicense](http://unlicense.org/).


## Change Log

### v1.1.3 - March 5, 2016
- Added `CString`.
- Added `DataReader.remaining()` and `reset()`.
- Added `reset()` to all builders that frees any dynamically allocated buffer and resumes using the internal buffer as well as clearing the builder.
- Template datatype fixes.
