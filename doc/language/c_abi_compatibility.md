# C-ABI Compatibility


Integers
--------

Hikolang's `int` type is compatible with C-style integers,
including layout and size in memory.

The following integers are compatible with the C-API.

 | hikolang-type                                     | hikolang alias | C-type
 |:--------------------------------------------------|:---------------|:---------
 | int[-128..=127]                                   | std.i8         | std::int8_t
 | int[0..=255]                                      | std.u8         | std::uint8_t
 | int[-32768..=32767]                               | std.i16        | std::int16_t
 | int[0..=65535]                                    | std.u16        | std::uint16_t
 | int[-2147483648..=2147483647]                     | std.i32        | std::int32_t
 | int[0..=4294967295]                               | std.u32        | std::uint32_t
 | int[-9223372036854775808..=9223372036854775807]   | std.i32        | std::int32_t
 | int[0..=18446744073709551615]                     | std.u32        | std::uint32_t
 | int[0..=255]                                      | std.c_bool     | std::bool
 | int[0..=255]                                      | std.c_byte     | std::byte
 |                                                   | std.c_char     | char
 | int[-128..=127]                                   | std.c_ichar    | signed char
 | int[0..=255]                                      | std.c_uchar    | unsigned char
 |                                                   | std.c_ishort   | signed short
 |                                                   | std.c_ushort   | unsigned short
 |                                                   | std.c_iint     | signed int
 |                                                   | std.c_uint     | unsigned int
 |                                                   | std.c_ilong    | signed long
 |                                                   | std.c_ulong    | unsigned long
 |                                                   | std.c_illong   | signed long long
 |                                                   | std.c_ullong   | unsigned long long
 |                                                   | std.c_isize    | std::ptrdiff_t
 |                                                   | std.c_usize    | std::size_t
 |                                                   | std.c_ptr      | std::intptr_t
 |                                                   | std.c_ptr      | std::uintptr_t
 |                                                   | std.c_ptr      | _type_ *
