# Customization Points

## Enum Index Compression

To reduce the space of the `optional` or similar types, a type may add an API
for creating an index out of niche (out-of-domain) values.

For example an `optional[int[0..=254]]` only needs to occupy a single byte, as
it can use 255 as a niche value.

The value returned from `__niche_index__()` is used to select one of the enum
names, the index equal to `__niche_max__()` is special as it will select the
enum name that has the type as an argument.

> [!NOTE]
> The niche indices from this API are used internally in the language. The
> external indices corresponding to the enum-names start at 0 and increment.

Here is an example of an enum with three names, one of them holds the type which
has two niche values. For best optimization the last enum-name should be the
one that holds the type.

```
enum my_enum {
    one,                  // niche: 0.
    two                   // niche: 1.
    three(int[0..=253]),  // niche: 2, the one beyond the niche is a normal value.
};

enum my_enum_less_optimized {
    one,                // nice: 0.
    two(int[0..=253]),  // nice: 2, the one beyond the niche is a normal value.
    three               // nice: 1.
};
```

If there are more enum-names than `__niche_max__() + 1` the enum will be encoded
with a separate index value.

The following functions need to be added for a type, for enum-types with a
single value to be compressed in the space of a single value:

### \_\_niche\_max\_\_()

`__niche_max__(self) -> int[M..=M]`

Returns the number of niche values. `M` is the result of `__niche_max__()`.


### \_\_niche\_index\_\_()

`__niche_index__(self) -> int[0..=M]`

Returns the index of the niche value,
   or `__niche_max__()` when `self` is in-domain.


### \_\_niche\_set\_index()

`__niche_set_index(self, index : int[0..<M])`

Set the niche value.


## Allocation Relocation

Since some base types like `long` and `string` allocate memory at compile time,
there needs to be a way to make these allocations survive into runtime. For
this it needs to be possible to relocate objects.

In the executable there will be a pre-populated read/write data segment which is
a memory-pool which will be managed by the `std.allocator`. All static variables
in the program are treated as an allocation in the memory-pool, each will be
marked by a `static`-flag. Allocations made by these static variables,
recursively, are also added to this memory-pool.

The last allocation in this memory-pool is for the `__reloc_patch_table__` table
that holds the pointer patches that have to be done at run-time. This patch
table is deallocated after the patches have finished.

> [!NOTE]
> This means that pointers in the .code segment can only point directly to
> static variables and recursively into its members. The .code segment can
> not point to objects that were allocated.

### \_\_reloc\_patch\_entry\_\_

A `__reloc_patch_entry__` entry is used to extract and patch the address of
a pointer in the executable. By using `mask` and `shift` it is able to handle
most forms of pointer-tagging.

```
class __reloc_patch_entry__ {
    offset : size_t
    target : size_t
    mask : pointer
    shift : int[-64..=64]
} 
```

 * offset: The offset of the pointer-to-patch in the .data segment.
 * target: The offset into the .data segment that the pointer must be rewritten
           to.
 * mask: The bits at the offset that will be patched, the other bits remain
         what they were.
 * shift: How many bits the target should be shifted before patching.


### \_\_reloc\_patch\_table\_\_

```
class __reloc_patch_table__ {
    size : size_t
    entries : array[__reloc_path_entry__, size]
}
```


### \_\_reloc\_get\_\_()

`__reloc_get__(self) -> generate[__reloc_patch_entry__]`

This is a generator-function that yields the pointers that need to be patched
when the application is loaded.

This function is called at the end of compilation, for each object reachable
through static variables and constants.

The defaulted version of `__reloc_get__()` simply recurses into the members of
its type.

> [!NOTE]
> If `__reloc_get__()` throws an error, then compilation will fail with
> an error. The implicit implementation of `__reloc_get__()` throws
> a `not_implemented` error.



