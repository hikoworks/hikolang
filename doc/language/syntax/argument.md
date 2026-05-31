# argument

## Syntax

_argument_ := __(__ `$`__?__ [_name_] `=` __)?__ [_expression_]

[_name_]: name.md
[_expression_]: expression.md


## Semantics

There are three forms of argument:
 - A positional argument.
 - A [_name_](name.md)ed argument.
 - A injected argument, which is a `$` followed by a [_name_](name.md).
   This is used to inject a value into the function recursively.


