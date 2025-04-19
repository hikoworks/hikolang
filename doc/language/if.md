# If statement

## Syntax
<style type="text/css">
strong {
    color: green !important;
    font-weight: normal !important;
}</style>

`if` `(` _condition-expression_ `)` `{` _statement-list_ `}` _catch-clauses_**?**
__(__ `elif` `(` _condition-expression_ `)` `{` _statement-list_ `}` _catch-clauses_**?** __)*__
__(__ `else` `{` _statement-list_ `}` __)?__

Alternatively when there are no `elif` clauses, the `catch` clauses may be
placed after the `else` clause.

`if` `(` _condition-expression_ `)` `{` statement-list `}`
`else` `{` statement-list `}` _catch-clauses_**?**

## Condition expression
The condition expression is a boolean expression that evaluates to true or
false.

The following rules apply to the condition expression:
 - Condition expressions are evaluated in the scope of the `if` statement.
   Variables declared in the `if` statement are visible in all code blocks
   within the `if` statement.
 - The condition expression is equivalent to a sub-expression, except:
    - The result must be convertible to a boolean value.
    - The result expression is not allowed to be an assignment expression. As
      a workaround an assignment expression may be contained inside a
      sub-expression.

## If statement as an expression
A `if` statement may be used as an expression, this is known as a
control-expression. When a `if` statement is used as a control-expression, then
the result value of all branches must be of the same time, or the result must be
convertible to the expected type.

Example:
```
function foo(x : int) -> int {
    let y = if (x > 0) {
        // implicit result: y is assigned x + z.
        let z = 3
        x + z
    } elif (x < 0) {
        // explicit result: y is assigned x - 1.
        result x - 1
    } else {
        // This directly returns from the function with the value 42.
        return 42
    }

    return y;
}
```
