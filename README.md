# hikolang
[![Windows](https://github.com/hikoworks/hikolang/actions/workflows/build-on-windows.yml/badge.svg?branch=main)](https://github.com/hikoworks/hikolang/actions/workflows/build-on-windows.yml)
[![Linux](https://github.com/hikoworks/hikolang/actions/workflows/build-on-linux.yml/badge.svg?branch=main)](https://github.com/hikoworks/hikolang/actions/workflows/build-on-linux.yml)
[![MacOS](https://github.com/hikoworks/hikolang/actions/workflows/build-on-macos.yml/badge.svg?branch=main)](https://github.com/hikoworks/hikolang/actions/workflows/build-on-macos.yml)
[![Version](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/hikoworks/hikolang/main/vcpkg.json&label=Latest%20Version&query=$[%27version%27]&color=blue)](https://github.com/hikoworks/hikolang/releases/latest)
[![License](https://img.shields.io/github/license/hikoworks/hikolang.svg)](https://github.com/hikoworks/hikolang/blob/main/LICENSE)
[![Coverage](https://codecov.io/github/hikoworks/hikolang/graph/badge.svg?token=P95N8UFH1D)](https://codecov.io/github/hikoworks/hikolang)

 * Safe Integers
 * Unit System
 * Hidden Context Arguments
 * Universal Call Syntax.

 * elaboration phase
 * language syntax is extendable
 * inline assembler
 * compile time reflection, types are values.
 * hidden dependency injection, both functions and structs.
 * enum only has named-values, it is not an integer.
 * function overloading, on both arguments (type and non-type) and return type.


## Safe integers

All operations on integers will either:
 * Always have a mathematically correct result, or
 * it may throw a error that must be caught locally.

Errors that may be thrown by certain operations:
 * Domain-errors such as division by zero, or sqrt of a
   negative number.
 * Narrow-error when explicitly casting to a smaller integer
   range.

The `int` template type automatically scales to fit the full
result of an operation: 

```
fn foo(x : int[10..=20], y : int[2..=4]) {
    // return type is infered as int[20..=80]
    return x * y
}
```

`int` is the basic integer type and the result of integer literals.
The `int` with proper ranges are compatible with both signed and
unsigned integers with C and C++.

The second integer type is `long`, this integer dynamically scales in size
and is allocated on the heap, and includes SIO (Short Integer Optimization).

## Units system
Real, rational and decimal types can be tagged with a unit, which is used
for dimensional analysis.

Conversions between unit-systems like using a resolution value
like `72.0 (px/in)` works as expected.

```
let duration = 15.0 min
let speed = 100.0 (km/h)
let distance = speed * duration
let distance_in_km = distance / 1km

fn convert(length : real #m, ppi : real #(px/in)) -> real #px
{
    return length * ppi
}
```

## Hidden Context Arguments
Context arguments reduces the need for global variables in many uses. It makes it easy
to inject context in unit-tests as well.

```
fn foo(x) {
    // $y is used, which makes foo() require $y to be passed in.
    return x + $y
}

fn bar(x) {
    // $y is required by foo(), so bar requiress $y passed in.
    // $y is automatically and invisibly passed into foo()
    return foo(x)
}

fn qux(x, y) {
    // qux() does not allow $y to be passed in.
    // Pass in $y explicitly into bar.
    return bar(x, $y=y)
}
```


## Universal call syntax
Functions and member functions may be called in two different ways:
 * `foo(a, b)`
 * `a.foo(b)`

## Builtin Package Manager
The compiler will automatically clone and update git repositories that
are imported with the `import git` statement. Repositories are
cloned into the `_hkdeps` directory, by recursively scanning the
repositories.

Directory structure within a repository is free-form; the compilation
order and conditional compilation is determined after scanning the
prologue of each module (a file).

## Compile-time allocations survive into runtime.
Since a lot of code will be executed at compile-time it is likely that
a lot of allocations will be executed. Especially since `int` type contains
two `long` values for the range which may allocate. And all `int` literals
are a `string`.

So allocations during compilation must survive into the executable.

 * Surviving allocation are compressed in a read-write `.alloc` section
 * A automatic generated/manual `__pointer_discovery__()` method is used to
   find all the pointers in the program.
 * If an object has encoded/tagged pointers you can add a `__pointer_relocate()`
   method.
 * In case the read-write .section is relocated:
    - The linker will rewrite the pointers, and
    - `__pointer_relocate__()` will be called.
 * The read-write `.alloc` section is part of the normal allocation,
   allowing deallocation and reusing.

## Enum with associated values
Enums members have zero or more associated values, like the following
optional type, which is a template. Template arguments use the bracketed
argument syntax.

```
enum optional[T : type] {
  none
  some(T)
}
```

Niche values and niche-mask allow optimization to compress the enum's index-tag to
occupy the same space as the associated value. For example the address of a reference
can never be zero, so this niche-value can be used for `none` with an
optional reference.

## Capturing string literals
String literals with the `t` prefix like the following `t"Hello {foo()}"`
are converted to a tuple of the following form: `("Hello {1:}", foo())`.

## Types are values
All types are constructed from templates. Returned types are interned; two types
returned from the same template with same arguments are identical.

Templates are normal functions that return a type, and like normal functions may have
an overload-set. Templates may be modified, at compile time, before the template is
used to instantiate a type. Since all functions are templates, any type-template in
a type decoration on arguments and return types are not instantiated until that
function is called.


A partially instantiated template returns a wrapper template that calls the original
template with the remaining arguments. This means that a type returned from the
wrapper template still is identified as comming from the original template.

For ease of use when a zero argument template is used where a type is needed it is
automatically instantiated there.

## Elaboration Phase
Certain languages have a separate elaboration phase during compilation.

In this phase the compiler will fill in all:
 * type and non-type arguments of templates.
 * global variables.
 * Guaranteed full constant folding and function elimination.

The process basically does full constant folding by executing functions until no
more functions can be executed.

It is a reportable error if any expression is left of which the value needs to
be known for the next compilation phase.

Note: all functions by default can be executed at compile time. 
Note: full constant folding allows generic/templated functions without a special syntax.
Note: since low level functions are written as inline-assembly, inline-assembly must be
      executable during compile time, possibly using an interpreter/JIT.

## Extensible syntax
### Custom operators
The language allows you to define custom operators. This is done by registering a
a keyword or pattern-syntax, precedence and associativity, and a function that
will be called when the operator is used.

### Custom literals
The language allows you to define custom literals. This is done by registering a
suffix-keyword and a function that will be called when the suffix is used with a literal.

The literal is passed to the function as a string, and the function determines the
returned value and type. Since the function is called at compile time, it can
dynamically create both the value and the type.

## Inline assembly
Inline assembly is supported through the `llvm` directive. This allows you to write low-level
LLVM assembly code directly in your source code. The inline assembly may even be executed during
the elaboration phase.

```
function foo(a : __i32__, b : __i32__) -> __i32__
{
    var c = a &+ b
    var r = 0 : __i32__

    llvm {
        %tmp = mul i32 %c, %a
        store i32 %tmp, i32* %r.addr
    }

    return r
}
```

## Compile time reflection
The language treats types as if they are first-class values.
This means types themselves are of a meta-type, recursively.

Since types are values, you can interrigate and manipulate types at compile time.
This allows you to write generic code that can work with any type, and even
create new types at compile time.

Meta-types are defined as built-in, by the standard library, and can be extended
by the user.



## Extending syntax
There is macro system that allows you to match with tokens and replace it with a different expression.

