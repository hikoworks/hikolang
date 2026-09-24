# hikolang
[![Windows](https://github.com/hikoworks/hikolang/actions/workflows/build-on-windows.yml/badge.svg?branch=main)](https://github.com/hikoworks/hikolang/actions/workflows/build-on-windows.yml)
[![Linux](https://github.com/hikoworks/hikolang/actions/workflows/build-on-linux.yml/badge.svg?branch=main)](https://github.com/hikoworks/hikolang/actions/workflows/build-on-linux.yml)
[![MacOS](https://github.com/hikoworks/hikolang/actions/workflows/build-on-macos.yml/badge.svg?branch=main)](https://github.com/hikoworks/hikolang/actions/workflows/build-on-macos.yml)
[![Version](https://img.shields.io/badge/dynamic/json?url=https://raw.githubusercontent.com/hikoworks/hikolang/main/vcpkg.json&label=Latest%20Version&query=$[%27version%27]&color=blue)](https://github.com/hikoworks/hikolang/releases/latest)
[![License](https://img.shields.io/github/license/hikoworks/hikolang.svg)](https://github.com/hikoworks/hikolang/blob/main/LICENSE)
[![Coverage](https://codecov.io/github/hikoworks/hikolang/graph/badge.svg?token=P95N8UFH1D)](https://codecov.io/github/hikoworks/hikolang)


Hikolang is a systems programming language built around the idea that
correctness, expressiveness, and low-level control do not have to be
competing goals.

Its central idea is to make information that is normally implicit in
systems programs explicit to the compiler, while still allowing programs
to extend and refine that information themselves.

Rather than relying solely on conventions, runtime checks, or programmer
discipline, hikolang allows the compiler to reason about properties that
are traditionally left implicit, without taking control away from the
programmer.

The language provides explicit mechanisms for handling errors, integer
safety, effects, memory, units, and compile-time computation, while
keeping the resulting programs suitable for systems-level development.

The language moves several traditionally separate concerns into the type
system and compilation process. Integer ranges can be tracked statically,
units participate in dimensional analysis, effects can be constrained, and
contracts can describe the assumptions made by functions. Compile-time
evaluation and reflection are treated as fundamental language features
rather than as separate metaprogramming facilities.

Hikolang programs are compiled as a whole rather than as independently
compiled libraries. Declarations and extensions therefore become part of
the compilation environment at their point of definition, allowing syntax,
types, effects, errors, units, and other language constructs to be extended
throughout the program.

A dedicated elaboration phase bridges compile-time and runtime. Code can
therefore participate directly in compilation without making compile-time
behavior indistinguishable from runtime behavior.

The result is a language designed for programs where explicit guarantees
and predictable behavior matter: programs that require explicit failure modes,
efficient representations, and strong compile-time reasoning, without giving
up the ability to express higher-level abstractions.

Features:

 * Fast errors
 * Safe integers
 * Unit system
 * Hidden context arguments
 * Universal call syntax
 * Variant with associated values
 * with_effect / without_effect constrain
 * Contracts and invariants
 * Elaboration phase
 * Compile time reflection
 * Builtin package manager
 * Extendable syntax


## Fast errors

There are three different outcomes from a function:
 * A value was returned using a value `return` statement.
 * An _empty_ was returned using a non-value `return` statement.
 * An _error_ was thrown using a `throw` statement.

An error must be handled by the caller. The `if` statement include
`catch` and `empty` clauses to make it easier.

```
if (x = foo()) {
    print("foo() returned a value {:1}", x)
} empty {
    print("foo() did not return anything")
} catch (out_of_bound) {
    print("foo() causes an out-of-bound error")
    throw   // rethrow the error.
}
```

Some error-codes may be annotated with `@auto_rethrow` for which the
compiler will automatically synthesize a catch-rethrow statement. This
keeps the invariant "An error must be handled by the caller".


## Safe integers

All operations on integers will either:
 * Always have a mathematically correct result, or
 * it may throw a error that must be caught locally.

Errors that may be thrown by certain operations:
 * Domain-errors such as division by zero, or sqrt of a
   negative number.
 * Narrow-error when explicitly casting to a smaller integer
   range.

The `int` template type infers based on interval arithmatic to fit
the full result of an operation: 

```
foo = fn(x : int[10...20], y : int[2...4]) {
    // return type is infered as int[20...80]
    return x * y
}
```

`int` is the basic integer type of the standard libary and the result
of integer literals. `int` values are ABI compatible with the equal or
next larger signed or unsigned C++ integer.

The range of an `int` is arbitrarilly long, but static. The memory size
of an `int` is based on the range.

The second integer type is `long`, this integer dynamically scales in size
and is allocated on the heap. The interval of an `int` are based on
`long`.


## Unit system

Types and values can be tagged with a unit, which is used
for dimensional analysis.

The `#` operator converts a scalar-value to a unit-value by multiplying
by the unit. The `#-` operator converts a unit-value back to a
scalar-value by dividing by the unit.

```
duration = 15.0 #min
speed = 100.0 #(km/h)
distance = speed * duration
distance_in_km = distance #-km

convert = fn(length : real #m, ppi : real #(px/in)) -> real #px
{
    return length * ppi
}
```


## Hidden context arguments

Context arguments reduces the need for global variables in many uses. It makes
it easy to inject context in unit-tests as well.

```
foo = fn(x) {
    // $y is used, which makes foo() require $y to be passed in.
    return x + $y
}

bar = fn(x) {
    // $y is required by foo(), so bar requiress $y passed in.
    // $y is automatically and invisibly passed into foo()
    return foo(x)
}

qux = fn(x, y) {
    // qux() does not allow $y to be passed in.
    // Pass in $y explicitly into bar.
    return bar(x, $y=y)
}
```


## Universal call syntax

Functions and member functions may be called in two different ways:
 * `foo(a, b)`
 * `a.foo(b)`

Functions and members are handled together in a single overload resolution
algorithm.


## Variant with associated values

Enums members have zero or more associated values, like the following
optional type, which is a template. Template arguments use the bracketed
argument syntax.

```
.std.optional[T : type] = variant {
    none
    some(T)
}
```

Niche values and niche-mask allow optimization to compress the enum's index-tag
to occupy the same space as the associated value. For example the address of a
reference can never be zero, so this niche-value can be used for `none` with an
optional reference.


## effect constrain

Sometimes you want to limit the effects that functions can make,
for example you may not want to do any: allocations, IO or block.

```
// This function's implementation and any callers are marked to have the
// effects +io or +block depending if the arguments are constant at compile
// time.
read = fn(fd, size) -> string {
  if (size == 0) {
    ...
  } else if (size <= 4096) effect(+io) {
    ...
  } else effect(+io, +block) {
    while (...) {...}
  }
}

foo = fn(fd, n) {
  effect(!block) {
    t = read(fd, 4096); // OK
    u = read(fd, 6000); // ERROR: read() has effect 'block'
    v = read(fd, n); // ERROR: read() has effect 'block'
  }
}
```


## Contracts and invariants

Function and method definitions can include `pre()` and `post()`
clauses that are checked in the scope of the caller to make sure
that the arguments, type-invariant and return values are correct.


## Elaboration phase

After parsing, the compiler enters the **elaboration phase**, where it resolves
the meaning of the program and produces the information required for
translation. Elaboration may happen lazily: translating one definition can
trigger elaboration of another definition when its meaning is required.

Translation may also perform **compile-time evaluation**, including constant
folding. An explicit `@fold` annotation requires an expression to be evaluated
during compilation:

```text
foo = fn(x) {
    y = @fold read_from_file()
    return y + x
}
```

The result of compile-time evaluation can be materialized as part of the
generated program. This also applies to allocations. The **allocation itself**:
allocated during compile-time evaluation becomes a corresponding runtime
allocation, with its resulting contents preserved.

For example, a compile-time-created array can become an allocated array in the
generated program rather than merely having its contents copied into some
unrelated runtime storage; and can therefore be deallocated.


## Compile time reflection

The language treats types as if they are first-class values.
This means types themselves are of a meta-type, recursively.

Since types are values, you can query and manipulate types at compile
time. This allows you to write generic code that can work with any type, and
even create new types at compile time.

Types are modifiable until they are used as a type, at this point they become
frozen. Only enums are special, because members can be added as long as their
values are previous unused values of the underling `int` type.

## Builtin Package Manager

The compiler will automatically clone and update git repositories that
are imported with the `import git` statement. Repositories are
cloned into the `_hkdeps` directory, by recursively scanning the
repositories.

Directory structure within a repository is free-form; the compilation
order and conditional compilation is determined after scanning the
prologue of each module (a module is a file).


## Extensible syntax

### Custom operators

The language allows you to define custom operators. This is done by registering
a keyword or pattern-syntax, precedence and associativity, and a function that
will be called when the operator is used.


### Custom units and domains

New units can be added, including full system of units (domains).


### Adding errors

Errors-codes can be added anywhere in the program, multiple definition merge.


### Adding effects

Effects can be added anywhere in the program, multiple definition merge.


### Adding members to open-enums.

Even after an enum is frozen you may add new members, as long as their values
where unused and fit the interval of the underlying `int` type.


### Metatypes

A function marked with `metatype` becomes a keyword that returns a type.

```
class = fn(definition, arguments) metatype {
  ...
  return new_type
}

T[template_argument: type] = class {
  ...
}
```

