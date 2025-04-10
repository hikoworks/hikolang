# Flow control

Flow control statements are used to control the flow of execution in a program.
They allow you to make decisions, repeat actions, and handle errors.

Flow control statements include:
 - if-statement
 - do-while-statement
 - switch-statement
 - try-catch-statement
 - trap-catch-statement
 - break / continue / goto statements
 - return statement
 - throw statement
 - yield statement
 - async statement
 - await statement


## Else-clause
You may add a single `else` clause to a `if`, `while` or `for` statement. The
code-block in the `else` will be executed if the expression of the preceding
`if`, `elif`, `while` or `for` statement is `false`.

The `else` clause is not executed in a `while` or `for` loop if it has been
exited early by a `break` statement.

## Catch-clause
The `catch` clause is used to handle errors that are thrown in the expression
of a control-flow statement. A `catch` clause must follow the clause with
an expression that can throw an error. It is a static-error if a error that
can be thrown is not caught by a `catch` clause.

Errors thrown from inside the code-block of a `if`, `while` or `for` statement
are not caught by the `catch`-clause.

There are two forms of a `catch` clause:
 - `catch (error-expression) {...}` - When the error-expression matches an
   error, the error is caught and the code in the `catch` block is executed.
 - `catch {...}` - The code in the `catch` block is executed if any error is
   thrown in the preceding expression. Inside the error block the current
   error code is accessable as `$error`.

The error-expression is a comma `,` separated list of error names. An error
matches if the error name is the same as the error name in the list.

```
if (foo()) {
    // code
} catch (underflow, overflow) {
    // code
} else {
    // code
}
```

## Break statement
The `break` statement is used to exit a loop or switch statement early. The
`break` statement may include a label to specify which loop or switch statement
to exit.

```
bar: while (true) {
    while (true) {
        break bar
    }
}
```

## Continue statement
The `continue` statement is used to skip the current iteration of a loop and
continue to the next iteration. The `continue` statement may include a label to
specify which loop or switch statement to continue.

```
bar: while (true) {
    while (true) {
        continue bar
    }
}
```

In a `do-while` statement the `continue` statement will also skip the code in the
`do` part of the statement, and execution continues at the `while` expression.

```
do {
    ...
} while (true) {
    ...
    continue // continue execution at the while expression
    ...
}
```

## Goto statement
The goto-statement is used to jump to a specific label in the code. The
following label types are supported:
 - `goto foo` - jump to the normal- or loop-label `foo`
 - `goto case 2` - jump to the label `case 2` of a switch statement.
 - `goto else` - jump to the label `else` of a switch statement.

When a `goto case` statement is used the case-label must be unambiguous. Either
the `goto` is inside the `switch` statement containing the case label; or the
label is unique in the code block.

## If statement
The `if` statement consists of a set of clauses:
 - It starts with a required `if` clause.
 - It may be followed by zero or more `elif` clauses.
 - It may be followed by an optional `else` clause.
 - Each `if` and `elif` clause may be followed by `catch` clauses.

Only one code block is executed in a `if`, `elif`, `catch`, `else` chain. After
the first code block is executed, the `if` statement is exited.

```
if (expression) {
    ...
} catch (error-expression) {
    ...
} catch {
    ...
} elif (expression) {
    ...
} catch (error-expression) {
    ...
} else {
    ...
}
```

The expressions in the `if` and `elif` clauses are executed in order, until:
 - A expression after converting to boolean is `true`: the code in the
   corresponding block is executed.
 - If all the expressions after converting to boolean are `false`: The code in
   the optional `else` clause is executed.
 - An expression throws an error: The error is caught by the corrosponding
   `catch` clauses and the `catch` block is executed. If no `catch` clause
   matches the error, the error is trapped.

It is a reportable error if the expression can throw an error but no
catch-clause exists.
   
 - `if (var x = foo()) {}` : Init-expression
 - `if (let x = foo(); x == 5) {}` : Init-expression with condition-expression
 - `if (x == 5) {}` : Condition-expression
 - `if ((x = 5)) {}` : work-around for unallowed assignment-expression


If the expression is an init-expression, the variable created will be available
in the corrosponding and subsequent if-elif/else-clause blocks. variables with
the same name created in subsequent expressions will shadow the variable created
in previous expressions.



## Do-while statement

```
do {
    // code
} while (if-expression) {
    // code
} else {
    // code
} catch (error-match-expression) {
    // code
} catch {
    // code
}
```


## Switch statement

```
switch (if-expression) {
case case-expression:
    // code
    break;
case case-expression:
case case-expression:
    // code
    break;
case case-expression:
    // code
    fallthrough;
case case-expression:
    // code
    break;
default:
    // code
} catch (error-match-expression) {
    // code
} catch {
    // code
}
```