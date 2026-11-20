# File

[_prologue_]: prologue.md
[_statement-list_]: statement_list.md


## Syntax

_file_ := [_prologue_] [_statement-list_]


## Semantics

A file is split into a [_prologue_] and a [_statement-list_]. When the compiler
is evaluating compilation order it only needs to read the [_prologue_].


The [_statement-list_] is evaluated from top to bottom and may modify the
internal state of the parser. Each statement may modify:

 * Adding new error-codes
 * Adding new expression-operators
 * Adding new units and domains
 * Adding new effects
 * Adding new meta-types
 * Changing the current namespace

