# Error handling
For ease of use, errors are thrown and caught, but they are not exceptions,
within this language we call them errors. 

A caller is responsible for catching all errors that can be thrown by an
expression. The compiler will check if all errors that can be thrown are caught.

Therefor all control flow is local, making it easier to reason about the code.

Features:
 - Although errors are thrown, they are not exceptions. 
 - No non-local control flow.
 - Compiler checks if all errors that can be thrown are caught directly by
   the caller.
 - Declaring a new error is easy; by simply throwing, trapping or catching it.
 - Very fast throw and catch.
   - No setup or teardown on catch or call.
   - Throwing is done by setting the return registers to an error code, and
     instruction pointer and setting the carry flag.
   - Initial check for an error is by checking the carry flag.
   - Catching specific errors is done by comparing the error code returned
     in a register by known constant values.
   - Rethrowing is the same as throwing, but keeping the register with the
     instruction pointer intact.
 - Fast trapping.
   - Trapping is done by checking the trap mask and either returning the error
     like throw, or calling the trap CPU instruction.
   - If a catch is explicitly catching a trap, the trap mask is cleared for that
     error code, and the previous mask is restored after the catch block.
     Expicitly catching traps should be extremely rare.
   - Retrapping is the same as trapping, but keeping the register with the
     instruction pointer intact.


## Throwing an error.
The `throw` statements is used to throw an error. The argument
of the `throw` statement is the error-name, and optionally a
string literal with a message.

By throwing, trapping or catching an error-name is implicently declared.
The error name is an identifier that exists in the global namespace and is
only available as arguments of `throw`, `trap` and `catch` statements.
The number of unique error-names is limited to 2^31.

Errors thrown by the `throw` statement must be directly caught by the
caller, explicitly by error-name or `catch` statement without an argument.
By requiring directly catching errors, the language reduces non-local control flow.

Example of a `throw` statement:
```
func foo(a: int) -> int {
    if a < 0 {
        throw out_of_bounds_error
    }
    return a
}
```

The compiler will track which errors can be thrown by a function.
It is a static error if a function may throw an error that is not caught by
the caller.

A function protype can be declared with a list of errors it can throw. The
compiler will make sure that the function will not try to throw an error that is
not in this list. This should be used when passing a function-pointer to a
function.

```
var my_func_type = func (a: int) throws(bound_error, underflow_error) -> int
```

A destructor is not allowed to throw an error.

## Trapping an error
Both the `throw` and `trap` statements are used to throw an error. The argument
of the `throw` and `trap` statement is the name of the error.

By throwing, trapping or catching an error the name of the error declared.
The error name is an identifier that exists in the global namespace and is
only available in `throw`, `trap` and `catch` statements. The number of
unique error names is limited to 2^32. Unique errors that may be used by a
`trap` statement is limited to 64.

The `throw` is for normal error handling and must be directly caught by the
caller, explicitly by name or using a catch-all. By requiring directly catching
errors, the language reduces non-local control flow.

A `trap` is used to indicate a programming error, and normally it would cause a
program to terminate with an error message and a stack trace. Traps may be
caught by any caller on the stack, explicitly by name, or `catch trap`,
but it ignores catch-all.

A `trap` allows the caller to ignore errors that are clearly programming errors,
and not recoverable. But also allow recovering from these errors in a controlled
manner, for example in a test environment or by dropping the connection to a
client.

A destructor is not allowed to throw an error, it is allowed to trap an error.
This trap can't be caught. This rule makes sure that the stack is unwound
properly in case of an error.


## Assertions
The following assert and contract statements trap with `assertion_failure`:
`assert()`, `debug_assert()`, `pre()`, `debug_pre()`, `post()`, `debug_post()`,
`invariant()`, `debug_invariant()`.

## Catch clause
Control flow statements can have a `catch` clause. The `catch` clause is used to
catch errors that are thrown in the expression of a control-flow statement. A
`catch` clause must directly follow the clause with an expression that can
throw an error.

The following control-flow statements can have a `catch` clause:
 - `if` statement
 - `while` statement
 - `for` statement
 - `switch` statement
 - `try` statement

An example of a `catch` clause in an `if` statement:
```
if (foo()) {
    // code
} catch (underflow, overflow) {
    // code
} else {
    // code
}
```

The expression of the `catch` clause is a comma `,` separated list of errors. A
thrown error matches if the error is listed in the `catch` expression. The
`catch` clause can also be empty, in which case it will catch any thrown error.
A `catch trap` clause will catch any error that was trapped by the `trap`
statement.

You can exit the `catch` block:
 - using a `throw` statement to rethrow the error possibly with a different
   error code. 
 - using a `trap` statement to trap the error. This will cause the program to
   terminate and call the error handler.
 - using a `return` statement to return from the function. This will cause the
   function to return successfully with a value.
 - executing to the end of the `catch` block. This will exit the current flow
   control statement and continue execution the current function.





## try statement
The `try` statement is used to handle errors possibly thrown by multiple
statements or expressions. The `try` block will execute the code and when an
error is thrown, an optional `catch` block may catch the error.

```
try {
    var x = foo()
    bar()
} catch (bound_error) {
    std.print("This was a bound error")
}
```

A `try { return foo() } catch { throw }` statement is compatible with tail-call optimization.


### try / trap / catch expression
Errors can also be handled within an expression. The `try` and `trap` prefix
operators are used to rethrow, or trap an error.

The binary `catch` operator evaluates the left operand and when it throws an
error, it will evaluate and return the right operand. The `catch` operator can
be used to catch an error and return a default value.

The `try`, `trap` and `catch` operators have a slightly lower precedence
(numerically higher) than the function-call operator. This means that the `try`
and `trap` operators will be applied directly to the result of the function
call.

```
var x = foo() // static error if foo() can throw.
var y = trap foo() // Any error is unhandled, and will trap.
var z = try foo() // Any error is rethrown.
var w = foo() catch 5 // Any error causes the results to be 5.
```

A `return try foo()` statement is compatible with tail-call optimization.

## ABI x86-64
On function return an error is signaled by setting an appropriate CPU flag. On
x86 this is done by setting the `CF` flag. The caller can check the flag and if
it is set with a conditional jump or a conditional move.

On the x86-64 architecture the return registers are used as follows:
 - RAX[30:0] = error code.
 - RDX[31:1] = throw location id.
 - RDX[0] = `1` if the error is a trap, `0` if it is a throw.

A table `__error_throw_location__` is used to map the throw location id with:
 - `__ptr__`: A pointer to the string with the error message.
 - `__ptr__`: A pointer to the string with the filename.
 - `__i32__`: The line number in the file (1-based).
 - `__i32__`: zero

When trapping an error the `__trap_mask_ptr__` pointer is checked   


### Throwing an error
Throwing an error outside of a catch block.
Because this is supposed to be fast, the error code and location id is passed
in registers.

```
            mov rdx, __error_line__this_location_0   ; This is the throw location id / throw.
            mov rax, __error_code__bounds_error      ; Error code.
            stc                                      ; Flag as error.
            ret
```

### Trapping an error
Trapping an error outside of a catch block.
Since a trap is supposed to terminate the application, the application should take
its time to create a detailed error message and copy it into the `__error_message__` string.

```
            ; Set __trap_message__ to the error message.
            ; Check __trap_mask__ to see if the error is in the trap mask if it is not, then execute int 3.
            mov rdx, __error_line__this_location_1   ; This is the throw location id / trap.
            mov rax, __error_code__assertion_failure ; Error code.
            stc                                      ; Flag as error.
            ret
```






Trapping an error:
```
            lea rdx, [rip + 0]                       ; Address of the throw statement.
            mov rax, __error_code__assertion_failure ; Error code, < 64.
            mov rcx, gs:[__trap_mask__]              ; Get the trap mask.
            bt rcx, rax                              ; Check if the error is in the trap mask.
            jc .trap_retrap                          ; If the error is in the trap mask, retrap.
            int 3                                    ; Trap the error.
.trap_retrap:
            ret                                      ; CF is already set.
```

Catching a thrown error:
```
            ; try {
            ;     a = foo()
            ; } catch (bounds_error) {
            ;     a = 42
            ; }
            call foo()
            jc .handle_error                         ; If the CF flag is set, ignore the error.
            mov [a], rax                             ; Store the result in a.
            ret
.handle_error:
            cmp eax, __error_code__bounds_error      ; Check if the error is a bounds error.
            je .handle_bounds_error                  ; If it is, handle it.
            stc                                      ; At this point it must be trap.
            ret
.handle_bounds_error:
            xor rax, rax                             ; Clear the CF flag.
            mov [a], 42                              ; Store the result in a.
            ret
```

Rethrow an error:
```
            ; try {
            ;     a = foo()
            ; } catch {
            ;     throw
            ; }
            call foo()
            jc .handle_error                         ; If the CF flag is set, ignore the error.
            mov [a], rax                             ; Store the result in a.
.handle_error:
            ret
```

Rethrow an tail call optimization:
```
            ; try {
            ;     return foo()
            ; } catch {
            ;     throw
            ; }
            jmp foo()
```

Retrap an error:
```
            ; try {
            ;     a = foo()
            ; } catch {
            ;     trap
            ; }
            call foo()
            jc .handle_error                         ; If the CF flag is set, ignore the error.
            mov [a], rax                             ; Store the result in a.
            ret
.handle_error:
            cmp eax, 64                              ; Check if the error is a trap.
            jb .handle_trap
            shl rax, 32                              ; Shift the error code to the upper 32 bits. Trap zero.
            mov rcx, gs:[__trap_mask__]              ; Get the trap mask.
            bt rcx, al
            jc .handle_trap
            int 3                                    ; Trap the error.
.handle_trap:
            stc
            ret
```



```
foo:        ; throw unbound_error
            lea rdx, [rip + 0]
            mov rax, __error_code__bound_error
            stc
            ret


bar:        ; assert(false)
            mov rax, __error_code__assert
            mov rdx, gs:[__trap_mask__]
            bt rdx, al
            jc .bar_retrap
            int 3
bar_retrap:
            lea rdx, [rip + 0]
            ret ; CF is already set.


foo_caller: ; if (foo() and bar()) {
            ; } catch (bounds_error) {
            ; } catch (assert_failure) {
            ; } catch {
            ;    throw overflow_error
            ; }

            ; Because assert_failure, a trap, is caught by the catch blocks.
            mov rcx, gs:[__trap_mask__]
            push rcx ; Save the trap mask.
            btr rcx, __error_code__assert ; Clear the trap mask for assert.
            mov gs:[__trap_mask__], rcx ; Set the trap mask. 

            call foo()
            jc .catch
            call bar()
            jc .catch

            ; Because assert_failure, a trap, is caught by the catch blocks.
            pop rcx ; Restore the trap mask.
            mov gs:[__trap_mask__], rcx ; Set the trap mask.

            ; success
            xor eax, eax ; Clears the CF flag.
            ret

catch:
            ; Because assert_failure, a trap, is caught by the catch blocks.
            pop rcx ; Restore the trap mask.
            mov gs:[__trap_mask__], rcx ; Set the trap mask.

            cmp eax, __error_code__bounds_error ; Only check the bottom 32 bits of the error code.
            je .catch_bounds_error

            cmp eax, __error_code__assertion_failure ; Only check the bottom 32 bits of the error code.
            je .catch_assert_failure

            cmp eax, 64
            jea .catch_default
            mov rdx, gs:[__trap_mask__]
            bt rdx, al
            jc .catch_retrap
            int 3
catch_retrap: ; CF is already set
            ret

catch_default:
            ; rethrow overflow_error, rdx still contains the address of the original error.
            mov rax, __error_code__overflow_error
            stc
            ret

catch_bound_error:
            xor rax, rax ; Clears the CF flag.
            ret

catch_assert_failure:
            xor rax, rax ; Clears the CF flag.
            ret
```
