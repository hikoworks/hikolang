# context-argument

## Syntax

_context_argument_ := `$` [_identifier_]

[_identifier_]: identifier.md

## Semantics
Context arguments are reference variables which are implied and hidden arguments of a function.
If a variable 

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
