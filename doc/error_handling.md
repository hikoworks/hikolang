# Error handling

## Extendable error codes
Because types in this language are values, applications and libraries will be able
to extend types like enums by adding new values to them. This is useful for error codes, where
the application or library can add new error codes to the enum type. This is done by
using the `enum:add()` function on the enum type.

```
var error_code = enum {
    ok = 0,
    assertion_failure = 1,
    precondition_failure = 2,
    postcondition_failure = 3,
    invariant_invalid = 4,
};

// Add a new error code to the enum type, if it already exists, it will be ignored.
error_code.try_add("my_error");
```

## Throwing an error.
The `throw` statement is used to throw an error; its argument is an expression which
results in an error code value.

Example:

```
func foo(a: int) -> int {
    if a < 0 {
        throw bound_error;
    }
    return a;
}
```

If you are handling an error, you can use the `throw` statement without an argument to
rethrow the error. When rethrowing an error the program will maintain a stack trace.

## Assertions
The `assert` statement is used to check if an expression is true. If the expression is false,
then the function will throw an `assertion_failure` error.

Preconditions and postcoditions are checked in the caller function, so when a condition fails,
the caller function will throw a `precondition_failure` or `postcondition_failure` error. The
line where the error occured will be the line that called the function.

An invariant check is done just before the post condition check for a member function that mutates
the object. If the invariant check fails, the member function will throw an `invariant_invalid` error.
The line where the error occured will be the line that called the member function.


## Error handling

### Access to error codes


### if-statement init-expression / boolean-expression
The error can be directly handled by an `if` statement. In the examples below
the function `foo()` returns a value and must be used with an init-expression.
The function `bar()` does not return a value and must be used with a boolean-expression.
In both cases the error code is stored in a special variable `$error`.

```
if (var x = foo()) {
    // x is a truthy value
} else {
    // x is a falsy value
} catch (bound_error) {
    std::print("This was a bound error");
} catch {
    std::print("error: ", $error);
    throw;
}

if (bar()) {
    // bar was successful, but did not return a value.
} catch (bound_error) {
    std::print("This was a bound error");
    // Rethrow the error
    throw;
} // No default catch block, the error was unhandled and will trap.

if (bar()) {
    // bar was successful, but did not return a value.
} catch {
    std::print("error: ", $error);
}

if (baz()) {
    // baz was truthy.
} // No default catch block, it is a reportable error if baz can throws.
```

### try-catch / trap-catch statement
The `try-catch` statement is used to handle errors possibly thrown by multiple functions.
The `try` block will execute the code and if an error is thrown, the `catch` block will
catch the error. Any error not catched will be rethrown.

```
try {
    var x = foo();
    bar();
} catch (bound_error) {
    std::print("This was a bound error");
} // No default catch block, other errors will be rethrown.

trap {
    var x = foo();
    bar();
} catch (bound_error) {
    std::print("This was a bound error");
    // Rethrow the error
    throw;
} // No default catch block, other errors will be unhandled and will trap.
```


### try expression

```
var x = foo(); // Reportable error if foo() may throw.
var y = trap foo(); // foo() may throw, any error is unhandled, and will trap.
var z = try foo(); // foo() may throw, any error is rethrown.

if (try foo()) { // foo() may throw, any error is rethrown.
    // bar was successful and truthy.
}
```


## ABI
On function return an error is signaled by setting an appropriate CPU flag. On x86 this is done
by setting the `CF` flag. The caller can check the flag and if it is set with a conditional jump or
a conditional move.

On error the `RAX` register will be set to the error code. The `RDX` register will be set to instruction
pointer of the `throw` statement, possibly using `LEA RDX, [RIP + 0]` instruction.

When a `throw` statement is executed while inside the error-handling-block RAX:RDX are appended to the
error-chain stack. At the end of the error-handling-block the error-chain stack is cleared when the
error is not rethrown. The error-chain is stored in a thread-local variable.

The error-chain stack is a stack of error codes and and addresses. This error chain is used to produce
a more complete stack trace, up to the point where the error was thrown.

An unhandled error will first add the error-code/address to the error-chain stack, and then
execute an appropiate cpu-trap to call the error handler. The trap setup will be done by the
run-time services library. An error handler can determine if it was caused by an unhandled error
by checking the error-chain stack.

