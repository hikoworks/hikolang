# __builtin_alloc

## Types
   Type          | Description
  :------------- |:----------------
   `__size`      | Register holding an array size.
   `__ptr_size`  | `{ p : __ptr, s : __size }`


## Functions

   Function                                                 | Description
  :-------------------------------------------------------- |:----------------
   `__builtin_alloc(size : __size, align : __size, return (__ptr, __size))` | allocate
   `__builtin_dealloc(addr : __ptr, size : __size)`         | deallocate


### __builtin_alloc()

Allocate memory of at least `size` bytes, aligned to `align` bytes. It
returns a pointer to the allocated memory and the actual allocated size.

The `align` value may be any power-of-two up-to the alignment of smallest page
size that is supported by memory mapping functions of the operating system.
The alignment may be internally rounded up to another power-of-two.

It may throw `bad_alloc` when the allocation fails, this is extremely uncommon
in modern operating systems with paging and swap.

When allocating zero bytes, it will return a non-null pointer to the smallest
allocation possible, and return the actual allocated size.


### __builtin_dealloc()

Deallocates previous allocated memory at `ptr`. If `ptr` is set to an incorrect
address, including null, this function will throw a `bad_alloc`.

The `size` parameter should be set between the requested and the actual amount
of memory allocated by `__builtin_alloc()`. Setting the `size` incorrectly does
not impact correctness, but it may impact performance.

