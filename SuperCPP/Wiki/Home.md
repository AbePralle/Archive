# SuperCPP
<table>
  <tr>
    <td>Purpose</td>
    <td>A lightweight set of general-purpose C++ classes, typedefs, and utility functions.</td>
  </tr>
  <tr>
    <td>Language</td>
    <td>C++</td>
  </tr>
</table>

## Compiling

### Pick and Choose Classes
Include arbitrary .h and .cpp files from Source/SuperCPP/ into your project.  The utility classes in SuperCPP are largely independent of one another; you can pick and choose which ones to use.  Each class's [[Wiki page|https://github.com/AbePralle/SuperCPP/wiki]] lists what files are required.

### Project Namespace
The SuperCPP library is defined within the nested namespace `PROJECT_NAMESPACE::SuperCPP`, where `PROJECT_NAMESPACE` is a `#define` that defaults to `Project`.  Including a header file such as "SuperCPP.h" automatically issues the directive `using namespace PROJECT_NAMESPACE;`.  If you're wanting to use SuperCPP in a precompiled library and avoid conflicts with end users who might also use SuperCPP, define `PROJECT_NAMESPACE=SomeOtherNamespace` as a compiler flag.

## API
[[Conditional Compilation Definitions]]

[[Data Types]]

[[Utility Functions]]

[[CString]]

[[StringTable]]

[[IntTable]]

[[List]]

[[Builder]]

[[StringBuilder]]

[[DataBuilder]]

[[DataReader]]

[[ResourceBank]]

## Change Log
[[Change Log]]

## License
This SuperCPP library is released into the Public Domain under the terms of [[The Unlicense|http://unlicense.org/]].
