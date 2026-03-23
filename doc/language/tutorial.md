# Tutorial

## Variables

A variable is manages the storage of a value of a specified type and a way to
access that value. A variable is introduced in a scope by using the `:=`
assignment-operator. A variable's value may be modified later within the scope.

```
sum := 0 : i32
for (x in 0..=100) {
    sum := sum + x
    sum += 1
}
```

### const

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
