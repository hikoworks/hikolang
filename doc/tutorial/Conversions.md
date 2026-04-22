Tutorial - Conversions
======================


Exact match `:=`
----------------

```
var a = 42 : int
var b = 42.0 : f64
```

Widening conversions:
 - Type matches exactly

Super class match `:^`
----------------------

Widening conversions:
 - Type matches exactly
 - Convert reference to a super class


Widening Conversion `:`
-----------------------

```
var a = 42 : f64
var b = 42 : int[0..=100]
```

Widening conversions:
 - Type matches exactly
 - Convert reference to a super class
 - Conversion functions that do not throw



Narrowing Conversion `:!`
-------------------------

```
var a = 2.0 :! int[0..=10]
var b = a :! int[0..=5]
```

Widening conversions:
 - Type matches exactly
 - Convert reference to a super class
 - Convert reference from a super class
 - Conversion functions that do not throw
 - Conversion functions that may throw




