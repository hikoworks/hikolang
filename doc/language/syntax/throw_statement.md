# throw statement

[_id_]: id.md


## Syntax

_throw-statement_ := `throw` [_id_]__?__ `;`


## Semantics

The `throw` statement returns an error from the function. The [_id_] is part
of the `std.error_code`. The [_id_] will be added to `std.error_code` if it did not exist.

The function will be internally marked as throwing that error, and the caller must catch any
(non-auto-propogating) errors that can be thrown.
