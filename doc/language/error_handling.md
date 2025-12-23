# Error handling

For ease of use, errors are thrown and caught, but they are not exceptions,
within this language we call them errors. Errors automatically convert a return
value of a function into a `std.result[T]`

A caller is responsible for catching all errors that can be thrown by an
expression. The compiler will check if all errors that can be thrown are caught.

Therefor all control flow is local, making it easier to reason about the code.

Features:
 - Although errors are thrown, they are not exceptions. 
 - No non-local control flow.
 - Compiler checks if all errors that can be thrown are caught directly by
   the caller.
 - Declaring a new error is easy; by simply throwing, trapping or catching it.
 - Very fast throw and catch recoverable errors.
   - No setup or teardown on catch or call.
   - Throwing is done by setting the return registers to an error code, and
     instruction pointer and setting the carry flag.
   - Initial check for an error is by checking the carry flag.
   - Catching specific errors is done by comparing the error code returned
     in a register by known constant values.
   - Rethrowing is the same as throwing, but keeping the register with the
     instruction pointer intact.
 - Separate from recoverable errors, you can trap fatal errors.
   - Normally terminates the application.
   - Possible to catch a trap for unit-testing and isolating programming bugs
     by terminating the connection to a client.
   - Slower than throwing an error.


## std.error_code

By throwing or catching; an error-name is implicitly declared. The error-name
is automatically added to the `std.error_code` enum.


## std.result

Any function which throws, rethrows *errors*, or ignore a *fatal-errors* will
convert a function's return type from `T` (or `std.optional[T]`) to
`std.result[T]`.

```
enum std.result[T : type] {
    none,
    value(T),
    error(std.error_code),
    fatal_error(std.error_code)
}
```


## Throwing an error

The `throw` statements is used to throw an *error*. The argument of the `throw`
statement is the error-name, and optionally a string literal with a message.
A thrown *error* must be directly caught by the calling function.

```
if (a < 0) {
    throw out_of_bounds_error, "a should be larger or equal to 0"
}
```

The compiler tracks which *errors* may be thrown by a function, and the compiler
will give an error message if not caught by the caller. Rethrown errors are
tracked as well. 

> [!NOTE]
> You can not throw an *error* from a destructor. 


## Throwing a fatal-error

When the error-name is suffixed with a exclamation mark `!` then a *fatal-error*
is thrown. A *fatal-error* does not need to be caught by the calling function. A
*fatal-error* unwinds the stack until caught; unless the *fatal-error* is never
caught, then the `throw` is converted to a `trap`.

```
if (a < 0) {
    throw out_of_bounds_error!, "a should be larger or equal to 0"
    //                       ^ this makes it a fatal-error.
}
```

The compiler will track thrown *fatal-errors* and if the compiler detects a
*fatal-error* is never caught, it will convert the `throw` into a `trap`.

> [!NOTE]
> You may throw an *fatal-error* from a destructor. 


## Trapping an error

The `trap` statement is used to trap an error. A trap will immediately abort an
application; if possible the CPU's `trap` instruction (int 3 on x86) is used.

The `trap` handler will do the following:

 * Pause all threads of the application.
 * Call the function registered with `std.set_trap_handler()`.
 * The default trap handler does:
    * Print an error message on the console.
    * Create a core-dump / mini-dump.


## Assertions

The following assert and contract statements throw `assertion_failure!`:
 * `assert()`, `debug_assert()`,
 * `pre()`, `debug_pre()`, `post()`, `debug_post()`,
 * `invariant()`, `debug_invariant()`.

See also the [assertions](assertions.md) document.


## Catch clause

Control-expressions can have zero or more `catch` clauses. The `catch` clauses
are used to catch *errors* and *fatal-errors* that are thrown in the
condition-expression of a control-expression. A `catch` clause must directly
follow the clause with an condition-expression that can throw an *error*.

The following control-expressions can have a `catch` clause:
 - `if` control-expression
 - `while` control-expressions
 - `for` control-expressions
 - `switch` control-expression
 - `try` control-expression

The `try` control-expression does not have a condition-expression, instead the
`catch` clauses may be used to catch errors thrown or trapped from the code
block.

When a `if` control-expression has no `elif` clauses, the `catch` clause may
appear after the `else` clause. This `catch` clause will still only catch errors
from the `if` condition-expression.

A `catch` clause has a list of error-names to catch. An error-name without a
suffix only catches *errors*, An error-name suffixed with an exclamation mark
`!` only catches *fatal-errors*. You can also use the glob `*` for any *error*
and exclame-glob `*!` for any *fatal-error*.

```
try {
    foo()
} catch (out_of_bound_error, division_by_zero_error) {
    throw my_error
} catch (bad_alloc_error, bad_alloc_error!) {
    throw!
} catch (*, *!) {
    throw
}
```

Inside the `catch` clause the special error-code variable `$?` will be set
to the `std.error_code` value of the caught *error* or *fatal-error*. The `$?`
variable is not available outside of the `catch` clause.

> [!NOTE]
> bash/sh uses the same `$?` variable for exit/error codes.

You may exit a `catch` block by:
 - using `throw` to rethrow the *error* or *fatal-error*.
 - using `throw!` to rethrow as a *fatal-error*.
 - any other way (including a normal `throw`) to exit a block.

a `try { return foo() } catch { throw }` statement is compatible with tail-call
optimization.


### try / catch expressions

Errors can also be handled within an expression. The `try` prefix
operators is used to directly rethrow the *error*.

```
var a = try foo() // Any error is rethrown.
```

The short-circuiting binary-operator `catch` evaluates the left operand and when
it throws an *error*, it will evaluate and return the right operand. The `catch`
operator can be used to catch an error and return a default value.

```
var b = foo() catch 5 // Any error causes the results to be 5.
```

> [!NOTE]
> The `try` and `catch` operators have a slightly lower precedence (numerically
> higher) than the function-call operator. This means that the `try` operators
> will be applied directly to the result of the function call.

> [!NOTE]
> A `return try foo()` statement is compatible with tail-call optimization.

> [!NOTE]
> These `try` and `catch` expressions do not catch *fatal-errors*.
