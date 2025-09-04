# Long Integer

A long integer is one that has a variable size.

## Internal Representation

```
## Memory layout.

This is the memory layout of a string:
```
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
  8 |msb            |        0 |            lsb|
  9 |     Size      |        9 |     Size      |
 10 |               |       10 |               |
 11 |            lsb|        7 |msb            |
    +-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+
 12 |msb            |        0 |            lsb|
 13 |   Capacity    |       13 |   Capacity    |
 14 |               |       14 |               |
 15 |            lsb|       15 |msb            |
    +-+-+-+-+-+-+-+-+          +-+-+-+-+-+-+-+-+
```

### Normal

 - The `Capacity` bit is not zero, contains the number of bytes allocated.
 - `Size` contains the number of digit-integers.
 - `Pointer` points to first digit-integers, the digits are in native endian order.

### Short Integer Optimization

 - The `Capacity` is 0, `Size` is 0 (simple to test).
 - The `Pointer` is a 64-bit signed integer
