
The language has the following features:
 - Compiled, system language
 - Builtin package manager, modules, packages (git repositories), conditional
   compilation
 - All integers are ranged; which may mean that when initializing
   a variable with an integer literal it needs to be converted to
   an integer of a larger range.
 - Type inferred, type annotated (matching and conversion)
 - Types are first class values. Types are mutable, but are frozen
   lazilly when used.
 - All function definitions are templates.
 - All type definitions are templates.
 - All type definitions are syntactic sugar around functions that
   return a type (since types are mutable, so are functions)
 - Types may be tagged with a unit for domain analysis.

Here are some language constructions:
 - Comments start with `//`
 - Each file has a prologue, the prologue ends with a module declaration like:
   + `import foo; module bar.baz`
   + `import foo; program "my_program"` (a program in an end-point module
     declaration)
   + `import bar.baz; module bar 1.0.0` (a module with a version number is an end-point)
 - Function definitions look like:
   + `fn foo(x, y : int[4..=10], return int) { ... }`
   + `fn foo[x, y : int[4..=10], return int] { ... }` (identical to the
     previous, but must be called like `foo[1, 2]`, used for type templates and
     index operator overload)
   + `var foo = fn(x, y : int[4..=10], return int) { ... }` (desugared)
 - Variables are declared like:
   + `var foo = 42.0`
   + `var bar = 10 : int[0..<100]`
   + `var baz = foo`
   + `var bla = foo : &mut` (A mutable reference to foo)
   + `var beer = foo : &const` (const strips the `mut` capability from the type)
   + `let qux = 42.0` syntactic sugar of `var qux = 42.0; seal qux`.
   + `ref qax = foo` syntactic sugar of `var qaz = foo : &mut`.
   + `view qix = foo` syntactic sugar of `var qix = foo : &const`.
 - Type conversions:
   + `10 : f64` convert `10 : int[10..=10]` into a `f64`. This is a widening conversion. It matches 1:1, or matches the super-type, or is converted by
    a conversion function.
   + `foo :! int[0..=10]` narrowing conversion may throw, this is used also to
    match a super-type with a derived type.
   + `foo :^ bar` Match with the super class `bar`.
   + `foo := bar` Match exactly with `bar`.
 - Types are declared as:
   + `struct foo { var bar : int = 4; var baz : string }`
   + `var foo = struct { var bar : int = 4; var baz : string }` (desugared)
   + `struct qux[T : type] { var bar : T }`
 - Units:
   + `5.0 m`
   + `10.0 (m/s)` (complex unit expressions are inside parenthesis)
   + `fn foo(x : float (m/s))` (This matches, it does not actually change the
     numeric value which may have been in feet/minute)
   + Conversion from scalar to unit is done by multiplying or dividing by a
     literal with a unit `foo * 1.0 m` or `foo / 1.0 m`.
   + There are multiple realms of units. For example SI using a tuple of
     exponents for each unit, you can also define others such a font-space in
     which distances are in Em, or screen-space for pixels. And conversions
     between these realms like with ppi (pixels per inch), ppi is a ratio
     between SI and screen space length.
 - Member functions:
   + `struct foo { fn bar(self := foo, x : int) { ... }}`
   + `fn foo.bar(self := foo, x : int) { ... }`
   + `foo.bar = fn (self := foo, x : int) { ... }`
 - Universal call syntax:
   + `foo(a, 10)`
   + `a.foo(10)` (different syntax, but same semantics)
 - Implicit function arguments:
   + `fn foo(x) { print($b) }` (`$b` is an implicit function argument)
   + `fn bar(x) { foo(x) }` (`bar()` forward `$b` to `foo()`)
   + `bar(10, $b = "hello")` (At some point `$b` must be explicitly passed in)
 - Exceptions:
   + Exceptions are returned as a `enum result[T : type] { some(T);
     error(error_code) }` type, which returns either the normal value of the
     function or a numeric error code.
   + Exceptions must be caught locally.


