# throw statement

[_identifier_]: identifier.md


## Syntax

_throw-statement_ := `throw` [_identifier_]__?__ `;`


## Semantics

The `throw` statement returns an error from the function. The [_identifier_] is part
of the `std.error_code`. The [_identifier_] will be added to `std.error_code` if it did not exist.

The function will be internally marked as throwing that error, and the caller must catch any
(non-auto-propogating) errors that can be thrown.
