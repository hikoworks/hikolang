# namespace-directive

## Syntax

namespace-directive :=\
      `namespace` [_fqname_] `;`\
    __|__ `namespace` `;`

[_fqname_]: fqname.md


## Semantics

Set the current namespace. Either relative to the current namespace, or when
the [_fqname_] starts with a single `.` a namespace relative to root.

A bare `namespace` without a [_fqname_] will reset the namespace back to the default
namespace of the file.

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


### Domains

There are distinct identifier domains. Each of those identifier domains are
managed using this _namespace_ statement. Here are the identifier domains:

 * Variables, including types
 * Meta-type keywords
 * Expression operator keywords
 * Units

 