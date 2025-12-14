# LLVM-directive

## Syntax

`llvm`　`{` [_bracketed-text_](bracketed_text.md) `}`

## Semantics
The `llvm` block is used to include LLVM assembly code within the source code.
It allows developers to write low-level LLVM instructions directly in their code.

This is used to implement the standard library and allow other developers to write
low-level code.

The [_bracketed-text_](bracketed_text.md) contains the LLVM assembly code, which
is passed completely as-is to the LLVM compiler. The code is limitedly parsed to
extract special variables that are used to access variables in the surrounding
code.

Special variables are available in the LLVM code:
 - `%` _name_ `.arg` : A variable that is passed as an argument to the LLVM
   code.
 - `%` _name_ `.var` : A variable in the surrounding code.
 - `%` _name_ `.arg.addr` : An address of a variable that is passed as an
   argument to the LLVM code.
 - `%` _name_ `.var.addr` : An address of a variable in the surrounding code.

> [!NOTE]
> LLVM code may be executed during constant evaluation.

## Example

```
function foo(a : __i32__, b : __i32__) -> __i32__ {
    var c = a &+ b
    var r = 0 : __i32__

#llvm
    %tmp = mul i32 %c.var, %a.arg
    store i32 %tmp, i32* %r.var.addr
#endllvm
    return r
}
```

In this example, the `#llvm` directive is used to include LLVM assembly code
that multiplies the variable `c` with the argument `a` and stores the result in
the variable `r`. The special variables `%c.var` and `%a.arg` are used to access
the values of the surrounding code.

