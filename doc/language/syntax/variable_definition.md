# variable-definition

## Syntax

[_storage-class_](storage_class.md)__?__ [_binding-mode_](binding_mode.md) [_fqname_](fqname.md) _type-declaration_**?**
[_initializer_](initializer.md)__?__


### binding_mode

_binding-mode_ __:=__ `var` __|__ `let` __|__ `ref` __|__ `view` 

### type_declaration

_type-declaration_ __:=__ [_coerce-operator_](coerce_operator.md) [_type-expression_](type_expression.md)


## Semantics
Declares a variable with a [_fqname_](fqname.md) which can be used in the scope or
child scopes where the variable is declared.

Variable declarations may appear in the following contexts:
 - In global scope.
 - In a function or lambda.
 - In a code block of a control-expression
 - In a [_condition-expression_](condition_expression.md) of a
   control-expression, the variable is then declared in the scope of all
   following code-blocks of that control-expression.
 - In a class declaration.

If the type is not explicitly set, the exact type of the first assigned value is
used as the type of the variable. The coercion-method is `:=`, which means that
any subsequent assignment to the variable must be of exactly the same type.

If the declaration includes an initializer, then it is used to create the
initial value for the variable. If the declaration does not include a
initializer, then the initial value is created by calling the
default-constructor of the type in the type declaration.

The result of the variable declaration as an expression is a reference to the
variable created, set to its initial value. If the initial value can not be
determined (no type, no default constructor, no initializer, etc.) then the
result of the variable declaration is `void` and therefor can not be used
as part of another expression.

### initializer



### binding modes

All binding modes are syntactic sugar around `var`.

`let a = expr` desugars into:
```
var a = expr
seal a
```

`ref a = expr` desugars into:
```
var a : & = expr
```

`view a = expr` desugars into:
```
var b : &const = expr
```
