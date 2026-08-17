# Bindings

The following value categories exist:
 - __value__: A literal value, or a variable bound to a storage
   location it is managing.
 - __reference__: A reference object, or a variable holding a reference
   to a storage location it does not itself manage.
 - __move-reference__: A reference object which allows the internals of the
   object referenced to be consumed. The moved-from object will be valid but
   indeterminate.

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
move-reference.


```
x := 42.0    // 'x' is the value 42.0
y := &x      // 'y' takes the reference of 'x'
z <- & := x  // 'z' takes the reference of 'x'

a := &y      // 'a' takes the reference of 'x' (it is not a reference to 'y').
```


```
foo = fn(a <- &) {
  a := a + 1.0
}

x := 42.0
foo(x)       // 'foo()' takes a reference of 'x' and x becomes 43.0.
```


```
// The return specification '-> &' is an explicit binding selector.
// So initializing a variable without a binding specification with the result
// of this function will cause the variable to be a reference.
foo = fn(a <- &) -> & {
  a := a + 1.0
  return a
}

x = foo(1.0) // 1.0 is materialized in the block of the caller.
             // 'x' is a reference to the materialized 1.0, now 2.0.
```

References are treated as-if they are non-nullable pointers, with special
functions/syntax it is possible to reseat a reference-variable.



## Binding Specification

The binding specification is how a variable, immutable or function parameter
infers its type. `<-` is the type-inference operator, there are other type-
coercion operators.

  variable binding   | `x: T`         | `x: const T`   | `x: &T`       | `x: &const T` | `x: &&T`      
 :------------------ |:-------------- |:-------------- |:------------- |:------------- |:--------------
  `a := x`           | `a: T`         | `a: T`         | `a: T`        | `a: T`        | `a: T`        
  `a <- := x`        | `a: T`         | `a: const T`   | `a: &T`       | `a: &const T` | `a: &&T`      
  `a <- * := x`      | `a: T`         | `a: const T`   | `a: T`        | `a: const T`  | `a: T`        
  `a <- & := x`      | `a: &T`        | `a: &const T`  | `a: &T`       | `a: &const T` | `a: &T`       
  `a <- &const := x` | `a: &const T`  | `a: &const T`  | `a: &const T` | `a: &const T` | `a: &const T` 
  `a <- && := x`     | `a: &&T`       | `a: &&const T` | -             | -             | `a: &&T`    

Function parameters follow the variable binding rules.

Immutable bindings will add `const` to the value or references. Immutables can
not be reseated; since values (in this case the non-nullable pointer) of
immutables are frozen.

  immutable binding  | `x: T`         | `x: const T`   | `x: &T`       | `x: &const T` | `x: &&T`      
 :------------------ |:-------------- |:-------------- |:------------- |:------------- |:--------------  
  `a = x`            | `a: const T`   | `a: const T`   | `a: const T`  | `a: const T`  | `a: const T`  
  `a <- = x`         | `a: const T`   | `a: const T`   | `a: &T`       | `a: &const T` | `a: &&const`  
  `a <- * = x`       | `a: const T`   | `a: const T`   | `a: const T`  | `a: const T`  | `a: const T`  
  `a <- & = x`       | `a: &T`        | `a: &const T`  | `a: &T`       | `a: &const T` | `a: &T`       
  `a <- &const = x`  | `a: &const T`  | `a: &const T`  | `a: &const T` | `a: &const T` | `a: &const T` 
  `a <- && = x`      | `a: &&const T` | `a: &&const T` | -             | -             | `a: &&const T`


## Binding Selector

When using a binding selector you add a binding-method to the expression. This
binding method can be consumed by an variable/immutable initializer or function
parameter to determine the binding method of the variable/immutable/function
parameter.

  binding selector | `x: T`         | `x: const T`   | `x: &T`       | `x: &const T` | `x: &&T`      
 :---------------- |:-------------- |:-------------- |:------------- |:------------- |:--------------
  `a := x`         | `a: T`         | `a: T`         | `a: T`        | `a: T`        | `a: T`
  `a := const x`   | `a: T`         | `a: T`         | `a: T`        | `a: T`        | `a: T`
  `a := *x`        | `a: T`         | `a: T`         | `a: T`        | `a: T`        | `a: T`
  `a := *const x`  | `a: T`         | `a: T`         | `a: T`        | `a: T`        | `a: T`
  `a := &x`        | `a: &T`        | `a: &const T`  | `a: &T`       | `a: &const T` | `a: &T`
  `a := &const x`  | `a: &const T`  | `a: &const T`  | `a: &const T` | `a: &const T` | `a: &const T`
  `a := &&x`       | `a: &&T`       | `a: &&const T` | -             | -             | `a: &&T`





