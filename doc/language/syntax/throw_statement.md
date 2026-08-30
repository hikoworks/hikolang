# throw statement

## Syntax

_throw-statement_ := `throw` `fatal`__?__ [_identifier_] `;`


## Semantics

The `throw` statement returns an error from the function. The [_identifier_] is part
of the `std.error_code`. The [_identifier_] will be added to `std.error` if it did not exist.

The function will be internally marked as throwing that error, and the caller must catch any
errors that can be thrown.

If `fatal` clause is added, it means the [_identifier_]-error is thrown as a fatal-error.
Fatal-errors do not need to be catched.
