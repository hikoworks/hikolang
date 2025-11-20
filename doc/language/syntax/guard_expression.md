# guard-expression

## Syntax

`(` _guard_expression_ `)` __|__\
`not` _guard_expression_ __|__\
_guard_expression_ `and` _guard_expression_ __|__\
_guard_expression_ `or` _guard_expression_ __|__\
_guard_expression_ `in` _guard_expression_ __|__\
_guard_expression_ `not` `in` _guard_expression_ __|__\
_guard_expression_ `==` _guard_expression_ __|__\
_guard_expression_ `!=` _guard_expression_ __|__\
_guard_expression_ `<` _guard_expression_ __|__\
_guard_expression_ `>` _guard_expression_ __|__\
_guard_expression_ `<=` _guard_expression_ __|__\
_guard_expression_ `>=` _guard_expression_ __|__\
[`_fqname_`](fqname.md) __|__\
[`_string_literal_`](string_literal.md) __|__\
[`_version_literal_`](version_literal.md) __|__\
[`_integer_literal_`](integer_literal.md) __|__\
[`_boolean_literal_`](boolean_literal.md)


### Operators

The following operators are available in the build guard expression:

 - `and`: Short-cutting logical AND
 - `or`: Short-cutting logical OR
 - `not`: Logical not
 - `in`: Check if a value is in the array
 - `not in`: Check if a value is not in the array
 - `==`: Compare equal
 - `!=`: Compare not equal
 - `<`: Compare less
 - `>`: Compare greater
 - `<=`: Compare less-or-equal
 - `>=`: Compare greater-or-equal
 - `(` `)`: brackets


### Types

  | type          | false  |
  |:------------- |:------ |
  | bool          | false  |
  | integer       | 0      |
  | string        | ""     |
  | array[string] | []     |


### Constants

For many of the constants its string type is a pseudo-enum or pseudo-flag. If a
constant is compared with a string that does not match any of the possible
values a warning is given. It is only a warning because future compilers
may have the value defined.

Beyond the constants listed below, you may add custom constants to the compiler
with the `--define` command line option, or the `HKDEFINE` environment variable.


#### lang.version : version

The version of the language


#### lang.features : array[string]

Checks for the availability of language features.


#### compiler.name : string

The name of the compiler


#### compiler.version : version

The version of the compiler


#### compiler.features : array[string]

Checks for the availability of compiler features.


#### build.type : string

  | value         | description                                 |
  |:------------- |:------------------------------------------- |
  | "debug"       | Compiling for debugging with extra checks.  |
  | "release"     | Compiling for performance.                  |


#### os.name : string

  | value             | description                                            |
  |:----------------- |:------------------------------------------------------ |
  | "Windows"         | The Microsoft Windows desktop Operating System         |
  | "Linux"           | The Linux desktop Operating System                     |
  | "Android"         | Operating System for Android mobile phones             |
  | "Android TV"      | Operating System for Android TVs and settop-boxes      |
  | "Wear OS"         | Operating System for Android watches                   |
  | "Meta Horizon OS" | Operating System for Android VR/AR headset             |
  | "macOS"           | The Mac OS X, OS X & macOS desktop operating system.   |
  | "iOS"             | Operating System for iPhone.                           |
  | "iPadOS"          | Operating System for iPad.                             |
  | "watchOS"         | Operating System for iWatch.                           |
  | "tvOS"            | Operating System for Apple TV.                         |
  | "audioOS"         | Operating System for HomePod.                          |
  | "visionOS"        | Operating System for Vision Pro.                       |
  | "bridgeOS"        | Apple's embedded Operating System.                     |


#### os.version : version

The version of the operating system, the version number used is depended on
the operating system itself:

 - "linux": The Linux kernel semantic version
 - "macos": The operating system's full semantic version
 - "windows":
   + Windows NT 4.0: 4.0.0
   + Windows 2000: 5.0.0
   + Windows XP: 5.1.0, 5.2.0
   + Windows Vista: 6.0.0
   + Windows 7: 7.0.0
   + Windows 8: 8.0.0, 8.1.0
   + Windows 10:
     * 1507 -> 10.15.7
     * 1511 -> 10.15.11
     * 20H2 -> 10.20.2
     * 21H1 -> 10.21.1
     * 22H2 -> 10.22.2
   + Windows 11:
     * 21H1 -> 11.21.1 (note: Windows 10 went up to 22H2)
     * 25H2 -> 11.25.2

#### os.kernel : string

  | value         | description                                            |
  |:------------- |:------------------------------------------------------ |
  | "Windows"     | Compiling for the Microsoft Windows operating system.  |
  | "Linux"       | Compiling for the Linux operating system.              |
  | "Darwin"      | Compiling for the Darwin operating system.             |

#### os.kernel.version : version

The version number of the kernel

#### os.full_name : string

The full name and version of the operating system.


#### os.platform : array[string]

In certain cases it is possible to compile for multiple platforms at the same
time for example for "phone" and "pad".

  | value         | description                                            |
  |:------------- |:------------------------------------------------------ |
  | "terminal"    | Compile as a terminal application.                     |
  | "service"     | Compile as a background service.                       |
  | "kernel"      | Compile as a kernel space driver.                     |
  | "desktop"     | Compile as a desktop application.                      |
  | "phone"       | Compile for a phone.                                   |
  | "pad"         | Compile for a pad.                                     |
  | "tv"          | Compile for a tv.                                      |
  | "watch"       | Compile for a watch.                                   |
  | "vision"      | Compile for a VR/AR headset.                           |
  | "audio"       | Compile for a speaker/pod device.                      |
  | "car"         | Compile for a car.                                     |

#### os.features : array[string]

Features that are available in the OS. Operating systems lately have been
diverging while sharing a large feature set between them.


#### cpu.arch : string

  | value         | description                                            |
  |:------------- |:------------------------------------------------------ |
  | "x86"         | Compile for Intel / AMD x86 or x86_64 processors.      |
  | "arm"         | Compile for ARM processors.                            |


#### cpu.arch.version : version

 - For "x86" CPUs the following versions are defined:
   + 0.0.0: x86
   + 0.1.0: x86 nocona: 32-bit mode, 64-bit general purpose registers
   + 1.0.0: x86-64-v1
   + 2.0.0: x86-64-v2
   + 3.0.0: x86-64-v3
   + 4.0.0: x86-64-v4


#### cpu.endian : string

  | value         | description                                            |
  |:------------- |:------------------------------------------------------ |
  | "big"         | Compile for big-endian memory layout                   |
  | "little"      | Compile for little-endian memory layout.               |


#### cpu.width : integer

Number of bits of the CPU addressing mode: 32 or 64 bits.


#### cpu.limb.width : integer

Number of bits of a register that is used for limbs of arbitrary sized integers.
In most cases this means the register which has carry-in/out for adds, and hi/lo
register output for multiplication.


#### cpu.features : array[string]

The CPU features that will guaranteed be available on the CPU.

 - x86:
   + "CMOV", "CX8", "FPU", "FXSR", "MMX", "OSFXSR", "SCE", "SSE", "SSE2"
   + "CMPXCHG16B", etc


#### cpu.optionals : array[string]

Optional features that may be available on the CPU. These features must be
checked for at runtime.

See `cpu.features` for the possible values.


#### target.\*, this.\*

Using the `target.*` and `this.*` constants you can specifically check the
`build`, `os`, `cpu` constants for the target machine and this machine
seperately.

This is needed when cross compiling applications, combined with the fact that
during compilation part of the code is executed on the current (this) machine.
