# Long Integer

A long integer is one that has a variable size.

## Internal Representation

```
struct long {
    _data : u64
    _size : u32
    _capacity : u32
}
```

`_data` is a pointer to the a variable sized 2's compliment signed integer in
native endian order. `_size` contains the number of 'digits' (a digit is
an unsigned integer of the native register size). `_capacity` contains the
number of bytes of the allocation.

If `_capacity` is zero, then `_data` contains the integer itself SIO
(Short Integer Optimization).
