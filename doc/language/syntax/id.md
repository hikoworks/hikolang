# id

## Syntax

Identifiers follow the Unicode UAX #31 (Unicode Identifier and Syntax) rules for
identifiers. Identifiers are case-sensitive. The following characters are added
as part of ID_Start and ID_Continue character classes:
 - `$` (DOLLAR SIGN)
 - `_` (UNDERSCORE)
 - `°` (DEGREE SIGN) This is used for units.

## Semantics

Identifiers are used to name variables, functions, namespaces, types and
keywords.

Internally identifiers are stored and compared in NFC (Normalization Form C).

Identifier that include a double underscore `__` are reserved for the language
implementation.

Identifiers starting with `$` followed directly by decimal digits are used for
positional arguments.

Other identifiers starting with `$` are names of hidden variables:
 - Hidden Context Variables
 - Hidden Member Variables


## Security

For security purposes, it is a reportable error when an identifier violates the
following rules:
 - Characters of identifiers must be part of the UAX #31 Recommended Scripts.
 - After splitting an identifier by characters that are part of the Common or
   Inherited scripts; all characters of each part must be from the same script.
   This is to prevent confusion between identifiers that are visually similar
   but are from different scripts.

