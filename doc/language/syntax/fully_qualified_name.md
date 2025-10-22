# fully qualified name

## Syntax

__(__ [_identifier_](identifier.md) __|__ `.`__*__ __)__ __(__ `.` [_identifier_](identifier.md) __)*__


## Semantics

If a fqname starts with a identifier, the fqname is relative to the current
namespace.

A single dot `.` prefix refers to the root namespace, it may be followed by
identifiers which will be relative to this root namespace.

Two or more dot `.` prefix will backtrack into the parent(s) of the current
namespace.

With the current namespace `.foo.bar.qux` here are a few examples:
 - `a.b` -> `.foo.bar.qux.a.b`
 - `.` -> `.`
 - `.a.b` -> `.a.b`
 - `..` -> `.foo.bar`
 - `..qux` -> `.foo.bar.qux`
 - `..a.b` -> `.foo.bar.a.b`
 - `...a.b` -> `.foo.a.b`
 - `....a.b` -> `.a.b`

As you may see from the examples, you can't directly make a fqname of the
current namespace itself. Instead you may use `..` followed by the last
identifier of the current namespace.


### Restricted context

In certain contexts, like in `module` declarations, the prefix dots `.`
are not allowed and the fully qualified name is implicitly relative
to root.

