# Niche values

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

## \_\_niche\_max\_\_()

`__niche_max__(self) -> int[M..=M]`

Returns the number of niche values. `M` is the result of `__niche_max__()`.


## \_\_niche\_index\_\_()

`__niche_index__(self) -> int[0..=M]`

Returns the index of the niche value,
   or `__niche_max__()` when `self` is in-domain.


## \_\_niche\_set\_index()

`__niche_set_index(self, index : int[0..<M])`

Set the niche value.

# Niche mask

If types expose a niche-mask then multiple enum-names may have arguments. This
requires the `1`-bits in a niche-mask of the enum-names to overlap, the largest
set of consecutive `1`-bits are used as the niche-index of the enum.

For example two objects of `int[0..=127]` have an overlapping niche-mask of
`0x80` and thus can share an enum with each other.

If one object is smaller, then the extra padding bits are treated as `1` in the
niche-mask, even if that object's types does not expose an explicit niche-mask
itself.

```
enum {
    a(int[0..=127])
    b(int[0..=127])
}
```

## \_\_niche\_mask\_\_()

`__niche_mask__(self) -> int[0..=2**N]`

Returns a mask of `N` bits where `1` means that the bit may be used for a
niche value. The value of these bits do not change the invariant status
of the object.

# Nested Enums

A enum-type will synthesize `__niche_*__()` functions that will expose the
left over niche-values and niche-mask bits. This makes it possible for
enum-types to be nested without using extra space.
