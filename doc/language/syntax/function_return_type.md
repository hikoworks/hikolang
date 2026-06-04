# function-return-type

## Syntax

_function-return-type_ :=\
      `->` __(__ `yield` __|__ `await` __)*__ `(` [_type-expression_] `)`\
      `->` __(__ `yield` __|__ `await` __)*__ [_type-tuple-literal_]\
      `->` __(__ `yield` __|__ `await` __)*__ [_fqname_]

[_type-expression_]: type_expression.md
[_type-tuple-literal_]: type_tuple_literal.md
[_fqname_]: fqname.md

## Semantics
