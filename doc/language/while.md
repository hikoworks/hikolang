# While statement

## Syntax
<style type="text/css">
strong {
    color: green !important;
    font-weight: normal !important;
}
</style>

__(__ `do` `{` _statement-list_ `}` __)?__
`while` `(` _condition-expression_ `)` __(__ `{` _statement-list_ `}` __)?__ _catch-clauses_**?**
__(__ `elwhile` `(` _condition-expression_ `)` __(__ `{` _statement-list_ `}` __)?__ _catch-clauses_**?** __)*__
__(__ `else` `{` _statement-list_ `}` __)?__

## Condition expression
See the [condition expression](#condition-expression) section in the [if
statement](if.md) page for details.

## While statement as an expression
When using the `while` statement as an expression, the result will be a list of
items created by the `result` statements in the code-blocks. In loops the
`result` statement acts like a local `yield` statement and does not end the loop.

example:
```
var i = 0
var table = do {
    result i * 10
} while (i < 5) {
    result i++
}
// table = [0, 0, 10, 1, 20, 2, 30, 3, 40, 4, 50]
```

As a short-hand for a `while` statement without a `do` clause; the `result`
statement is optional, if there is no `result` statement, the last expression
of the code-blocks is used as the result:
```
var i = 0
var table = while (i < 5) { i++ } else { 42 }
// table = [0, 1, 2, 3, 4, 42]
```

