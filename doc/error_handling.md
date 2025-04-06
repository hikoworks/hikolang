# Error handling

## Extendable error codes
Because types in this language are values, applications and libraries will be
able to extend types like enums by adding new values to them. This is useful for
error codes, where the application or library can add new error codes to the
enum type. This is done by using the `enum:add()` function on the enum type.

```
var error_code = enum {
    assertion_failure,
    precondition_failure,
    postcondition_failure,
    invariant_invalid,
};

// Add a new error code to the enum type, if it already exists, it will be ignored.
try {
    error_code.add("my_error");
} catch {}
```

## Throwing an error.
The `throw` expression is used to throw an error; its argument is an expression
which results in an error code value.

Example:

```
func foo(a: int) -> int {
    if a < 0 {
        throw bound_error;
    }
    return a;
}
```

If you are handling an error, you can use the `throw` statement without an
argument to rethrow the error. When rethrowing an error the program will
maintain a stack trace.

## Assertions
The `assert` statement is used to check if an expression is true. If the
expression is false, then the function will throw an `assertion_failure` error.

Preconditions and postcoditions are checked in the caller function, so when a
condition fails, the caller function will throw a `precondition_failure` or
`postcondition_failure` error. The line where the error occured will be the line
that called the function.

An invariant check is done just before the post condition check for a member
function that mutates the object. If the invariant check fails, the member
function will throw an `invariant_invalid` error. The line where the error
occured will be the line that called the member function.


## Error handling

### Access to error codes
Inside a catch block the current error code, and error address can be accessed
using the `$error` and `$error_address` variables.

The error chain stack can be accessed using the following functions:
 - `std.get_error_chain() -> std.error_chain` returns the error chain stack.
 - `std.error_chain:__index__(index: int) -> (error_code, error_address)`
   returns the error code and address at the given index.
 - `std.error_chain:size() -> int` returns the size of the error chain stack.
 - `format_error(e: std.error_code, a: std.error_address) -> std::string` format
    the full stack trace, error-chain and current error code and address.
 - `print_error(e: std.error_code, a: std.error_address)` prints the full stack
    trace, error-chain and current error code and address.


### if-catch / do-while-catch statement
The error can be directly handled by an `if`, `while` statement, by
using the `catch` clause. The `catch` clause will catch any error thrown by the
expression in the `if` or `while` statement.

Errors thrown from inside of the `if`, `elif`, `else`, `while` or `do` block are
not caught by the `catch` clause. The `catch` clause only catches errors thrown
in the expression of the `if`, `elif`, `while` clauses.

If the expression results in an error, the `catch` clause will be executed. If
none of the `catch` clauses are executed, the error is trapped.

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

while (bar()) {
    // bar() result is true.
} catch (bound_error) {
    std::print("This was a bound error");
    throw;
} // No default catch block, the error will trap.

if (bar()) {
    // bar() result is true
} catch {
    std::print("error: ", $error);
}

if (bar()) {
    // bar() result is true
} // No catch block, it is a reportable error if bar() may throw.
```

### try-catch / trap-catch statement
The `try-catch` statement is used to handle errors possibly thrown by multiple
functions. The `try` block will execute the code and if an error is thrown, the
`catch` block will catch the error. Any error not catched will be rethrown.

```
try {
    var x = foo();
    bar();
} catch (bound_error) {
    std::print("This was a bound error");
} // No default catch block, other errors will be rethrown.
```

A `trap-catch` statement is simular to `try-catch`, except when an error is not
catched, the error will cause a trap.
```
trap {
    var x = foo();
    bar();
} catch (bound_error) {
    std::print("This was a bound error");
    throw; // Rethrow the error
} // No default catch block, other errors will be unhandled and will trap.
```

A `try { return foo(); }` statement is compatible with tail-call optimization.

### try / trap / catch expression
Errors can also be handled within an expression. The `try` and `trap` prefix
operators are used to rethrow, or trap an error.

The binary `catch` operator evaluates the left operand and if an error is
thrown, it will evaluate the right operand. The `catch` operator can be used to
catch an error and return a default value.

The `try`, `trap` and `catch` operators have a slightly lower precedence
(numerically higher) than the function-call operator. This means that the `try`
and `trap` operators will be applied directly to the result of the function
call.

```
var x = foo(); // Reportable error if foo() may throw.
var y = trap foo(); // Any error is unhandled, and will trap.
var z = try foo(); // Any error is rethrown.
var w = foo() catch 5; // An error results in 5.

if (try foo()) { // foo() may throw, any error is rethrown.
    // bar was successful and truthy.
}
```

A `return try foo();` statement is compatible with tail-call optimization.

You can use `trap throw my_error;` to force a trap in a function.

## ABI
On function return an error is signaled by setting an appropriate CPU flag. On
x86 this is done by setting the `CF` flag. The caller can check the flag and if
it is set with a conditional jump or a conditional move.

On error the lower 32 bits of the `RAX` register will be set to the error code.
Bit 32 of the `RAX` register will be set to 1 if the error is rethrown, this is
used as a performance optimization to avoid resetting the error-chain.

The `RDX` register will be set to instruction pointer of the `throw` statement,
possibly using `LEA RDX, [RIP + 0]` instruction.

When a `throw` statement is executed while inside the error-handling-block
RAX:RDX are appended to the error-chain stack. At the end of the
error-handling-block the error-chain stack is cleared, this only needs to
be done for rethrown errors as otherwise the error-chain would already by empty.

The error-chain stack is a stack of error codes and and addresses. This error
chain is used to produce a more complete stack trace, up to the point where the
error was thrown.

An trapped error will first add the error-code/address to the error-chain stack,
and then execute an appropiate trap-instruction to call the error handler. The
run-time services library should setup the trap system to call an trap handler.
The trap handler can determine if it was caused by an error by checking the
error-chain stack.

```

push_error_chain:
            mov rcx, 1
            xadd byte gs:[error_chain_head], rcx
            cmp rcx, 0xff
            je .push_error_chain_overflow
            sll rcx, 4
            lea rcx, [error_chain_data + rcx]
            mov gs:[rcx], rax
            mov gs:[rcx + 8], rdx
            ret
push_error_chain_overflow:
            ; The trap handler will detect error_chain_head has overflowed.
            ; There was still room for one more entry, so we can find where
            ; the overflow happened.
            int 3
            
foo:
            lea rdx, [rip + 0]
            mov rax, unbound_error_value
            stc
            ret

foo_caller:
            call foo()
            jc .failure

success:
            ...
            clc
            ret

failure:
            cmp eax, unbound_error_value ; Only check the bottom 32 bits of the error code.
            je .bound_error

            cmp eax, assertion_failure_value ; Only check the bottom 32 bits of the error code.
            je .assert_error

            ; Ignore any other error. Clear the error chain.
            bt rax, 32
            cmovc byte gs:[error_chain_head], 0
            jmp .success

bound_error:
            ; rethrow
            call push_error_chain

            lea rdx, [rip + 0]
            ; Optionally set rax to a new error code.
            bts rax, 32 ; Indicate that the error is rethrown
            stc
            ret

assert_error:
            ; trap
            call push_error_chain
            int 3
```
