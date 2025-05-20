# name-expression

## Syntax

[_identifier_](identifier.md)

## Semantics

### Variable name
A identifier used in an expression is a variable name. A variable names is searched in:
- The current scope, then parent scopes,
- The instance variables,
- Variable declared in the namespace,
- The global variables.

If the name is not found in any of these scopes, it may be a enum name.
If it is not a enum name, it is an error.

> [!NOTE]
> Types and namespaces are values stored in variables. So a variable-name may
> point to a variable containing a type or namespace.

### Enum name
If a name is directly used as a operand or a function argument, all overloaded functions
are searched. When the type of the function argument matches a enum-type which has the same
name, the function is called with the enum-type as argument. If multiple such functions
are found this is an static error.
