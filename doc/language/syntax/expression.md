# Expression

## Primary Expression

   Expression                             | Description
  :-------------------------------------- |:-------------------------
   `(` _expression_ `)`                   | Sub expression
   [_code-block_]                         | Code block
   [_integer-literal_]                    | Integer literal
   [_float-literal_]                      | Floating point literal
   [_string-literal_]                     | String literal
   [_tuple-literal_]                      |
   [_if-control-expression_]              |
   [_iterative-for-control-expression_]   |
   [_ranged-for-control-expression_]      |
   [_while-control-expression_]           |

[_code-block_]: code_block.md
[_float-literal_]: float_literal.md
[_integer-literal_]: integer_literal.md
[_string-literal_]: string_literal.md
[_tuple-literal_]: tuple_literal.md
[_type-expression_]: type_expression.md
[_simple-unit-expression_]: unit_expression.md
[_unit-expression_]: unit_expression.md
[_if-control-expression_]: if_control_expression.md
[_for-control-expression_]: for_control_expression.md
[_while-control-expression_]: while_control_expression.md
[_iterative-for-control-expression_]: iterative_for_control_expression.md
[_ranged-for-control-expression_]: ranged_for_control_expression.md

## Standard Operators

 * `a`: Left hand side value.
 * `b`: Right hand side value.
 * `r`: Right hand side reference
 * `T`: [_type-expression_]
 * `E`: [_error-list_]
 * `U`: [_unit-expression_]


  Operator       | Precedence | Function                    | Description
 :------------   |:---------- |:------------------------------ |:----------
  `a++`          |  2000 L    | `__post_increment__(a)`        |
  `a--`          |  2000 L    | `__post_decrement__(a)`        |
  `a(...)`       |  2000 L A  |                                | [_call-operator_]
  `a[...]`       |  2000 L A  |                                | [_index-operator_]
  `a.<name>`     |  2000 L    |                                | [_member-access_]
  `a::<name>`    |  2000 L    |                                | [_type-member-access_]
  `++b`          |  3000 R    | `__increment__(a)`             |
  `--b`          |  3000 R    | `__decrement__(a)`             |
  `+b`           |  3000 R    | `__pos__(a)`                   |
  `-b`           |  3000 R    | `__neg__(a)`                   |
  `~b`           |  3000 R    | `__inv__(a)`                   |
  `not b`        |  3000 R    | `__not__(a)`                   |
  `await b`      |  3000 R A  |                                | [_await-operator_]
  `try b`        |  3000 R A  |                                | [_try-operator_]
  `&b`           |  3000 R    |                                | [_binding-selector_]
  `&&b`          |  3000 R    |                                | [_binding-selector_]
  `*b`           |  3000 R    |                                | [_binding-selector_]
  `const r`      |  3000 R    |                                | [_make-const-operator_]
  `?b`           |  3000 R    |                                | [_make-optional-operator_]
  `a * b`        |  5000 L    | `__mul__(a, b)`                |
  `a / b`        |  5000 L    | `__div__(a, b)`                |
  `a % b`        |  5000 L    | `__rem__(a, b)`                |
  `a &* b`       |  5000 L    | `__mod_mul__(a, b)`            |
  `a &/ b`       |  5000 L    | `__mod_div__(a, b)`            |
  `a + b`        |  6000 L    | `__add__(a, b)`                |
  `a - b`        |  6000 L    | `__sub__(a, b)`                |
  `a &+ b`       |  6000 L    | `__mod_add__(a, b)`            |
  `a &- b`       |  6000 L    | `__mod_sub__(a, b)`            |
  `a << b`       |  7000 L    | `__sal__(a, b)`                |
  `a >> b`       |  7000 L    | `__sar__(a, b)`                |
  `a &>> b`      |  7000 L    | `__slr__(a, b)`                |
  `a #U`         |  7500 L    |                                | [_unit-expression_]
  `T #U`         |  7500 L    |                                | [_unit-expression_]
  `a <=> b`      |  8000 L    | `__cmp__(a, b)`                |
  `a < b`        |  9000 L    | `__lt__(a, b)`                 |
  `a > b`        |  9000 L    | `__gt__(a, b)`                 |
  `a <= b`       |  9000 L    | `__le__(a, b)`                 |
  `a >= b`       |  9000 L    | `__ge__(a, b)`                 |
  `a == b`       | 10000 L    | `__eq__(a, b)`                 |
  `a != b`       | 10000 L    | `__ne__(a, b)`                 |
  `a in b`       | 10000 L    | `__in__(a, b)`                 |
  `a not in b`   | 10000 L    | `__not_in__(a, b)`             |
  `a & b`        | 11000 L    | `__bit_and__(a, b)`            |
  `a ^ b`        | 12000 L    | `__bit_xor__(a, b)`            |
  `a \| b`       | 13000 L    | `__bit_or__(a, b)`             |
  `a and b`      | 14000 L A  |                                | [_shortcut-operator_]
  `a or b`       | 15000 L A  |                                | [_shortcut-operator_]
  `a catch b`    | 15100 R A  |                                | [_catch-operator_]
  `a catch(E) b` | 15100 R A  |                                | [_catch-operator_]
  `a empty b`    | 15100 R A  |                                | [_empty-operator_]
  `a :~+ T`      | 15500 L    | `__convert_widen__(a, T)`      | [_type-operator_]
  `a :~! T`      | 15500 L    | `__convert_truncate__(a, T)`   | [_type-operator_]
  `a :~? T`      | 15500 L    | `__convert_narrow__(a, T)`     | [_type-operator_]
  `a :+ T`       | 15500 L    |                                | [_type-operator_]
  `a :! T`       | 15500 L    |                                | [_type-operator_]
  `a :? T`       | 15500 L    |                                | [_type-operator_]
  `a : T`        | 15500 L    |                                | [_type-operator_]
  `a = b`        | 16000 R A  | * `__merge__(a, b)`            | [_variable-definition_]
  `a := b`       | 16000 R A  | * `__assign__(a, b)`           | [_variable-definition_]
  `a += b`       | 16000 R    | `__inplace_add__(a, b)`        |
  `a -= b`       | 16000 R    | `__inplace_sub__(a, b)`        |
  `a *= b`       | 16000 R    | `__inplace_mul__(a, b)`        |
  `a /= b`       | 16000 R    | `__inplace_div__(a, b)`        |
  `a %= b`       | 16000 R    | `__inplace_mod__(a, b)`        |
  `a <<= b`      | 16000 R    | `__inplace_sll__(a, b)`        |
  `a >>= b`      | 16000 R    | `__inplace_sar__(a, b)`        |
  `a &>>= b`     | 16000 R    | `__inplace_slr__(a, b)`        |
  `a &= b`       | 16000 R    | `__inplace_and__(a, b)`        |
  `a ^= b`       | 16000 R    | `__inplace_xor__(a, b)`        |
  `a \|= b`      | 16000 R    | `__inplace_or__(a, b)`         |

> [!note]
> \* `a = b` and `a := b` are assignment operators or variable definitions
> depending on the context.

[_await-operator_]: await_operator.md
[_binding-selector_]: binding_selector.md
[_catch-operator_]: catch_operator.md
[_call-operator_]: call_operator.md
[_empty-operator_]: empty_operator.md
[_error-list_]: error_list.md
[_index-operator_]: index_operator.md
[_make-const-operator_]: make_const_operator.md
[_make-optional-operator_]: make_optional_operator.md
[_member-access_]: member_access.md
[_type-operator_]: type_operator.md
[_shortcut-operator_]: shortcut_operator.md
[_try-operator_]: try_operator.md
[_type-member-access_]: type_member_access.md
[_variable-definition_]: variable_definition.md

## Semantics

An _expression_ is a sequence of symbols that can be evaluated to produce a
value. When the result of the expression is not used, the restriction on the
type of the result is relaxed.

For parsing expressions an algorithm like [Shunting
Yard](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) is used.
The other expressions listed in this document are terminal expressions which
are parsed outside of the shunting yard.

