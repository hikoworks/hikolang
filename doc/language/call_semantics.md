# Call Semantics

## Universal Call Syntax

A member function call in the form `a.<name>(...)` is always desugared as
`<name>(a, ...)`.

> [!note]
> Function calls of the form `a.b(...)` where `b` is not an identifier will not
> be desugared.

Member functions are normally desugared as free functions, so there are very few
cases where the member-call remains. By normalizing both the calls and the
member functions to free-functions we reduce the semantic complexity for both
searching and overload resolution.

A free-function call will look for `<name>` in the following order:
 - The current scope (as explicit override)
 - In the object passed as the first argument
 - Argument-Dependent Lookup
 - The current name-space (override global default within this module)
 - The global name-space (global default)

1. Collect candidates from the highest-priority lookup domain.
2. If that domain produces candidates, don't search lower-priority domains.
3. Perform overload resolution on those candidates.
4. If no viable overload exists, report an error.

## Free functions

A free function is specified as follows:

```rust
struct foo {
    z <- 10
}

fn bar(@self a : foo, b, c) {
    return a + b + z
}
```

The `@self` directive is short hand for both `@adl` and `@private`, any of these
may appear on each argument.

 - `@adl`: This free-function is added the the ADL (Argument-Dependent Lookup)
   list for the explicit type listed for this argument.
 - `@private`: The function's code block has access to private members of the
   object that is passed into this argument.


## Member Function Rewrite

A member functions like `bar` here:

```rust
struct foo {
    z <- 10.0

    fn bar(x, y) {
        return x + y + z
    }

    fn baz(x, y) {
        return x + y + $self.z
    }
};

a = foo()
b = a.bar(1.0, 2.0)  // b = 13.0
```

Is rewritten as a free function:

```rust
struct foo {
    z <- 10.0
}

// @self was implied as a member function.
fn bar(@self self : foo, x, y) {
    return x + y + self.z
}

a = foo()
b = bar(a, 1.0, 2.0)  // b = 13.0
```

## Dynamic Dispatch

Dynamic member functions like `bar` here:
```rust

@dynamic
struct foo {
    z <- 10.0

    @dynamic
    fn bar(x, y) {
        return x + y + z
    }
};

a = foo()
b = a.bar(1.0, 2.0)  // b = 13.0
```

Is rewritten as a free function that dispatches to the actual function through
the vtable of the object:

```rust
@dynamic
struct foo {
    z <- 10.0
}

fn foo__bar(@self self : foo, x, y) {
    return x + y + self.z
}

// The `:` is used as a forwarding reference.
fn bar(@self self : foo, x :, y :) {
    // `&&` forwards the arguments. 
    return self.__vtable[foo__bar__index](&&foo, &&x, &&y)
}

a = foo()
b = bar(a, 1.0, 2.0)  // b = 13.0
```


## Callable Member Values

As you see in the chapter above member functions will be desugared as
free-member functions. That leaves us with member variables that hold callable
objects or function pointers.

For example:

```rust
struct qux {
    fn __call__(x, y) {
        return x + y + 42.0
    }
}

struct foo {
    z <- 10.0
    bar <- qux()
};

a = foo()
b = a.bar(1.0, 2.0)  // b = 45.0
```
