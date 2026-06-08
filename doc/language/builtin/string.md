# string

A string is used for storing text in UTF-8 encoding, it has the following
features:
 - Type is 16 bytes in size.
 - The string is nul-terminated to be compatible with C-ABI.
 - SSO (Short String Optimization) for strings up to 15 characters length.
 - Maximum size is 4GB.

## Memory layout.

This is the memory layout of a string:
```
       big-endian:               little-endian
     7 6 5 4 3 2 1 0            7 6 5 4 3 2 1 0  
    +-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+
  0 |msb            |        0 |            lsb|
  1 |               |        1 |               |
  2 |               |        2 |               |
  3 |    Pointer    |        3 |    Pointer    |
  4 |               |        4 |               |
  5 |               |        5 |               |
  6 |               |        6 |               |
  7 |            lsb|        7 |msb            |
    +-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+
  0 |msb            |        0 |            lsb|
  1 |               |        1 |               |
  2 |               |        2 |               |
  3 |    Capacity   |        3 |    Capacity   |
  4 |               |        4 |               |
  5 |               |        5 |               |
  6 |               |        6 |               |
  7 |            lsb|        7 |msb            |
    +-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+
  0 |msb            |        0 |            lsb|
  1 |               |        1 |               |
  2 |               |        2 |               |
  3 |      Size     |        3 |      Size     |
  4 |               |        4 |               |
  5 |               |        5 |               |
  6 |               |        6 |               |
 15 |     SSO     |L|       15 |L|    SSO      |
    +-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+
```

### Encoding

The encoding flag is mostly used to optimize validity-checks, normalization and concatenation.

  Value | Name          | Description
  -----:|:------------- |:--------
      0 | No encoding   | Used when just storing bytes.
      1 | WTF-8         | UTF-8-like encoding for storing file names.
      2 | UTF-8         | Valid UTF-8
      3 | UTF-8 NFC     | Valid UTF-8 and also normalized into NFC.


### Normal

 - The `L` bit is `1`.
 - The allocator always returns a buffer of even size.
 - `Capacity` contains the size of the alloction:
   - big-endian: clear the least-significant-bit to get the capacity.
   - little-endian: Shift left by 1 to get the capacity.
 - `Size` contains the length of the string (excluding nul-termination).
 - `Pointer` points to the start of the string.
   - Bottom 3 bits can be used flags:
     - Checked: If the string has been checked for UTF-8 validity
     - UTF-8: The string is valid UTF-8

### Short String Optimization

 - The `L` bit is `0`.
 - The 7 `SSO` bits contains the number of unused characters in the SSO.
   - 15: string length is zero.
   - 0: string length is 15 (this value doubles as the nul-termination)
 - The `Pointer`, `Size` and `Capacity` is used for the string bytes
 - The string is padded with zero bytes upto the SSO field so that equality
   comparison is a 128-bit compare.
