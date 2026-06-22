# string

A string is used for storing text in UTF-8 encoding, it has the following
features:
 - Type is 12/24 bytes in size.
 - The string is nul-terminated to be compatible with C-ABI.
 - SSO (Short String Optimization) for strings up to 11/23 (12/24 including nul) characters length.


## type template

The string type template is `string[encoding : char_encoding]`. `char_encoding`
is an open enum, allowing more encodings to be added by the program.

  char_encoding  | Description
 :-------------- |:-----------------------------------------
  bytes          | Just bytes, no encoding
  WTF8           | WTF-8 encoding used for file names.
  UTF8           | Validated correct UTF-8
  UTF8_NFC       | Validated correct UTF-8 in validated NFC form.

String functions may upgrade or downgrade between the encodings described
above. Most other encodings like ISO-8859-1 are temporary run-time encodings
and may not need to be part of the enum.


## Memory layout.

This is the memory layout of a string:
```
       64-bit BE             64-bit LE              32-bit BE             32-bit LE
     7 6 5 4 3 2 1 0       7 6 5 4 3 2 1 0        7 6 5 4 3 2 1 0       7 6 5 4 3 2 1 0  
    +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+
  0 |msb            |   0 |            lsb|   0 |msb            |   0 |            lsb|
  1 |               |   1 |               |   1 |     _ptr      |   1 |     _ptr      |
  2 |               |   2 |               |   2 |               |   2 |               |
  3 |     _ptr      |   3 |     _ptr      |   3 |            lsb|   3 |msb            |
  4 |               |   4 |               |     +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+
  5 |               |   5 |               |   4 |msb            |   4 |            lsb|
  6 |               |   6 |               |   5 |    _size      |   5 |     _size     |
  7 |            lsb|   7 |msb            |   6 |               |   6 |               |
    +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+   7 |            lsb|   7 |msb            |
  8 |msb            |   8 |            lsb|     +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+
  9 |               |   9 |               |   8 |               |   8 |               |
 10 |               |  10 |               |   9 |   _capacity   |   9 |   _capacity   |
  1 |     _size     |   1 |     _size     |  10 |_______________|  10 |_______________|
  2 |               |   2 |               |   1 | _short_size |L|   1 |L| _short_size |
  3 |               |   3 |               |     +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+
  4 |               |   4 |               |
  5 |            lsb|   5 |msb            |
    +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+
  6 |msb            |   6 |            lsb|
  7 |               |   7 |               |
  8 |               |   8 |               |
  9 |   _capacity   |   9 |   _capacity   |
 20 |               |  20 |               |
  1 |               |   1 |               |
  2 |_______________|   2 |_______________|
  3 | _short_size |L|   3 |L| _short_size |
    +-+-+-+-+-+-+-+-+     +-+-+-+-+-+-+-+-+
     7 6 5 4 3 2 1 0       7 6 5 4 3 2 1 0 
```


### SSO (Short String Optimization)

A short string is <= 11/23 bytes long. When the string is 11/23 bytes, the last byte
is zero, meaning it is still nul terminated. 

 * L == 0
 * .capacity() -> 11/23
 * .size() -> .capacity() - short_size
 * .c_str() -> &self
 * Bytes beyond the actual string must be zero. This makes it possible to do
   fast comparison.


### Normal String

 * L == 1
 * .capacity() -> big_endian ? _capacity - 1 : _capacity << 1
 * .size() -> _size
 * .c_str() -> _ptr

