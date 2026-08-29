# do-clause

## Syntax

_do-clause_ := `do` `{` [_code-block_] `}`

[_code-block_]: code_block.md

## Semantics

`do` introduces a block preceding a control-flow expression.
If the control-flow expression continues, the `do` block may be revisited according
to the semantics of that control-flow operator.

An `if` control-flow doesn't iterate, but the `do` clause does participates in producing
a value. For example:

```
a = do {
    if (x) {
        result 1
    }
} if (c) {
    result 2
} else {
    result 3
}
```

