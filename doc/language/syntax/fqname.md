# fqname

## Syntax

`.`__*__ [_identifier_](identifier.md) __(__ `.` [_identifier_](identifier.md) __)*__


## Semantics

A name that is a path traversing modules or namespaces. It is mostly used
for declaring types in another namespace than the current, and for declaring
modules.

A _fqname_ starts with zero or more dots `.`. A single dot `.` prefix
refers to the root namespace. If the _relative_fqname_ starts with two or
more dots `.`; each dot backtracks to the next parent of the current namespace.

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

