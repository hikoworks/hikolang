# Minimum Language

## Tokens

### Identifier / keyword
Identifiers are a sequence of characters defined in UAX#31 (Unicode Annex 31).

### Operator
Operators are a sequence of Pattern\_Syntax characters defined in UAX#31 (Unicode Annex 31).

### Close / Open
The following characters are used as open/close characters '(', ')', '{', '}', '[', ']'.

```bnf
block-open := '{'
block-close := '}'
```

### Integer literal

```bnf
integer := binary-integer | octal-integer | decimal-integer | hexadecimal-integer | simple-decimal-integer
binary-integer := [-+]? '0' [bB] [0-1][0-1_]* custom-integer-literal-suffix?
octal-integer := [-+]? '0' [oO] [0-7][0-7_]* custom-integer-literal-suffix?
decimal-integer := [-+]? '0' [dD] [0-9][0-9_]* custom-integer-literal-suffix?
hexadecimal-integer := [-+]? '0' [xX] [0-9a-fA-F][0-9a-fA-F_]* custom-integer-literal-suffix?
simple-decimal-integer := [-+]? [0-9][0-9]* custom-integer-literal-suffix?
custom-integer-literal-suffix := identifier
```

### Floating point literal

```bnf
floating-point :=
    [-+]? floating-point-integral '.' floating-point-fraction? floating-point-exponent? custom-floating-point-literal-suffix? |
    [-+]? '.' floating-point-fraction floating-point-exponent? custom-floating-point-literal-suffix?
floating-point-integral := [0-9]+
floating-point-fraction := [0-9]+
floating-point-exponent := [eE] [-+]? [0-9]+
custom-floating-point-literal-suffix := identifier
```

### Character / String literal

```bnf
character := "'" character-char | string-escape-code "'"
character-char := [^'\]

string := '"' ( string-char | string-escape-code )* '"' custom-string-literal-suffix?
custom-string-literal-suffix := identifier
string-char := [^"\]

raw-string := 'r"' ( string-char | string-escape-code )* '"'
raw-string-char := [^"]

string-escape-code := "\\" | "\'" | '\"' | '\' ascii-letter | '\u{' unicode-name | unicode-code-point-number '}'
unicode-code-point := [0-9a-fA-F]+
unicode-name := [a-zA-Z0-9-_<> ]+
```

## Syntax

### Statement


#### Statement block

```bnf
block := '{' block-body '}'

// An expression statement at the end of block acts as a implied return statement.
// This implied return statement does not need a semicolon.
block-body :=
    | ( label? ( block-statement | last-line-statement ';' | implied-return-statement ';'? | ';'? ) )
    | ( label? ( block-statement | line-statement ';' | ';' ) ) statements 

label := line-label | case-label

block-statement := block | assembly-block | if-statement | do-while-statement | for-statement | switch-statement
last-line-statement := init-statement | return-statement | goto-statement | break-statement | continue-statement
line-statement := last-line-statement | expression-statement
implied-return-statement := expression-statement

```

#### Assembly block

```bnf
assembly-block := 'llvm{' llvm-statements '}'
```

#### Expression statement

note: To disambiguate between a block-statement, a lambda can not apear at the start of an expression.
      In other words an expression does not start with a open-brace `{` character.

```bnf
expression-statement := expression
```

#### Label

```bnf
label := identifier ':'
```

#### Case label

```bnf
case-label := 'case' expression ':'
```

#### Init statement

```bnf
init-statement := ('var' | 'let') variable-name '=' expression
```

#### Return statement

```bnf
return-statement := 'return' expression
```

#### Goto statements

```bnf
goto-statement := 'goto' jump-label
goto-case-statement := 'goto' 'case' expression
jump-label := identifier
```

#### Break statements

```bnf
break-statement := 'break' jump-label
```

#### Continue statements

```bnf
continue-statement := 'continue' jump-label
```

#### If statement

```bnf
if-statement := 'if' if-expression block ( 'elif' if-expression block )* ( 'else' block )?
if-expression := '(' ( if-init-expression | if-simple-expression ) ')'
if-init-expression := init-expression ( ';' if-simple-expression )?
if-simple-expression := expression
```

#### Do-While statement

```bnf
do-while-statement := ( 'do' block )? 'while' if-expression block ( 'else' block )?
```

#### For loop statement

```bnf
for-statement := 'for' for-expression block ( 'else' block )
for-expression := '(' ('var' | 'let') variable-name ':' expression ')'
```

#### Switch statement

```bnf
switch-statement := 'switch' if-expression block ( 'else' block )
case-statement := 'case' case-label block-statement*
case-label := statement-label
```

### Expression

#### Sub-expression

```bnf
sub-expression := '(' expression ')'
```

#### Name expression

```bnf
name-expression := variable-name | argument-number
variable-name := identifier
argument-number := '$' [0-9]+
```

#### Member access

```bnf
root-member-access := '.' member-name
inject-member-access := 'inject.' member-name
object-member-access := 'this.' member-name
member-access := expression '.' member-name
member-name := identifier
```

#### Prefix operator

```bnf
prefix-operator := @ expression
```

### Suffix operator

```bnf
suffix-operator := expression @
```

#### Binary operator

```bnf
binary-operator := expression @ expression
```

#### Inplace operator

```
inplace-operator := expression @ expression
```

#### Ternary operator

```bnf
ternary-operator := expression '?' expression ':' expression
```

#### Index operator

```bnf
index-operator := expression '[' index-operator-arguments? ']'
inplace-index-operator := index-operator inplace-operator expression
index-operator-arguments := expression ( ',' expression )* ','?
inplace-operator := '=' | '+=' | '-=', etc.
```

The inplace-index-operator has the same precedence as the normal inplace operators.
The inplace-function is passed as the first argument to `__set_at_index__()`. 

 - index-operator calls: `__get_at_index__(` arguments `)`
 - assign-index-operator calls: `__set_at_index__(` operator-function `,` arguments `)`

### Lambda expression

```bnf
lambda-expression := '{' ( lambda-expression-arguments 'in' )? block-body '}'
lambda-expression-arguments :=
```

### Call operator

```bnf
call-operator := expression '(' call-operator-arguments? ')'
trailing-lambda-call-operator := expression '(' call-operator-arguments? ')' lambda-expression
call-operator-arguments := call-operaror-argument ( ',' call-operaror-argument )* ','?
call-operaror-argument := ( call-operator-argument-name | call-operator-injection-name '=' )? expression
call-operator-argument-name := identifier
call-operator-injection-name := 'inject.' identifier
```


### Declaring operators

```bnf
operator-declaration := operator-type 'operator' operator-name operator-precedence operator-function ';'
operator-type := 'suffix' | 'prefix' | 'binary' | 'inplace'
operator-precedence := 'precedence' integer-literal ( 'left' | 'right' )
operator-function := 'function' function-name
operator-name := uax31-identifier | uax31-pattern-syntax+
```

These are the default operator definitions:

```
suffix operator ++ precedence 10 right function __post_increment__;
suffix operator -- precedence 10 right function __post_decrement__;

prefix operator - precedence 20 left function __neg__; 
prefix operator + precedence 20 left function __pos__; 
prefix operator ~ precedence 20 left function __bitwise_not__; 
prefix operator not precedence 20 left function __not__; 
prefix operator ++ precedence 20 left function __increment__; 
prefix operator -- precedence 20 left function __decrement__; 

binary operator * precedence 100 left function __mul__; 
binary operator &* precedence 100 left function __wrapping_mul__; 
binary operator ?* precedence 100 left function __checked_mul__; 
binary operator / precedence 100 left function __div__; 
binary operator % precedence 100 left function __mod__;
binary operator ** precedence 100 left function __pow__; 
binary operator &** precedence 100 left function __wrapping_pow__; 
binary operator ?** precedence 100 left function __checked_pow__; 

binary operator + precedence 110 left function __add__; 
binary operator &+ precedence 110 left function __wrapping_add__; 
binary operator ?+ precedence 110 left function __checked_add__; 
binary operator - precedence 110 left function __sub__;
binary operator &- precedence 110 left function __wrapping_sub__;
binary operator ?- precedence 110 left function __checked_sub__;

binary operator << precedence 120 left function __shift_left__; 
binary operator |<< precedence 120 left function __rotate_left__; 
binary operator >> precedence 120 left function __shift_right__; 
binary operator |>> precedence 120 left function __rotate_right__; 

binary operator ..< precedence 130 left function __range__;
binary operator ... precedence 130 left function __closed_range__;

binary operator <=> precedence 200 left function __cmp__;

binary operator < precedence 210 left function __lt__; 
binary operator > precedence 210 left function __gt__; 
binary operator <= precedence 210 left function __le__; 
binary operator >= precedence 210 left function __ge__;

binary operator == precedence 220 left function __eq__; 
binary operator != precedence 220 left function __ne__; 

binary operator & precedence 310 left function __bitwise_and__; 
binary operator ^ precedence 320 left function __bitwise_xor__; 
binary operator | precedence 330 left function __bitwise_or__; 

binary operator && precedence 410 left function __and__; 
binary operator ^^ precedence 420 left function __xor__; 
binary operator || precedence 430 left function __or__;
binary operator and precedence 410 left function __and__; 
binary operator xor precedence 420 left function __xor__; 
binary operator or precedence 430 left function __or__;

inplace operator = precedence 1000 right function __assign__; 
inplace operator += precedence 1000 right function __inplace_add__; 
inplace operator &+= precedence 1000 right function __inplace_wrapping_add__; 
inplace operator -= precedence 1000 right function __inplace_sub__; 
inplace operator &-= precedence 1000 right function __inplace_wrapping_sub__; 
inplace operator *= precedence 1000 right function __inplace_mul__; 
inplace operator &*= precedence 1000 right function __inplace_wrapping_mul__; 
inplace operator /= precedence 1000 right function __inplace_div__; 
inplace operator %= precedence 1000 right function __inplace_mod__; 
inplace operator <<= precedence 1000 right function __inplace_shift_left__; 
inplace operator |<<= precedence 1000 right function __inplace_rotate_left__; 
inplace operator >>= precedence 1000 right function __inplace_shift_right__; 
inplace operator |>>= precedence 1000 right function __inplace_rotate_right__; 
inplace operator &= precedence 1000 right function __inplace_bitwise_and__; 
inplace operator ^= precedence 1000 right function __inplace_bitwise_xor__; 
inplace operator |= precedence 1000 right function __inplace_bitwise_or__; 
inplace operator &&= precedence 1000 right function __inplace_and__; 
inplace operator ^^= precedence 1000 right function __inplace_xor__; 
inplace operator ||= precedence 1000 right function __inplace_or__; 
```

## Extending Syntax

```bnf
syntax-declaration := 'syntax' syntax-match '{' syntax-tokens '}'

```

## Function definition

```bnf
function-definition := 'function' function-name '(' function-arguments? ')' ( '->' function-return-type )? '{' function-block '}'
function-name := aux31-identifier
function-arguments := function-argument ( ',' function-argument )*
function-argument := function-argument-name-type | function-argument-value
function-argument-value := value-expression
function-argument-name-type := function-argument-name ( ':' function-argument-type )? ( '=' function-argument-default-value )?
function-argument-name := aux31-identifier
function-argument-type := type-expression
function-argument-default-value := value-expression
function-return-type := type-expression
function-block := ( '{' function-block '}' | function-statement )*
function-statement := 
```



