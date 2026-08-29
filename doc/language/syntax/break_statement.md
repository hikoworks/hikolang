# break statement

## Syntax

_break_statement_ :=\
      __(__ `break` _label_**?** `;` __)__\
    __|__ __(__ `break` _label_**?**  `with` `;` __)__\
    __|__ __(__ `break` _label_**?**  `with` [_expression_] `;` __)__\
    __|__ __(__ `break` _label_**?**  `throw` [_idenitifier_] `;` __)__\
    __|__ __(__ `break` _label_**?**  `throw` `fatal` [_idenitifier_] `;` __)__

[_expression_]: expression.md
[_identifier_]: identifier.md
[_label_]: label.md


## Semantics

The `break` statement is used to break out of a block or control flow expression.

A `break` without a label and without `with` or `throw` clause will break from a
single block or control flow expression.

A `break` with a label will break from the block or control flow expression with
that same label.

A `break` with a `with` clause will return a value or empty from the the block or
control flow expression that may be assigned or initialize a variable or immutable
or passed as an argument to a function. The `break` in this case will break from
the nearest enclosing from where the returned value is used. If a [_label_] is
passed the labelled block will return the value.

A `break` with a `throw` clause will return an error or fatal error in the same
way as `break` `with` returns a value. Example:

```
if (a = if (x < 10) { break 42 } else { break throw out_of_bound }) {
   print(a)
} catch (out_of_bound) {
   print("oops")
}
```
