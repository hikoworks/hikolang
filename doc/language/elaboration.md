# Elaboration

Hikolang separates program execution into three phases: **elaboration**,
**translation**, and **runtime**.

The distinction is important because Hikolang allows ordinary language
expressions to execute during compilation. Compile-time execution is not a
separate metaprogramming language; it uses the same language and the same types
as runtime code. The compiler determines when an expression must be evaluated,
when it may be evaluated early, and when it must remain part of the generated
program.

# Elaboration

Elaboration is the phase in which the program is constructed.

Modules are parsed depth-first. As each module is parsed, statements at file
scope are elaborated immediately. Most file-scope statements define functions,
types, variables, effects, errors, units, or other declarations, but a
statements may also modify an existing construct.

Because these statements execute as they are encountered, later parts of the
program observe the definitions and modifications produced by earlier
statements.

Bodies of function and type declarations are different. Statements inside these
bodies are not elaborated merely because it was declared. They are elaborated
when the compiler needs to evaluate or translate an call to a function or use a
type.

Elaboration is therefore demand-driven as well as sequential: the compiler
evaluates whatever is necessary to construct the program's compilation
environment.

During elaboration, the compiler environment is the execution environment. An
operation that performs I/O, accesses the filesystem, or otherwise observes its
environment does so in the compiler's environment.

For example, a declaration may compute a value, inspect a type, construct
another type, or allocate an object. The resulting value becomes part of the
compilation state and can subsequently be used by other declarations.


# Translation

Once the reachable declarations have been elaborated, the compiler translates
the functions and other runtime code that must be emitted into the executable,
such as the `main()` function.

Translation is also capable of triggering elaboration. A function can refer to a
type whose definition has not yet been completed, for example. Before the
compiler can generate code that uses that type, the type must be complete, so
the necessary elaboration is performed.

The compiler therefore does not have a strict boundary where all elaboration
happens first and all code generation happens afterward. Translation can
discover new compile-time work as it encounters the program.

This allows the compiler to remain lazy about computations that are not needed
while still guaranteeing that all information required for generated code is
available when it is needed.


# Constant-folding

Hikolang aggressively performs constant-folding.

By default, functions are assumed to be **phase-invariant**: evaluating them
during compilation produces the same observable result as evaluating them at
runtime. Such functions may therefore be evaluated by the compiler whenever
doing so is useful.

Constant-folding is an optimization rather than a separate execution phase. The
compiler may evaluate an expression early.

Some functions are **phase-variant**. They are annotated with
`@effect(phase_variant)` because their behavior can depend on whether they
execute in the compiler or runtime environment.

For example, a function that reads a file might return the contents of a file
during elaboration, but must read the file from the runtime's environment
when executed at runtime.

The rules are:

* During **elaboration**, calls are evaluated immediately, including calls to
  phase-variant functions.
* During **constant-folding**
  - phase-invariant calls may be evaluated immediately.
  - calls to phase-variant functions are normally translated into runtime code
    rather than evaluated by the compiler.
  - A phase-variant expression may explicitly be marked with `@fold` to require
    evaluation during constant-folding.
* During **runtime**, translated code executes in the runtime environment.

This distinction prevents accidental execution of environment-dependent
operations during optimization while still allowing the programmer to explicitly
request compile-time evaluation.

For example:

```text
@effect(phase_variant)
read_value_from_file = fn(path) {
    ...
}

main = fn() {
    x = read_value_from_file("file.txt")
}
```

Here the call to `read_value_from_file()` remains runtime code when encountered
during constant-folding.

An explicitly folded call can instead request compile-time evaluation:

```text
main = fn() {
    x = @fold read_value_from_file("file.txt")
}
```

The resulting value is then available to the compiler rather than requiring the
generated program to perform the call.

Expressions inside a phase-variant function are still optimized normally. A
phase-variant function does not make its entire body phase-variant. A
expression that is phase-invariant may still be constant-folded, while an
environment-dependent operation remains runtime code.


## Compile-time values and runtime values

Compile-time evaluation does not imply that its results must disappear when
compilation finishes. A value produced during elaboration or constant-folding
can become part of the generated program.

This has implications for allocations made during elaboration and
constant-folding.

Hikolang permits compile-time evaluation to allocate memory. Such allocations
may contain ordinary runtime objects, including values whose representation
depends on dynamically allocated storage.

The compiler therefore cannot simply discard the compiler's allocation after
evaluating an expression. If a resulting value is referenced by generated
runtime code, the allocation must survive into the executable.


### Moving compile-time allocations into the executable

Before translation is complete, allocations created during elaboration and
constant-folding belong to the compiler's execution environment.

When the compiler emits the executable, allocations that are still reachable by
runtime values are transferred into the generated program.

Conceptually, the compiler takes the live portion of its compile-time allocation
state and embeds it into the executable as an **initial allocation arena**.

The generated program starts with this arena already populated.

The objects in the arena are ordinary runtime allocations. They are not
immutable compiler artifacts and they do not remain owned by the compiler.
Runtime code may access them, modify them, and deallocate them according to the
normal rules of the language.

Once an allocation has been transferred into the executable, its origin during
compilation has no special significance to the runtime.

For example:

```text
make_string = fn() {
    return "hello"
}

value = make_string()
```

`make_string()` is evaluated during compilation and its result requires an
allocation, that allocation can be embedded into the executable. Runtime code
receiving `value` observes an ordinary runtime value.

Likewise, a compile-time computation may construct a larger data structure:

```text
table = build_table()
```

`build_table()` is evaluated during elaboration and constructs dynamically
allocated objects, those objects can be transferred into the executable together
with the resulting `table`.

The runtime therefore starts with the result of the compile-time computation,
rather than having to reconstruct it.


### The embedded allocation arena

The embedded allocation arena is best understood as an initial state of the
runtime allocator.

It is not a frozen snapshot of compiler memory.

During compilation, the compiler may have allocated objects temporarily in order
to perform a calculation. Only allocations that remain necessary for the
generated program are transferred.

After the program starts, the transferred memory participates in normal
allocation semantics:

 * objects may be read and modified;
 * objects may be deallocated;
 * references may become invalid when their objects are deallocated, according
   to the normal language rules;
 * freed memory may be reused by subsequent runtime allocations.

Consequently, an allocation does not need to remain permanently associated with
the compile-time arena merely because it originated there.

The important property is that **allocation lifetime crosses the
compilation/runtime boundary**. Compilation can construct the initial object
graph, and runtime execution can subsequently manage that object graph as
ordinary program state.

## Target-Machine Semantics During Cross-Compilation

Elaboration and compile-time evaluation operate on behalf of the target program,
not the machine on which the compiler happens to be running. This distinction
becomes important when compiling for a target whose in-memory representation
differs from that of the host.

During cross-compilation, elaboration and constant folding must therefore
emulate the relevant properties of the target machine. In particular, whenever
the representation of a value is observable by the program, the compiler must
use the target's representation rather than the host's representation.

This includes properties such as:

* the size and alignment of primitive types;
* the representation of integers and floating-point values;
* the layout and padding of structures;
* pointer and address representations, where applicable;
* and the byte order used to represent multi-byte values in memory.

For example, consider an operation that exposes the individual bytes of an
integer:

```text
@fold bytes_of(0x12345678)
```

On a little-endian target, this might produce:

```text
[0x78, 0x56, 0x34, 0x12]
```

whereas on a big-endian target it would produce:

```text
[0x12, 0x34, 0x56, 0x78]
```

The compiler may itself be running on a little-endian machine while producing
code for a big-endian target. The fact that the compiler's host representation
happens to be little-endian must not affect the result of compile-time
evaluation. The evaluation is semantically an execution of the target program
and must therefore observe the target's representation.

This does not necessarily require the compiler to literally execute the target
machine's instruction set. Instead, the evaluator operates on a target-aware
representation of values and implements the observable semantics of the target.
In effect, compile-time evaluation forms a small model of the target machine
wherever those machine properties are visible to the language.

This principle also applies to elaboration. If elaboration needs to inspect,
construct, or transform a value whose representation is target-dependent, it
must do so according to the target's rules. Consequently, the result of
elaboration and constant folding must be independent of the host architecture,
provided the selected target is the same.

The host machine is therefore an implementation detail of the compiler. The
target machine determines the semantics of values that become part of the
generated program.

In this sense, cross-compilation does not merely translate source code for
another architecture. It also requires the compile-time portions of the language
to behave as though they were executing in the target environment.


### Why this is necessary

This model is required by language features such as dynamically sized `long`
integers and compile-time reflection.

For example, an `int` can have an arbitrarily large statically known range.
Operations on such values may require representations whose storage is allocated
during compilation. If the resulting value is embedded in runtime code, its
dynamically allocated representation must also be available at runtime.

Rather than introducing a special representation for values created during
compilation, Hikolang preserves the normal runtime representation and transfers
the required allocations into the executable.

This gives compile-time computation the same expressive power as ordinary
runtime code while maintaining a clear phase boundary: **the computation may
happen during compilation, but its resulting runtime state can become part of
the program**.


## Three execution environments

The resulting model can be summarized as four environments:

| Phase               | Purpose                                  | Environment |
| ------------------- | ---------------------------------------- | ----------- |
| Elaboration         | Complete type definitions                | Compiler    |
| Constant-folding    | Evaluate phase invariant expressions     | -           |
| `@fold`             | Force evaluate phase variant expressions | Compiler    |
| Translate / Runtime | Translate code for execution at runtime  | Runtime     |

The compiler may move computation from a later phase to an earlier one when the
language rules permit it. It may not, however, silently change the meaning of a
phase-variant operation by executing it in the wrong environment.

The boundary between phases is therefore not primarily about what syntax is
being executed. The same Hikolang code may participate in multiple phases. The
distinction is **when the code executes, which environment it observes, and
whether its resulting state must be transferred into the generated program**.

This makes compile-time computation a fundamental part of the language rather
than a separate metaprogramming mechanism, while keeping runtime behavior
explicit and predictable.
