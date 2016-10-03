## Requirements
<table>
  <tr><td><b>Include</b></td><td>SuperCPP.h</td></tr>
  <tr><td><b>Namespace</b></td><td>SuperCPP</td></tr>
  <tr><td><b>Required Files</b></td>
  <td>SuperCPP.h</td></tr>
</table>

## Description
Including `SuperCPP.h` defines the following preprocessor symbols based on the current platform.  Use e.g. `#if defined(CPP_PLATFORM_MAC)`.

Platform       | Defined Symbols
---------------|----------
Mac            | `CPP_PLATFORM_MAC`
iOS Simulator  | `CPP_PLATFORM_IOS`, `CPP_PLATFORM_IOS_SIMULATOR`
iOS Device     | `CPP_PLATFORM_IOS`, `CPP_PLATFORM_IOS_DEVICE`
Windows 32-bit | `CPP_PLATFORM_WINDOWS`, `CPP_PLATFORM_WINDOWS_32`
Windows 64-bit | `CPP_PLATFORM_WINDOWS`, `CPP_PLATFORM_WINDOWS_64`
Android        | `CPP_PLATFORM_ANDROID`
Unix           | `CPP_PLATFORM_UNIX`
Linux          | `CPP_PLATFORM_UNIX`, `CPP_PLATFORM_LINUX`
Posix          | `CPP_PLATFORM_UNIX`, `CPP_PLATFORM_POSIX`
