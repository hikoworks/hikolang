# If control expression

## Syntax

```
"if" "(" condition-expression ")" "{" statement-list "}"
( "catch" "(" error-match-expression ")" "{" statement-list "}" )*
( "catch" "{" statement-list "}" )?

(
    "elif" "(" condition-expression ")" "{" statement-list "}"
    ( "catch" "(" error-match-expression ")" "{" statement-list "}" )*
    ( "catch" "{" statement-list "}" )?
)*

( "else" "{" statement-list "}" )?
```

Alternatively when there is no `elif` clause, the `catch` clause may be
placed after the `else` clause.
```
"if" "(" condition-expression ")" "{" statement-list "}"
"else" "{" statement-list "}"
( "catch" "(" error-match-expression ")" "{" statement-list "}" )*
( "catch" "{" statement-list "}" )?
```