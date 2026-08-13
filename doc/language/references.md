# References

A reference is like an alias to a storage location. A refernce proxies all operations on it to the referenced object.

## Creating references

A reference is created when an expression enters a reference context.

A reference context is created when:
 - In the initialization of a variable or inmutable when its type expects a reference.
 - In a function argument, when the type of the arguments expect a reference.

If the expression inside the reference context is a temporary object, then the temporary object
is actualized in an anonymous storage location inside the current block of code. This means that
the temporary object's lifetime ends at the end of the block, among other objects that get
destroyed in reverse order of creation.

```
foo = fn(a : &int) {
    return a
}

x = 42
y : &int = x  // Make a reference to x
z : &int = y  // Make a reference to x (not y)
w = foo(x) // the 'a' argument is a reference to x
v = foo(y) // the 'a' argument is a reference to x (not y)
```

## Non-proxy references

A non-proxy reference can be created from an expression with the `&` prefix operator.
A non-proxy reference can collapse into a normal reference in the following situations:
 - A variable has a reference type
 - A variable has no type
 - A function argument has a reference type
 - A function argument has no type
 - return from a function

 ```
 foo = fn(a : &int) {
    return a
 }

 bar = fn(a) {
    return a
 }

 qux = fn(a : &int) {
    return &a
 }

 x = 42
 y : &int = &x
 z = &x
 k = foo(&x)
 l = bar(&x)
 m = qux(x) // m is a reference to x.
 ```

A reference may be returned if its target storage is guaranteed to outlive the current function invocation; local variables and function-local temporaries do not satisfy this guarantee.

The non-proxy reference can also be used to reseat a reference:

```
a = 42
b = 5

x := &a // x->a
y := &x // y->a

x := 1 // a == 1, x->a, y->a
x := &b // a == 1, x->b, y->a
```

 ## Forwarding reference

 A forwarding reference preserves the value category of the expression used to initialize it.

 ```
foo = fn(a : &&int) {
    b = a
    return b
}

x = 42
y = &x

f = foo(42) // 'a' is a value, 'b' is a value, 'f' is a value
g = foo(x)  // 'a' is a value, 'b' is a value, 'g' is a value
h = foo(y)  // 'a' is a reference, 'b' is a value, 'h' is a value
i = foo(&x) // 'a' is a non-proxy reference, 'b' is a reference, 'i' is a value
 ```
