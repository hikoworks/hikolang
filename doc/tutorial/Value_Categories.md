# Value Categories

The following value categories exist:
 - __value__: A literal value, or a variable bound to a storage
   location it is managing.
 - __reference__: A reference object, or a variable holding a reference
   to a storage location is does not itself manage. Operations on a
   reference are proxies to the value it points to.
 - __pointer__: Like a reference object, but it does not proxy operations.
 - __alias__: A variable that can hold a value, reference or pointer. Used
   for templates to perfectly forward objects of any of the value categories.

```
 +-------+     preserves  +-----------+
 | alias |---+----------->|  pointer  |
 +-------+   |            +-----------+
             |                  |
             |                  | decays
             |                  V
             | preserves  +-----------+
             +----------->| reference |-------+
             |            +-----------+       |
             |                  |             |
             |                  | decays      | proxies
             |                  V             |
             | preserves  +-----------+       |
             +----------->|   value   |<------+
                          +-----------+

```


## Value

The type of a value is `T`.

## Reference

The type of a reference is `&T`. References can't be nested, which means `&&T`
is not a reference to a reference (it is instead a pointer, or an alias).

Most operations are proxied to the value it references. This means when a
reference is used it acts as-if it is the value itself.

When a reference is used as the initializer of a variable without a type, or
passed as a function argument without a type the reference decays into a value.

A reference is treated as a reference only when in a reference taking context,
Reference taking context are:
   + The initializer of a variable with a reference-type
   + The argument with a reference-type of a function call
   + The right hand side of the `&` prefix operator

If an temporary expression is in a reference taking context, the expression is
materialized as an anonymous variable, a reference is taken from this variable.
The anonymous variable's life-time is terminated at the end of the block, in
reverse order among other variables created in this block.

>[!NOTE]
> Using a reference to reassigning to a reference-variable will copy the
> proxied object into the value referenced by the reference-variable. 


## Pointer

The `&` prefix operator creates an pointer from the reference taken from the
right hand side's reference taking context (see: Reference).

The type of a pointer is `&&T`.

A pointer does not proxy operations to the value it points to.

If a pointer initializes a non-typed variable, it sets the type of this variable
to a reference to the value it points to. This makes a initializer expression
a reference capturing context.

If a pointer is passed to a non-typed function argument, it sets the type of
this argument to a reference to the value it points to. This makes the argument
at the call-site a reference capturing context.

A pointer in a reference capturing context decays into a reference.

If a pointer is used to reassign a reference-variable, then the reference of
that variable is reseated.

Since pointers normally decays into a reference, you can only store a pointer in
a variable when it's type is a alias.


## Alias

A alias is a variable or function argument with a `&&T` type.

A alias preserves the value category of the object passed into
it. This is useful when creating template functions that accepts objects of
different value categories.

A alias can even preserve a pointer, which would normally decay into
a reference.

```
foo = fn(a : &&) {
    b = a // One step of dereferencing.
    return b
}

x := 42.0
y := &x

foo(x)  // a = 42.0, b = 42.0, return 42.0
foo(y)  // a->x,     b = 42.0, return 42.0
foo(&x) // a = &x,   b->x,     return 42.0
```

