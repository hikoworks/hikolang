# Tutorial

## Variables

A variable is manages the storage of a value of a specified type and a way to
access that value. A variable is introduced in a scope by using the `:=`
assignment-operator. A variable's value may be modified later within the scope.

```
sum := 42 : i32
for (x in 0..=100) {
    sum := sum + x
    sum += 1
}
```

### Type

The type of the variable is copied from the expression on the right hand side.

Sometimes it is required to convert the type of the expression to make a proper
variable. In the example `sum := 42 : i32` the literal `42` has the implicit
type `int[42..=42]`. Since a `int[42..=42]` can only hold the value `42` it
needs to be converted to a integer with a larger interval, in this case `i32`.

### Type capabilities

A type is associated with capabilities, which determines if member-functions
associated with that type can be called.

By default all capabilities are turned on by default. When converting to a
reference, capabilities can be implicitly removed. Capabilities can only be
explicitly added.

The following capabilities exists by default:

   match        | remove       | add          | Description
  :------------ |:------------ |:------------ |:---------------
   `#mutable`   | `#-mutable`  | `#+mutable`  | Can call member functions that mutate members variables
   `#private`   | `#-private`  | `#+private`  | Can call member functions from the private interface.


```
// Function that removes the `mutable` capability of the argument passed in.
fn foo(a : &T #-mutable) {}
// Function can only be called when the argument has the `mutable` capability.
fn bar(a : &T #mutable) {}
// Error: function arguments can not implicitly add a capability.
//fn baz(a: &T #+mutable) {}

a := T()               // Constructors implicitly remove `private` capability.
b := &a : #-mutable    // Remove `mutable` capability.
// c := &b : #mutable  // Error: `b` is not `mutable`.
// d := &b : #private  // Error: `b` is not `private`.
e := &b : #+mutable    // Explicit add `mutable` capability.
bar(e)
```


### frozen

### Sealing variables

```
foo := 42
seal foo
// `foo` and the value `42` can no longer be modified
```

### Definitions

An immutable is like a variable, except that it's value can not be modified
after its introduction, we call this "sealing". An immutable is introduced in a
scope by using the `=` definition-operator.

```
sum := 0 : i32
for (x in 0..=100) {
    y = x + 1
    sum += y
}
```

Types may specify that bindings to a definition is lazilly-sealed, this means
that the definition can be modified up-to the point where the definition is
used/read. For example a function-overload-set is not sealed until one of the
functions in the set is called. Functions also implement the merge-operator,
which will merge the previous definition with the new definition; adding a
function to the overload-set.

```
foo = fn() {
    return 42 : i32
}

foo = fn(x) {
    return 42 + x : i32
}

x = foo() // Seals immutable `foo`
y = foo(3)
```

Here is another example of a lazilly-sealed definition of a member-function
being added to an existing type.

```
foo = struct {
    a = 0 : i32

    fn __init__(self, a) {
        self.a = a
    }
}

foo.bar = fn(self : foo, x) {
    return self.a + x
}

x = foo(5) // seals foo, including foo.bar
y = x.bar(42)
```

# Aspect Oriented Programming

```
fn bar(x : int) {
    return x
}

fn baz(x : int) {
    return x + 1
}

fn foo(x : int) {
    return x + 42
}

fn before /ba./(x : int) {
    print(x)
}
```