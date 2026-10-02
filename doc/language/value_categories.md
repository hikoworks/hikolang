# Binding Modes

Hikolang has three expression categories:
 * **value** - a value located in managed storage, or the temporary result of an
   expression.
 * **reference** - a reference to storage managed by something else.
 * **movable** - a reference that permits the internals of the referenced
   object to be consumed.

This distinction affects how variables are initialized, how function arguments
are passed, and whether modifying an object modifies the original object or a
copy.

Here are the possible bindings, including constness, and which conversions
can take place:

```
    const T ←────── T
       │            │
       │       ┌────┴────┐
       │       ↓         ↓
       │      &T ←────  &&T
       │       │
       │       ↓
       └──→ &const T
```


 Category  | Type       | Description
 :---------|:-----------|:---------
 value     | `T`        |
 reference | `&T`       |
 movable   | `&&`       |


## Type selector

 Selector   | Description
 -----------|------------
 `x`        | 
 `*x`       |
 `const x`  | 
 `&x`       | Suggest to take a reference
 `&const x` | Suggest to take a const reference
 `&&x`      | Forward


## Type specification

 Specification    | Description
 -----------------|------------
 `fn(x)`          | Take a value or a const reference based on performance.
 `fn(x : *)`      | Take a copy of the value
 `fn(x : &)`      | Take a reference to the value
 `fn(x : &const)` | Take a const reference to the value
 `fn(x : &&)`     | Require a movable
 `fn(x :)`        | Infer the binding mode
