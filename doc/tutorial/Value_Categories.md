# Value Categories

The following value categories exist:
 - __value__: A literal value, or a variable bound to a storage
   location it is managing.
 - __reference__: A reference object, or a variable holding a reference
   to a storage location it does not itself manage.
 - __move-reference__: A reference object which allows movement from the
   referenced object.
 - __automatic-binding__: A variable that can hold a value, reference or
   move-reference. Used for templates to perfectly forward objects of any of the
   value categories.

Although an automatic-binding is listed as a value category, it is not an actual
value category itself. Rather, it is a binding mechanism that preserves the
value category of the object it refers to.

```
 +-------------------+     preserves  +----------------+
 | automatic binding |---+----------->| move reference |
 +-------------------+   |            +----------------+
                         |                  |
                         |                  | decays
                         |                  V
                         | preserves  +-----------+
                         +----------->| reference |
                         |            +-----------+
                         |                  |      
                         |                  | decay
                         |                  V      
                         | preserves  +-----------+
                         +----------->|   value   |
                                      +-----------+

```


## Type and Binding Specification

```
x = 42.0

a = x               // is a copy
b <- = x            // is a copy
c <- * = x          // is a copy
d <- & = x          // is a reference to x
e <- &const = x     // is a reference to const x
f <- && = x         // is a move-reference to x
g <- &&const = x    // what even is a move-reference to a const x?
```

```
y = &x              // is reference to x

h = y               // is a copy
i <- = y            // is a reference to x
j <- * = y          // is a copy
k <- & = y          // is a reference to x
l <- &const = y     // is a reference to const x
//m <- && = y       // ERROR: can't convert reference to move-reference.
//n <- &&const = y  // ERROR: can't convert reference to move-reference.
```

```
foo = fn(a)            { return 1 }
bar = fn(a <-)         { return 2 }
foo = fn(a <- *)       { return 3 }
foo = fn(a <- &)       { return 4 }
foo = fn(a <- &const)  { return 5 }
foo = fn(a <- &&)      { return 6 }
foo = fn(a <- &&const) { return 7 }
```

## Binding Selector


  selector:      | `a`         | `*a`      | `&a`       | `&const a` | `&&a`
 :-------------- |:----------- |:--------- |:---------- |:--------   |:-----
  `a: T`         | `T`         | `T`       | `&T`       | `&const T` | `&&T`
  `a: const T`   | `const T`   | `const T` | `&const T` | `&const T` | `&&const T`
  `a: &T`        | `&T`        | `T`       | `&T`       | `&const T` | -
  `a: &const T`  | `&const T`  | `const T` | `&const T` | `&const T` | -
  `a: &&T`       | `&&T`       | `T`       | `&T`       | `&const T` | `&&T`
  `a: &&const T` | `&&const T` | `const T` | `&const T` | `&const T` | `&&const T`

```
x = 42.0

a = x
//b : & = x
b = &x
//c : &const = x
c = &const x
//d : && = x
d = && x
//e : &&const = x
e = &&const x
```

## Value

Values are:
 - Literal
 - Result of an expression
 - A variable or immutable which binds and manages the storage of a value.

```
// Argument 'a' will make a copy of the value passed into it.
foo = fn(a) {
    return a
}

x = 42      // 'x' is bound and manages the storage of the value 42.
y = foo(x)  // The value 42 is passed into the function foo(),
            // then 'y' is initialized with the returned value.
```

## Reference

The type of a reference is `&T`. References cannot be nested. Consequently,
`&&T` does not denote a reference to a reference; `&&T` is the syntax for an
alias.

Most operations are proxied to the value it references. This means when a
reference is used it acts as-if it is the value itself.

When a reference is used as the initializer of a variable without a type, or
passed as a function argument without a type the reference decays into a value.

A reference variable is a variable that is prefixed with `&` during its
definition, or when reseating the reference.

```
x = 42.0  // 'x' is the value 42.0
&y = x    // The initializer of '&y' takes the reference of 'x'
z = y + 1 // 'y' decays into the value it references.
```

A reference is treated as a reference only when in a reference context.
Reference contexts are:
   + The initializer of a referenc-variable
   + In a function call for reference-arguments
   + The right hand side of the `&` prefix operator

```
foo = fn(&a) {
  a := a + 1.0
}

x := 42.0
foo(x)       // 'foo()' takes a reference of 'x' and x becomes 43.0.
```

If the result of an expression is in a reference context, the expression is
materialized as an anonymous variable, a reference is taken from this variable.
The anonymous variable's life-time is terminated at the end of the block it was
defined in, in reverse order among other variables created in this block.

```
foo = fn(&a) -> & {
  a := a + 1.0
  return a   // The return type puts 'a' into a reference-context.
}

&x = foo(1.0) // 1.0 is materialized in the block of the caller.
              // &x is a reference to the materialized 1.0, now 2.0.
```

When reassigning to a reference-variable, the assignment operator is applied
to the value the reference is pointing to.

```
x := 42.0
y := 10.0

&a := x
a := 3.0  // 'x' now has the value '3.0'.
&a := y   // reseat '&a' to the storage location of 'y'
a := x    // 'y' now has the value '3.0'.
```


## automatic binding

Automatic binding preserves the value and value-category of the object
passed to it. This is useful when creating template functions that accepts
objects of different value categories.

Automatic binding is a variable or function argument which is prefixed with
`=>`.

```
foo = fn(=>a) {
    repr(&a) // pass a reference or value to repr(). 
    print(a) // a perfectly forwards itself to print().
}

x := 42.0
&y := x

foo(x)   // a : f64
foo(y)   // a : &f64
foo(*y)  // a : f64
foo(&x)  // a : &f64
foo(&y)  // a : &f64
foo(&&x) // a : &&f64
x := 3.0 // x was moved from, set to a proper value again.
foo(&&y) // a : &&f64
```


