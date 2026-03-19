# Variables


## var
```
var i8 = int[0..=255]
var a : i8 = 42
var b = i8(42)
a = 3
```

## const
Const is a type modifier, it creates a distinct type with limited compatibility.
Access to members of a `const` type is restricted by `const` specialized member
functions that allow or not allow access to members.

## frozen
Frozen is the state of a value, it means the value can not be changed anymore.

## seal

 - `seal const <foo>`: Change the type of the variable `<foo>` to the `const`
   variant.
 - `seal freeze <foo>`: Freeze the value of the variable `<foo>`, so that the
   value can not be modified.
 - `seal <foo>`: Both `freeze` and `const` a variable.

Implicit `seal`:
 - When a block containing `let <foo> = <value>` is specialized, `<foo>` is implicitly sealed.
 - When a block containing a call to `<foo>()` is specialized, `<foo>` is implicitly sealed.
 - When a block containing a concrete type is specialized, that type is implicitly sealed.

## let

```
let a = 42
```

Is syntactic sugar for:

```
var a = 42
seal a
```
