# Statement

# If statement

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



# Do-while statement

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

## optional: do-clause

## optional: while-clause

## optional: else-clause

## optional: catch-clause

# Switch statement

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