# Error handling

For ease of use, errors are thrown and caught, but they are not like traditional
exceptions, within this language we call them errors. Errors are returned from
a function.

A caller is responsible for catching all errors that can be thrown by an
expression. The compiler will check if all errors that can be thrown are caught.

Therefore all control flow is local, making it easier to reason about the code.

Features:
 - Errors are returned from a function using the `throw` keyword.
 - Returning `empty` uses the same mechanics as errors.
 - Hardware traps are converted to errors.
 - Errors must be handled directly by the caller.
 - New errors can be added using `syntax error` statements.
 - Very fast throw and catch:
   - No setup or teardown on catch or call.
   - Throwing is done by setting the return registers to an error code.
   - Catching specific errors is done by comparing the error code returned
     in a register by known constant values.
 - errors can be defined as `auto_rethrow`:
   - Compiler automatically synthesises rethrow handlers.
   - An `auto_rethrow` error aborts the application when it is not explicitly
     caught.

## Adding error codes

```
.std.error_code = enum {
    out_of_bounds
    @auto_rethrow out_of_memory
}
```

## Throwing an error

The `throw` statements is used to throw an *error*. The argument of the `throw`
statement is the error-name, and optionally a string literal with a message.
A thrown *error* must be directly caught by the calling function.

```
if (a < 0) {
    throw out_of_bounds, "a should be larger or equal to 0"
}
```

The compiler tracks which *errors* may be thrown by a function, and the compiler
will give an error message if not caught by the caller. Rethrown errors are
tracked as well. 

> [!caution]
> You can't throw an *error* from a destructor. 

The message is part of the table with throw-id as key, so that this information
does not need to be constructed during the throw. This improves perfomance of
throwing errors.

## Returning no-value

If a function does not have an explicit return type and every `return`
statement is a no-value `return` statement, then the return type of
the function is inferred as `void`.

If a function has the `void` return type, then
a no-value `return` statement will return a `void` value.

If a function has a non-`void` return type, then
a no-value `return` statement becomes syntactic sugar for `throw empty`.

> [!NOTE]
> `void` is a compiler-provided type with a single zero-bit value.
> Values of `void` may be discarded.

A `empty` clause on a flow control expression is syntactic sugar for a
`catch(empty)` clause.


## Assertions

The following assert and contract statements `throw assertion` and `throw
contract`:
 * `assert()`, `debug_assert()`,
 * `pre()`, `debug_pre()`, `post()`, `debug_post()`,
 * `invariant()`, `debug_invariant()`.

See also the [assertions](assertions.md) document.


## Traps

Recoverable hardware traps like divide-by-zero are converted to errors.

On x86 Linux this could be done by having function that can trap having the
first slot on the stack frame set to the jump-address to instructions that
unwinds the function and return the error. The signal handler would look at the
stack-frame and return to that address and set the register with the proper
error-code.


## Catch clause

Control-expressions can have zero or more `catch` clauses. The `catch` clauses
are used to catch *errors* that are thrown in the
condition-expression of a control-expression. A `catch` clause must directly
follow the clause with a condition-expression that can throw an *error*.


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

A `catch` clause has a list of error-names to catch.

```
if (a = foo()) {
    return a
} empty {
    print("no value")
} catch (out_of_bounds, division_by_zero) {
    throw my_error
} catch {
    throw
}
```

Within a `catch` block you can use the `std.errortrace()` function to query
the original throw and each subsequent rethrow that caused this error.
The `std.errortrace()` is not available outside of a `catch` block.

a `try { return foo() } catch { throw }` statement is compatible with tail-call
optimization.


### try / catch expressions

Errors can also be handled within an expression. The `try` prefix
operators is used to directly rethrow the *error*.

```
a = try foo() // Any error is rethrown.
```

The short-circuiting binary-operator `catch` evaluates the left operand and when
it throws an *error*, it will evaluate and return the right operand. The `catch`
operator can be used to catch an error and return a default value.

```
b = foo() catch 5 // Any error, including empty causes the results to be 5.
c = foo() catch(out_of_bounds) 42
d = foo() empty 69
```

> [!NOTE]
> The `try` and `catch` operators have a slightly lower precedence (numerically
> higher) than the function-call operator. This means that the `try` operators
> will be applied directly to the result of the function call.

> [!NOTE]
> The `catch(...)` operator is normally difficult to parse, but all errors
> are added as context sensitive keywords. 

> [!NOTE]
> A `return try foo()` statement is compatible with tail-call optimization.


## ABI x86

If a function is guaranteed to return a value and cannot throw an error, it uses
the normal x86-64 ABI. This means ordinary functions incur no additional cost
from the error-handling mechanism.

If a function can return an error, it uses the error-aware ABI. The result is
communicated primarily through `RAX` and `RDX`. `RDX[63]` acts as a
discriminator: a `1` bit indicates a successful value, while a `0` bit indicates
an error.

On success, values up to 127 bits can be returned directly in
`RDX[62:0]:RAX[63:0]`. Larger values are returned indirectly through a pointer
passed as the first argument.

On error, the remaining bits of `RDX` contain the error metadata:
 - `RDX[62:32]`: The error code identifies the kind of error,
 - `RDX[31:8]`:  the throw ID identifies the source throw site,
 - `RDX[7:0]`:   and the throw depth records how many rethrows have contributed
                 to the error trace.


### Initial throw

An initial throw only needs to construct the error metadata in `RDX` and return.
In particular, it does not need to touch the per-thread trace table. This keeps
the common case of throwing an error as cheap as possible. The initial throw
always starts with a throw depth of zero:

```asm
        mov rdx, <error-code, throw-id, throw-depth=0>
        ret
```

The absence of a trace-table write here is intentional. The trace table is only
populated when an error is rethrown.


### Catch

A caller first invokes the function normally and then checks the value flag in
`RDX`. Because the flag occupies the sign bit, `test rdx, rdx` followed by `jns`
provides a compact way to distinguish errors from successful values.

For an error, the error code occupies `RDX[62:32]`. Since the value flag is
known to be zero in this case, shifting the value right by 32 bits extracts the
error code without requiring a mask. The handler can then dispatch on the error
code:

```asm
        call foo
        test rdx, rdx
        jns catch_handlers          ; value-flag == '0'
        ...

catch_handlers:
        mov rax, rdx
        shr rax, 32                 ; extract error-code
        jz empty_handler
        cmp eax, out_of_bounds
        je out_or_bound_handler
        ud2                         ; unreachable

out_of_bounds_handler:
        ...

empty_handler:
        ...
```

This makes catching an error very cheap: the caller only needs to test the
result and, if necessary, inspect the error code.


### Rethrow

A rethrow records the current throw ID in the per-thread trace table before
replacing the throw metadata with the rethrowing throw site.

The current depth selects the trace-table entry. After storing the entry, the
depth is incremented with saturation at 255. This means the metadata carried by
the error itself determines how much of its rethrow history is relevant; there
is no need to reset or clear the trace table when an error is handled.

The old error code is preserved during an ordinary rethrow, while the throw ID
is replaced:

```asm
        mov fs:[throw_table + dl * 4], edx  ; entry 255 may get clobbered
        add dl, 1
        sbb dl, 0                           ; saturate to 255

        mov rax, 0xffffffff000000ff
        and rdx, rax
        or rdx, <throw_id, throw-depth=0>
```

The mask clears the old throw ID and preserves the error code and current depth.
The final `or` inserts the new throw ID and resets the throw depth.


### Rethrow with new error

When a catch handler converts one error into a different error, the old error
metadata is discarded after its trace entry has been recorded.

Only the newly accumulated depth needs to survive from the old error. Clearing
`RDX` down to its low byte achieves this directly, after which the new error
code and throw ID can be inserted as a complete error value.

```asm
        mov fs:[trace_table + dl * 4], edx  ; entry 255 may get clobbered
        add dl, 1
        sbb dl, 0                           ; saturate to 255

        and edx, 0xff                       ; clear top 56 bits
        mov rax, <error-code, throw-id, throw-depth=0>
        or rdx, rax
```

This also means that creating a new error does not require individually clearing
the old error code and throw ID. The `and edx, 0xff` operation clears the upper
56 bits in one instruction while retaining the accumulated depth.
