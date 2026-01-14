# Value Categories


## Move properties

Simple `class` and `enum` types have the following move-properties by default:

 * memset: An object can be initialized by setting the members to pre-declared values.
 * memcopy: An object can be copied/moved using a memory copy.
 * memmove: An object can be moved by copying memory then clearing the old.
 * memswap: An object can be swapped/moved by swapping memory.

When a destructor is added to a type, the `memcopy` and `memmove` properties are disabled. `memswap` would
almost always still work correctly.

If a default-constructor is added to the type, the `memset` property is disabled.

If you added a `__swap__()` function, `memswap` property is disabled
