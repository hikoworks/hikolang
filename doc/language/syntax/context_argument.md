# context-argument

[_identifier_]: identifier.md


## Syntax

_context_argument_ := `$` [_identifier_]


## Semantics

A context argument is a reference variable that is implicitly supplied to a
function without appearing in its explicit argument declaration. A context
argument is identified by a `$`-prefixed identifier, such as `$log`.

When a function references a context argument, that context argument becomes
an implicit dependency of the function. When the function calls another
function that references the same context argument, the context argument is
implicitly forwarded to that call. This propagation continues through the call
chain until the context argument is explicitly provided by the caller.

The context argument is therefore resolved by name rather than by position. A
caller may satisfy the dependency by explicitly providing a value using the
`$identifier=value` syntax. If no value is available when a context argument is
required, it is an error.

Context arguments are distinct from ordinary function arguments: they do not
form part of the function's explicit calling convention and do not need to be
declared by the function or by intermediate functions that merely forward them.


## Example

In the following example `main()` will inject an instance of `logger` into
`foo()` with the name `$log`. Notice that neither `foo()` or `bar()` have `$log`
in the argument declaration. `$log` is automatically passed to `bar()` simply
because `bar()` is using `$log` in its body.

```
bar = fn(x)
{
    $log("bar called with {}" % x)
    return x + 1
}

foo = fn(x : int)
{
    return bar(x) + 1
}

main = fn() -> int
{
    log = logger("logfile.txt")
    log("Hello world")

    return foo(5, $log=log)
}
```
