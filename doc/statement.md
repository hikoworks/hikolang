# Statement

## Flow control
Flow control statements are used to control the flow of execution in a program. They allow you to make decisions, repeat actions, and handle errors.
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


### Else-clause
You may add a single else-clause to a if-statement, do-while-statement or for-statemtnet.
The code-block in the else-clause will be executed if the expression of the preceding
if-statement / elif-clause, do-while-statement or for-statement is false.

The else-clause is not executed in a do-while-statement or for-statement if one of their
code-blocks executes a break-statement.

### Catch-clause
The catch-clause is used to handle errors that are thrown in the expression of a
preceding if-clause / elif-clause, while-clause, for-clause or switch-expression.

In a elif-clause chain the catch-clause must be added before the next elif-clause.

Errors thrown from inside the code-block of the if-statement, do-while-statement or
for-statement are not caught by the catch-clause. 

### Break / Continue / Goto statement
The `break` statement is used to exit a loop or switch statement early. The `break` statement
may include a label to specify which loop or switch statement to exit.

```
bar: while (true) {
    while (true) {
        break bar
    }
}
```

The `continue` statement is used to skip the current iteration of a loop and
continue to the next iteration. The `continue` statement may include a label to specify which loop
or switch statement to continue.

```
bar: while (true) {
    while (true) {
        continue bar
    }
}
```

The `continue` statement may also be used in a `do` `while` statement in this case continue
will skip both the while-part-2 and the do-part and continue with the next iteration
by executing the while expression.
```
bar: do {
    // do part
} while (true) {
    // while part 1
    continue bar
    // while part 2
}
```

The goto-statement is used to jump to a specific label in the code. The following label
types are supported:
 - `goto foo` - jump to label `foo`
 - `goto bar` - jump to the label `bar` of a loop.
 - `goto case 2` - jump to the label `case 2` of a switch statement.
 - `goto default` - jump to the label `default` of a switch statement.

When a goto case/default label is used the label must be unambiguous. Either the
goto is inside the switch statement or the label is unique in the code block.
It is a reportable error if the label is ambiguous.

## If statement

```
if (expression) {
    // code
} elif (expression) {
    // code
} else {
    // code
} catch (error-expression) {
    // code
} catch {
    // code
}
```

The if-statement consists of a set of clauses:
 - It starts with a required if-clause.
 - It may be followed by zero or more elif-clauses.
 - It may be followed by an optional else-clause.
 - It may be followed by zero or more error matching catch-clauses.
 - It may be followed by an optional default catch-clause.

The expressions in the if/elif-clauses are executed in order, until one of the expressions:
 - yields a truthy value: the code in the corresponding block is executed and further processing
   of the if-statement is skipped.
 - If none of the expressions yield a truthy value: The code in the optional else-clause is executed
   and further processing of the if-statement is skipped.
 - throws an error: The error is caught by the catch-clause and the corrosponding block is
   executed. If no catch-clause is executed, the error is trapped.

It is a reportable error if the expression can throw an error but no catch-clause exists.
   
 - `if (var x = foo()) {}` : Init-expression
 - `if (let x = foo(); x == 5) {}` : Init-expression with condition-expression
 - `if (x == 5) {}` : Condition-expression
 - `if ((x = 5)) {}` : work-around for unallowed assignment-expression


If the expression is an init-expression, the variable created will be available in the
corrosponding and subsequent if-elif/else-clause blocks. variables with the same name created
in subsequent expressions will shadow the variable created in previous expressions.



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

### optional: do-clause

### optional: while-clause

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