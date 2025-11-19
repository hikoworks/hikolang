# relative_fqname

## Syntax

`.`__+__ [_fqname_](fqname.md)


## Semantics

A _relative_fqname_ starts with one or more dots `.`. A single dot `.` prefix
refers to the root namespace, which makes the _relative_fqname_ an absolute
name. If the _relative_fqname_ starts with two or more dots `.`; each dot
backtracks to the next parent of the current namespace.

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

