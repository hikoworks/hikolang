# namespace

## Syntax

`namespace` _namespace-name_ `;`

_namespace-name_ := [_fully_qualified_name_](fully_qualitified_name.md)


## Semantics

Set the current namespace. Either relative to the current namespace, or when
the _namespace-name_ starts with `.` a namespace relative to root.

It is not possible to set the current namespace to the root itself.

```
module com.foo 1.0.0
// At this point the `.com.foo` namespace is open.
namespace bar
// At this point the `.com.foo.bar` namespace is open.
namespace .qux
// At this point the `.qux` namespace is open.
```

Sub-modules will inherit the default namespace of its anchor-module, so:

```
module com.foo.thud
// At this point the `.com.foo` namespace is open.
```

## Adding objects to the root namespace

If you want object to be part of the root namespace you can use an absolute
fully qualified path for the object being defined, like:

```
// Define the function `bar()` in the root namespace.
fn .bar(x) {
    return x + 1
}
```
