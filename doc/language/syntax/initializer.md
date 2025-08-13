# initializer

## Syntax

__(__ `=` __|__ _inplace-operator_ __)__ [_expression_](expression.md)

### inplace-operator

Any operator that a inplace operator may be used to initialize a variable.
The following inplace operators exist by default:
`+=`, `-=`, `*=`, `/=`, `%=`, `|=`, `&=`, `^=`, `<<=`, `>>=`.

## Semantics

An initializer is used in a variable declaration to assign a initial value
to the variable.

If the initializer operator is `=` then the variable is declared and assigned
the initial value from the [_expression_](expression.md) on the right.

If the initializer operator is a _inplace-operator_ then initializion depends
if the variable already is reachable from the current scope.
 - __Unreachable__: Create a new variable and the initial value is the result
   of the [_expression_](expression.md).
 - __Reachable__: Call the inplace operator on the already existing variable
   with the result of the [_expression_](expression.md).

It is a syntax error to have a _inplace-operator_ on a variable with `let` or
`view` bindings.

In the example below the first line creates a variable `foo` with the initial
value `12`. The second line adds `23` to the already existing variable `foo`.
The resulting value of `foo` is `35`:

```
var foo += 12;
var foo += 23;
```

> [!NOTE]
> The inplace-operator syntax was explicitly added to allow a variable to be
> assigned multiple lambda functions with different signatures. It basically
> adds function-call operators to the lambda-type in the variable.
> Remember; types are modifiable values by themselves.
