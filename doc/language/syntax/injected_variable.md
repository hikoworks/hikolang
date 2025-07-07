# injected-variable

## Syntax

`$` [_name_](name.md)

## Semantics
Injected variables are used to easilly inject dependencies between functions.
Functions will automatically pass injected variables into function-calls recursively.

## Example

In the following example `main()` will inject an instance of `logger` into
`foo()` with the name `$log`. Notice that neither `foo()` or `bar()` have `$log`
in the argument declaration. `$log` is automatically passed to `bar()` simply
because `bar()` is using `$log` in its body.
```
function bar(x)
{
    $log("bar called with {}" % x)
    return x + 1
}

function foo(x : int)
{
    return bar(x) + 1
}

function main() -> int
{
    var log = logger("logfile.txt")
    log("Hello world")

    return foo(5, $log=log)
}
```
