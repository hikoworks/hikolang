# __builtin_int_X__

   Type               | # bits  | Description
  :------------------ | -------:|:----------------
   `__i`\<N\>         |      N  | A integer of N bits.
   `__i`\<N\>`x`\<M\> |  N x M  | A vector of M integers of N bits.
   `__f`\<N\>         |      N  | A float of N bits.
   `__f`\<N\>`x`\<M\> |  N x M  | A vector of M floats of N bits.
   `__ptr`            |      -  | A opaque pointer.

## Array indicing and allocation

   Type          | Description
  :------------- |:----------------
   `__size`      | Register holding an array size.
   `__index`     | Register holding an array size.
   `__limb`      | Register holding a limb of a large integer.
   `__addr_size` | `{ a : __addr, s : __size }`

`__builtin_alloc(size : __size) -> __addr_size`

`__builtin_dealloc(addr : __addr, size : __size)`


## Carry / Borrow

   Type       | Components                | Description
  :---------- |:------------------------- |:----------------
   `__i8c`    | `{ v : __i8 , c : __i1 }` | 8 bit register
   `__i16c`   | `{ v : __i16, c : __i1 }` | 16 bit register
   `__i32c`   | `{ v : __i32, c : __i1 }` | 32 bit register
   `__i64c`   | `{ v : __i64, c : __i1 }` | 64 bit register

## Wide

   Type       | Components                   | Description
  :---------- |:---------------------------- |:----------------
   `__i8w`    | `{ lo : __i8 , hi : __i8 }`  | 8 bit register
   `__i16w`   | `{ lo : __i16, hi : __i16 }` | 16 bit register
   `__i32w`   | `{ lo : __i32, hi : __i32 }` | 32 bit register
   `__i64w`   | `{ lo : __i64, hi : __i64 }` | 64 bit register




