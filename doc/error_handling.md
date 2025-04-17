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
The `throw` statements is used to throw an error. The argument of the `throw`
statement is the error-name, and optionally a string literal with a message.

A `throw` statement is meant to be fast to throw and catch, this is why the
error message may only be a string literal.

```
if (a < 0) {
    throw out_of_bounds_error, "a should be larger or equal to 0"
}
```

By throwing, trapping or catching; an error-name is implicitly declared.
The error name is an identifier that exists in the global namespace and is
only available as arguments of `throw`, `trap` and `catch` statements.
The number of unique error-names is limited to 2^31.

Errors thrown by the `throw` statement must be directly caught by the caller,
explicitly by error-name or `catch` statement without an argument. The compiler
will track which errors can be thrown by a function. It is a static error if a
function may throw an error that is not caught by the caller.

A function prototype can be declared with a list of errors it can throw. The
compiler will make sure that the function will only throw errors that is
on this list. This should be used when passing a function-pointer to a
function.

```
var my_func_type = func (a: int) throws(bound_error, underflow_error) -> int
```

A destructor is not allowed to throw an error.

## Trapping an error
The `trap` statement is used to trap an error. The argument of the `trap`
statement is the error-name, and optionally a string literal, or a dynamically
generated message.

Because the `trap` statement is used to terminate the program, it is not
designed to be fast. There is some cost to setup and teardown a `catch trap`
clause, and to `trap` the error itself. Therefor it is recommended to
dynamically generate a detailed error message and pass it to the `trap`
statement.

```
if (a < 0) {
    trap out_of_bounds_error, std.format("a should be larger or equal to 0, a = {}", a)
}
```

By throwing, trapping or catching; an error-name is implicitly declared.
The error name is an identifier that exists in the global namespace and is
only available as arguments of `throw`, `trap` and `catch` statements.
The number of unique error-names is limited to 2^31.

Errors trapped by the `trap` statement will normally terminate the program with
and error message. This is done by calling the appropriate CPU trap instruction.
A debugger attached to the application should point directly to the trap
statement.

It is possible to catch a trap error by using a `catch trap` clause. This is
used by unit-tests, or by applications that are able to isolate programming bugs
(by for example terminating the connection to a client).

A trap may be retrapped, but may not be rethrown. If the `throw` statement is
used inside the `catch trap` block, a new error will be thrown, it is not a
continuation of the previous error.

A second trap caused while unwinding the stack to catch the first trap can not
be caught and will cause the application to terminate with a CPU trap.

## Assertions
The following assert and contract statements trap with `assertion_failure`:
`assert()`, `debug_assert()`, `pre()`, `debug_pre()`, `post()`, `debug_post()`,
`invariant()`, `debug_invariant()`.

See also the [assertions](assertions.md) document.

## Catch clause
Control-expressions can have zero or more `catch` clauses. The `catch` clauses
are used to catch errors that are thrown or trapped in the condition-expression
of a control-expression. A `catch` clause must directly follow the clause with
an condition-expression that can throw an error.

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

The `catch` clause comes in four different forms:
 - `catch (<error_names>)`: Catch a thrown or trapped error by name
 - `catch`: Catch any thrown error (does not catch trapped errors)
 - `catch trap (<error_names>)`: Catch a trapped error by name
 - `catch trap`: Catch any trapped error

You may exit a `catch` block by:
 - using `throw <error_name>` to throw a new error. The original error is
   discarded. The new error is not a continuation of the original error.
 - using `throw` to rethrow the error. This is a continuation of the original
   error, it has the original error-code, message and location. You can't
   rethrow a trapped error.
 - using `trap <error_name>` to trap a new the error. The original error is
   discarded. The new error is not a continuation of the original error.
 - using `trap` to retrap an error. This is a continuation of the
   original error, it has the original error-code, message and location.
   You may retrap either a thrown or a trapped error.
 - using a `return` statement to return from the function. This will
   cause the function to return successfully with a value.
 - using `break`, `continue`, `goto` or `result` statement to exit the block
   of code.
 - executing to the end of the block.

a `try { return foo() } catch { throw }` statement is compatible with tail-call
optimization.

### try / trap / catch expressions
Errors can also be handled within an expression. The `try` and `trap` prefix
operators are used to rethrow, or trap an error.

The short-circuiting binary-operator `catch` evaluates the left operand and when
it throws an error, it will evaluate and return the right operand. The `catch`
operator can be used to catch an error and return a default value.

The `try`, `trap` and `catch` operators have a slightly lower precedence
(numerically higher) than the function-call operator. This means that the `try`
and `trap` operators will be applied directly to the result of the function
call.

```
var y = trap foo() // Any error is unhandled, and will trap.
var z = try foo() // Any error is rethrown.
var w = foo() catch 5 // Any error causes the results to be 5.
```

A `return try foo()` statement is compatible with tail-call optimization.

## ABI x86-64
On function return an error is signaled by setting an appropriate CPU flag. On
x86 this is done by setting the `CF` flag. The caller can check the flag and if
it is set with a conditional jump or a conditional move.

The return registers are used as follows:
 - RAX[30:0] = error code.
 - RDX[31] = `1` if the error is a trap, `0` if it is a throw.
 - RDX[30:0] = throw location id.

The error code is a 31 bit unsigned integer, the value zero is not used for
an error.

A global static table `__error_throw_location__` is used to associative map the
throw location id with, item struct:
 - `__ptr__`: A pointer to the string with the error message.
 - `__ptr__`: A pointer to the string with the filename.
 - `__u32__`: The line number in the file (1-based).
 - `__u32__`: zero

When trapping an error the thread-local `__trap_mask_ptr__` pointer is checked.
It points to a struct with the following fields. This struct is allocated on the
stack frame of the function that tries to catch a trap:
 - `__ptr__`: A pointer to the next struct in the chain.
              Bit 0 is set if all traps are caught.
 - `__u32__[n]`: A specific trap error code that will be caught.
 - `__u32__`: Terminating zero.

When trapping an error the trap statement may set a dynamically generated
error message. This is done by setting the thread-local `__trap_error_message__`
string to an error message, this string is cleared when the trap is caught and
not retrapped. 


### Throwing an error
Throwing an error outside of a catch block.
Because this is supposed to be fast, the error code and location id is passed
in registers.

```
            ; End lifetime of local variables.
            mov edx, __error_line__this_location     ; This is the throw location id / throw.
            mov eax, __throw_code__bounds_error      ; Error code.
            stc                                      ; Flag as error.
            ret
```

### Trapping an error
Trapping an error outside of a catch block. Since a trap is supposed to
terminate the application, the application should take its time to create a
detailed error message and copy it into the `__error_message__` string.


There is a globaly available __check_trap_mask__ function, it walks the
trap-mask linked list starting at __trap_mask_ptr__ and checks if the current
error should be caught. The caller of __check_trap_mask__ uses the CF flag to
determine if the error should be passed to the caller or cause a CPU trap. 
```
            ; This function must be called just before returning, pass the
            ; trap-error to the caller, or to trap the CPU.
            ; This function will keep RDX and RAX intact, but it will modify
            ; RCX, R8 and R9.
.__check_trap_mask__:
            mov rcx, gs:[__trap_mask_ptr__]
.__check_trap_mask__l0:
            test rcx, rcx
            jne .__check_trap_mask__f2
            ret                                     ; No more masks to check. CF = 0
.__check_trap_mask__f2:
            mov r8, [rcx]
            btr r8, 0
            jc .__check_trap_mask__f3               ; Found wildcard catch.
            add rcx, 12
.__check_trap_mask__l1:
            add rcx, 4
            mov dword r9, [rcx]
            cmp r9, eax
            je .__check_trap_mask__f3               ; Found specific catch.
            test r9, r9
            jne .__check_trap_mask__l1              ; Check next specific.
            mov rcx, r8
            jmp .__check_trap_mask__l0              ; Check next mask.

.__check_trap_mask__f3:                             ; Trap is caught. CF=1
            stc
            ret
```

When a trap is executed these are the sequence of instructions.
```
            ; Set __trap_message__ to the error message.
            ; End lifetime of local variables.
            mov edx, __error_location                ; Location id for the trap statement.
            mov eax, __trap_code__assertion_failure  ; Error code. bit 31 is set to 1.
            call __check_trap_mask__
            jnc skip_1
            ret                                      ; CF=1 pass the trap to the caller.
.skip_1:    mov gs:[__trap_error_code__], eax        ; Set the error code.
            mov gs:[__trap_location_id__], edx       ; Set the location id.
            int 3                                    ; CPU trap.
```

### Catching an error
Catching a thrown error:
```
; try {
;     a = foo()
            call foo()
            jc .handle_error                         ; If the CF flag is set, ignore the error.
            mov [a], rax                             ; Store the result in a.
finally:
            xor eax, eax                             ; Clear the CF flag.
            ret

handle_error:
            cmp eax, __error_code__parse_error
            je .handle_parse_error
            cmp eax, __error_code__permission_denied
            je .handle_permission_denied
            cmp eax, __error_code__file_not_found
            je .handle_file_not_found
            cmp eax, __error_code__color_not_found
            je .handle_color_not_found
            stc                                      ; At this point it must be trap, automatically retrap.
            ret

.handle_parse_error:
; } catch (parse_error) {
;     a = 42
            mov [a], 42                              ; Store the result in a.
            jmp finally                              ; Jump to the finally block.

.handle_index_error:
; } catch (permission_denied) {
;    throw
            stc
            ret

.handle_file_not_found:
; } catch (file_not_found) {
;    throw cannot_load_theme_error, "Cannot load the theme."
            mov edx, __error_line__handle_index_error
            mov eax, __error_code__cannot_load_theme_error
            stc
            ret

.handle_color_not_found:
; } catch (color_not_found) {
;    trap
            bts eax, 31                              ; Set the trap bit.
            call __check_trap_mask__
            jnc skip_1
            ret                                      ; CF=1 pass the trap to the caller.
.skip_1:    mov gs:[__trap_error_code__], eax        ; Set the error code.
            mov gs:[__trap_location_id__], edx       ; Set the location id.
            int 3                                    ; CPU trap.     
; }
```

### Catching a trapped error:
```
; try {
;     a = foo()
            mov qword [sp + trap_mask_ptr], gs:[__trap_mask_ptr__] ; Save the trap mask pointer.
            bts qword [sp + trap_mask_ptr], 0 ; Set the wildcard catch bit.
            mov gs:[__trap_mask_ptr], sp + trap_mask_ptr ; Set the trap mask pointer to the new mask.
            mov dword [sp + trap_mask_ptr + 8], __trap_code__assertion_failure
            mov dword [sp + trap_mask_ptr + 4], 0

            call foo()
            jc .handle_error                         ; If the CF flag is set, ignore the error.
            mov gs:[__trap_mask_ptr], [sp + trap_mask_ptr] ; Restore the trap mask pointer.
            mov [a], rax                             ; Store the result in a.
finally:
            xor eax, eax                             ; Clear the CF flag.
            ret

handle_error:
            mov gs:[__trap_mask_ptr], [sp + trap_mask_ptr] ; Restore the trap mask pointer.

            cmp eax, __trap_code__assertion_failure
            je .handle_assertion_failure
            jmp .handle_any_trap

.handle_assertion_failure
; } catch trap (assertion_failure) {
;     a = 42
            mov eax, __trap_error_message__
            call std__string__clear
            mov [a], 42                              ; Store the result in a.
            jmp finally                              ; Jump to the finally block.

.handle_any_trap:
; } catch trap {
;     throw caught_a_trap_error, "oops"
            mov eax, __trap_error_message__
            call std__string__clear
            mov eax, __error_code__caught_a_trap_error
            mov edx, __error_line__handle_any_trap
            stc
            ret
; }
