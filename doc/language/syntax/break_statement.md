# break statement

## Syntax

_break_statement_ :=\
      `break` [_label_]**?** `;`\
    __|__ `break` [_label_]**?**  `with` [_expression_]__?__ `;`\
    __|__ `break` [_label_]**?**  `throw` `fatal`__?__ [_identifier_] `;`

[_expression_]: expression.md
[_identifier_]: identifier.md
[_label_]: label.md


## Semantics

The `break` statement is used to break out of a block or control flow
expression.

A `break` without a [_label_] will break from single block or control flow
expression.

A `break` with a [_label_] will break from the control flow expression
with that same [_label_].

A `break` with a `with` clause will return a value from the the block or control
flow expression that is used in an expression expecting a value. The `break` in
this case will break from the nearest enclosing control flow expression where
the returned value is used.

A `break` with a `throw` clause will return an error or fatal error in the same
way as `break` `with` returns a value, for example:

```
if (a = if (x < 10) { break with 42 } else { break throw out_of_bound }) {
   print(a)
} catch (out_of_bound) {
   print("oops")
}
```
