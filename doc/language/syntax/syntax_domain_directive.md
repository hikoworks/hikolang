# syntax-domain-directive

## Syntax

_syntax-domain-directive_ :=\
      `syntax` `domain` [_identifier_] `(` _unit-list_ `)` `;`\
    __|__ `syntax` `domain` [_identifier_] `;`

_unit-list_ := [_identifier_] __(__ `,` [_identifier_] __)*__

[_identifier_]: identifier.md

## Semantic

A structured domain introduces a system of units, for example the SI-units are
declared as:

```
syntax domain SI (s, m, kg, A, K, mol, cd)
```

The domain `SI` and the units `s`, `m`, `kg`, `A`, `K`, `mol`, `cd` are added to
a global namespace that is valid inside a
[_unit-expression_](unit_expression.md). The list of units form the
dimensional-exponents-tuple used in dimensional analysis.

### Tag

A domain can be used to add a tag to a value, and constrain a type to have
that tag. You can add domains without any units for this particular purpose.

```
syntax domain my_flag

// Require boolean argument to be tagged.
foo = fn(flag : bool #my_flag) {
    return not flag
}

// Tag the boolean value.
x = foo(true #my_flag)
```
