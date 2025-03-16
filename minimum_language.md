# Minimum Language

## Tokens

### Identifier / keyword
Identifiers are a sequence of characters defined in UAX#31 (Unicode Annex 31).

### Operator
Operators are a sequence of Pattern\_Syntax characters defined in UAX#31 (Unicode Annex 31).

### Close / Open
The following characters are used as open/close characters '(', ')', '{', '}', '[', ']'.

### Integer literal

Integer literals without a custom-integer-literal-suffix have the type:
`int[<literal-value>...<literal-value>]`.

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

```bnf
// Any statement which ends in an close-brace '}' implies that a semicolon ';'
// token is appended after the statement.
statements := statements-item ( ';' statements-item )* ';'?

statements-item := statement-label? statement

// Case and default labels are valid outside of switch statements.
statement-label :=
      identifier ':'
    | 'case' expression ':'
    | 'default' ':'

// expressions can not start with a open-brace '{'
statement :=
      <empty>
    | block
    | assembly-block
    | if-statement
    | do-while-statement
    | for-statement
    | switch-statement
    | return-statement
    | break-statement
    | continue-statement
    | expression
```

#### Block

```bnf
block := '{' statements '}'
```

#### Assembly block

```bnf
assembly-block := 'asm' '{' asm-statements '}'
```

#### Expression statement

note: To disambiguate between a block-statement, a lambda can not apear at the start of an expression.
      In other words an expression does not start with a open-brace `{` character.

```bnf
expression-statement := expression
```

#### Return statement

```bnf
return-statement := 'return' expression
```

#### Goto statements

The `goto case` statement can only jump to `case` labels in the same block.
This is because `case` labels may not be unique within a function with multiple
`switch` statements.

The `goto default` statement can only jump to `default` label in the same block.
This is because there may be more than one `default` label within a function
with multiple `switch` statements.


```bnf
goto-statement := goto-label-statement | goto-case-statement | goto-default-statement | computed-goto-statement
goto-label-statement := 'goto' jump-label
goto-case-statement := 'goto' 'case' expression
goto-default-statement := 'goto' 'default'
computed-goto-statement := 
jump-label := identifier
```

#### Break statements

Breaks out of a `while` or `for` loop. You can break out of an outer loop by
specifying the label of the loop in the `break` statement.

```bnf
break-statement := 'break' jump-label?
```

#### Continue statements

Continue the next iteration of `while` or `for` loop. You can continue an
outer loop by specifying the label of the loop in the `continue` statement.

```bnf
continue-statement := 'continue' jump-label?
```

#### If statement

```bnf
if-statement := 'if' if-expression block ( 'elif' if-expression block )* ( 'else' block )?
if-expression := '(' ( if-init-expression | if-simple-expression ) ')'
if-init-expression := init-expression ( ';' if-simple-expression )?
if-simple-expression := expression
```

#### Do-While statement

The while statement can have both a `do` block and a `while` block at the
same time. At the end of a `while` block execution contiues at the start of
the `do` block.

The while statement can have a `else` block, when the `while` expression is
false execution is continued in the `else` block. When using `break` or `goto`
the `else` block is skipped.

```bnf
do-while-statement :=
      ( 'do' block )? 'while' if-expression block ( 'else' block )?
    | 'do' block 'while' if-expression ( 'else' block )?
```

#### For loop statement

```bnf
for-statement := 'for' for-expression block ( 'else' block )
for-expression := '(' ('var' | 'let') variable-name ':' expression ')'
```

#### Switch statement

```bnf
switch-statement := 'switch' if-expression block
```

#### Function definition

Function arguments use the init-expression syntax. The expression after '='
is used when that particular argument is not passed in by the caller. The
expression is executed in the context of the caller.

Like the normal init-expression function arguments values may be converted
using the `__convert__()` function.

The function may be overloaded for different arguments and return types.
The overload-resolution algorithm is as follows:
 - Find a function with an exact match for the argument types and exact return type.
 - Find a function with an exact match for the argument types and conversion matching return type.
 - Find a function with a conversion match for the argument types and exact return type.
 - Find a function with a conversion match for the argument types and conversion matching return type.

```bnf
function-definition := 'function' function-name '(' function-arguments? ')' ( '->' function-return-type )? block
function-name := identifier
function-arguments := function-argument ( ',' function-argument )* ','?
function-argument := init-expression
function-return-type := type-expression
```

#### Extending Syntax

```bnf
syntax-declaration := 'syntax' syntax-match '{' syntax-tokens '}'

```

### Expression

```bnf
expression :=
      sub-expression
    | literal
    | 
```

#### Sub-expression

```bnf
sub-expression := '(' expression ')'
```

#### Literal

```bnf
literal :=
      null-literal
    | bool-literal
    | integer-literal
    | floating-point-literal
    | char-literal
    | string-literal
    | name-expression

null-literal := 'null'
bool-literal := 'true' | 'false'
```

User defined literals have a suffix. The literal will be passed as a string
to one of the following functions:

 * integer-literal: `__integer_literal_<suffix>__()`
 * floating-point-literal: `__float_literal_<suffix>__()`
 * char-literal: `__char_literal_<suffix>__()`
 * string-literal: `__string_literal_<suffix>__()`

Here are the predefined literal functions:

 * `__integer_literal_ih__()` -> i128
 * `__integer_literal_uh__()` -> u128
 * `__integer_literal_iq__()` -> i64
 * `__integer_literal_uq__()` -> u64
 * `__integer_literal_id__()` -> i32
 * `__integer_literal_ud__()` -> u32
 * `__integer_literal_iw__()` -> i16
 * `__integer_literal_uw__()` -> u16
 * `__integer_literal_ib__()` -> i8
 * `__integer_literal_ub__()` -> u8
 * `__integer_literal_is__()` -> isize
 * `__integer_literal_us__()` -> usize
 * `__integer_literal_ip__()` -> iptr
 * `__integer_literal_up__()` -> uptr
 * `__float_literal_h__()` -> f16
 * `__float_literal_f__()` -> f32
 * `__float_literal_d__()` -> f64
 * `__float_literal_q__()` -> f128

#### Name expression

A function keeps track of all dependency-names that are read in a function,
recursively to called functions, excluding dependency-names that are explicitly
passed to called functions.

Argument-number expression refers to the positional argument of the current
function. This is useful for lambda's without explicit argument declaration, but
it may also be used in normal functions.

A temporary is allowed to be assigned to without explicitly declaring it with a
init-expression. Each write of a different type in a block will result in the
creation of a new `$` variable for that type in that block. It is also possible
to read from the `$` variable, but only if there is only one created for the
current block.

```bnf
name-expression := variable-name | argument-number | dependency-name | temporary
variable-name := identifier
argument-number := '$' [0-9]+
dependency-name := '$' identifier
temporary := '$'
```

#### Init expression

If the result type of an expression does not match the type-expression the
expression will be converted by calling the `__convert__()` function using both
the arguments and return type as the signature of the overloaded `__convert__()`
function.

An `explicit` type will not allow the expression result value to be converted.

The variable-access has the following meanings:
 - `var`: The value will be copied or moved to the new variable,
          and the value of the variable may be modified.
 - `let`: The value will be copied, moved or is a immutable reference,
          the value can not be modified.
 - `ref`: A reference of a variable is taken, the value in the referenced
          variable may be modified. It is not possible to re-seat the actual
          reference. A reference can not be to a temporary value.
          It follows that a reference cannot convert a value.
 - `shared`: This variable is shared between function calls, and is only
             initialized on the first call, initialization is done in a
             thread-safe manner. After initialization the programmer is
             responsable for thread-safe access to this variable.
 - `thread`: Variables are shared between function calls on the same thread,
             and is only initialized on the first call on each thread.
             Thread variables are also valid as global variables.

Before a variable is read, the variable must be assigned a value on every
possible path up to the read. If no value or type is specified then all
assignments must result in the same type. If the type-expression is a trait,
all assignment to the variable must result in the same type.

```bnf
init-expression := variable-access variable-name ( ':' 'explicit'? type-expression )? ( '=' value-expression )?
value-expression := expression
type-expression := expression

variable-access := 'var' | 'let' | 'ref' | 'shared' | 'thread'
```

#### Member access

```bnf
root-member-access := '.' member-name
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

#### Lambda expression

```bnf
lambda-expression := '{' ( lambda-expression-arguments 'in' )? block-body '}'
lambda-expression-arguments :=
```

#### Call operator

```bnf
call-operator := simple-call-operator | trailing-lambda-call-operator
simple-call-operator := expression '(' call-operator-arguments? ')'
trailing-lambda-call-operator := expression '(' call-operator-arguments? ')' lambda-expression

call-operator-arguments := call-operaror-argument ( ',' call-operator-argument )* ','?
call-operator-argument := ( '$'? identifier '=' )? expression
```


#### Declaring operators

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

## Types and values

### Value
These are normal values, like:

 * integer numbers
 * floating point numbers
 * string objects
 * enum values
 * tuple values
 * array values
 * object from custom types

### Type-value
Types themselves are also values, you can analyse and create types during
the elaboration phase programatically. Variables, function arguments,
function return values may be types. The type of a type is `type`.

### Expression-value
Normally expressions are type inferred and executed in-place where the
expression is located. For example an expression in a function argument will
be executed before the function is called.

If an expression is assigned or passed to an object with an explicit
`expression` type the expression itself is assigned to the object. An
expression value can be analyzed and modified during elaboration. At run-time
an expression object can be executed. This works as if an `expression` is
a lambda object without arguments.



