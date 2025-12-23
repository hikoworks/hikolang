# Relocation

Since some base types like `long` and `string` allocate memory at compile time,
there needs to be a way to make these allocations survive into runtime. For
this it needs to be possible to relocate objects.

In the executable there will be a pre-populated read/write `.data` rw-segment
which is a memory-pool which will be managed by the `std.allocator`. Allocations
made by static variables at compilation, recursively, are added to this
memory-pool.

The `.reloc` ro-segment contains the `__reloc_patch_table__` table that holds
the pointer patches that have to be done at run-time. This `.reloc` segment
may be unloaded after patching.



## \_\_reloc\_patch\_table\_\_

A `__reloc_patch_entry_type__` entry is used to extract and patch the address of
a pointer in the executable. By using `mask` and `shift` it is able to handle
most forms of pointer-tagging.

```
class __reloc_patch_entry_type__ {
    offset : size_t
    target : size_t
    mask : pointer
    shift : int[-64..=64]
} 
```

 * offset: The offset of the pointer-to-patch in the `.data` segment.
 * target: The offset into the `.data` segment that the pointer must be
           rewritten to.
 * mask: The bits at the offset that will be patched, the other bits remain
         what they were.
 * shift: How many bits the target should be shifted before patching.

```
class __reloc_patch_table_type__ {
    size : size_t
    entries : array[__reloc_path_entry_type__, size]
}

let [[segment=.reloc]] __reloc_patch_table__ : __reloc_patch_table_type__
```


## \_\_reloc\_get\_\_()

`__reloc_get__(self) -> generate[__reloc_patch_entry__]`

This is a generator-function that yields the patch-entries that will be used
to populate the `__reloc_patch_table__`.

This function is called at the end of compilation, for each object reachable
through static variables and constants.

The implicit version of `__reloc_get__()` simply recurses into the members of
its type.

> [!NOTE]
> If `__reloc_get__()` throws an error, then compilation will fail with
> an error. The implicit implementation of `__reloc_get__()` throws
> a `not_implemented` error.

