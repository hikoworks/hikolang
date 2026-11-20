# annotation

[_argument-list_]: argument_chain.md
[_code-block_]: code_block.md
[_call-operator_]: call_operator.md
[_documentation_]: documentation.md
[_expression_]: expression.md
[_function_definition_]: function_definition.md
[_fqname_]: fqname.md
[_type_definition_]: type_definition.md

## Syntax

_attribute_ := `@` [_fqname_] __(__ `(` [_argument-list_] `)` __)?__



## Semantics



## Attributes

 * F - Function
 * T - Type
 * V - Variable
 * E - Expression
 * M - Member
 * B - Code Block

| Attribute                  | On      | Purpose                                           
|----------------------------|-------  |---------------------------------------------------
| `@abi(x)`                  | F T     | The ABI used.
| `@alignas(size)`           | T V     | Alignment of data.
| `@auto_rethrow`            | M       | Synthesize catch/rethrow for `std.error_code`.
| `@call`                    | E       | Emit call instruction, disabling inlining.
| `@datastack(size)`         | F       | Enable data-stack.
| `@deprecated(msg)`         | F M T V | Mark as deprecated.
| `@discard`                 | F       | The returned value may be ignored.
| `@domain(domain, idx)`     | M       | Add primary unit to a domain for `std.unit`.
| `@effect(effect, ...)`     | B F     | Add, remove or check effect.
| `@export(abi)`             | F V     | Export into library / executable.
| `@fast`                    | B       | The code that has priority.
| `@fold`                    | E       | Execute expression at compile time.
| `@implies(effect, ...)`    | M       | Imply another effect for `std.effect`.
| `@keyword`                 | M       | Add enum name as literal keyword.
| `@lean`                    | B       | Reduce code size and register presure.
| `@merge`                   | T       | Merge type definition together.
| `@metatype`                | F       | A function, that turns a definition into a type.
| `@left(prio [, op])`       | M       | Add left associate operator in `std.operator`.
| `@no_return`               | F       | This function does not return.
| `@optional`                | F       | Disable pre/post/assert condition checks at run-time.
| `@prefix(prio, op)`        | M       | Add prefix operator in `std.operator`.
| `@public`                  | F M T V | The name of the object is available outside of the package.
| `@right(prio, op)`         | M       | Add right associate operator in `std.operator`.
| `@shared(allocator)`       | V       | A variable whose storage in a reference counted heap allocation.
| `@section(link-name)`      | V F     | The object is located in a specific link-section.
| `@suffix(prio, op)`        | M       | Add suffix operator in `std.operator`.
| `@tag`                     | M       | Add a tag for `std.unit`.
| `@thread_local`            | V       | A variable that is located in thread-local storage.
| `@type`                    | M       | The member is part of the type, not its instance.
| `@unique(allocator)`       | V       | A variable whose storage is allocated on the heap.
| `@unit(unit-expr)`         | M       | Add a unit for `std.unit`.
| `@weak`                    | V       | A variable which is an optional reference to `@shared(...)` storage.


### @abi(x)

On: [_function_definition_], [_type_definition_]

On a type this will change how the memory layout is for an object.
 
 abi         | Description
 :---------- | :---------
 `c`         | Use the C calling convention.


### @alignas(integer-expression)

On: [_variable-definition_], [_type-definition_]

Make the allocation for this objects aligned to the given expression.


#### @auto_rethrow

On: enum-name(std.error)

The compiler will synthesize a catch-rethrow for any
flow-control-expression that does not explicitly catch this error.


### @call

On: [_calloperator_]

Explicitly emit a `call`-like instruction and do not inline.


### @datastack(size)

On: [_function_definition_](function_definition.md)

When this modifier is applied to a function definition, the function executes
using a separate data stack alongside the native call stack. The annotation
specifies the amount of virtual address space reserved for the data stack when
executing that function and its call tree.

The data-stack requirement is determined by the function's call graph. The
annotated function establishes the reservation for its execution and the
functions it calls, so the programmer must ensure that `size` is sufficient
for the maximum data-stack usage anywhere in that call tree.

If no function in the program's call trees are annotated with
`@datastack(size)`, the program uses the traditional shared native call stack
for local data.

There are potential performance implications, although their magnitude depends
on the workload and is difficult to quantify:

 * **Startup overhead:** Reserving virtual address space for data stacks may
   add some startup overhead.
 * **Instruction overhead:** Managing a separate data stack may require
   additional instructions for frame allocation, restoration, and bounds
   checking.
 * **Memory locality:** Using a separate data stack may affect cache behavior
   and memory locality, depending on allocation patterns and workload.


### @deprecated(message)

On: [_variable-definition_], [_function-definition_], [_type-definition_]

This variable/function is deprecated. The compiler will emit a warning message
where the variable is used, including the `message` passed in the attribute.


### @discard

On: [_function_definition_]

The function's return value may be discarded.


### @domain(domain, index)

On: enum-name(std.unit)

 - domain: A `std.unit` to be used as a domain,
 - index: An exponation index inside this domain.


### @effect([_effect-list_])

On: [_function_definition_], [_code_block_]

Add, remove or assert the existance or non-existance of effects.

 op     | description
 :----- |:----------------------------------------------------
 `+foo` | Add the effect `foo`.
 `-foo` | Remove the effect `foo`.
 `=foo` | Assert `foo` is in effect.
 `!foo` | Assert `foo` is NOT IN effect.

Functions that call a function that has an effect, inherits that effect.
An effect like this can be removed using the `-` [_id_] syntax.

The default set of effects can be found here [_enum-definition_]. You
can also modify the `.std.effect` enum to add more effects.

If a pre-condition with `@effect(+foo)` can not be proven to be `true` during
compile time, then the effect `foo` is attached to the function. This is used
most often with `unsafe`.


### @export(abi)

On: [_variable-definition_], [_function-definition_]

The function will be available in the executable using the `abi`
specified.

  abi      | Description
 :-------- |:---------------
  `"c"`    | Use the C ABI.
  `"c++"`  | Use the C++ ABI.


### @fast

On: [_code-block_]

Used on a [_code-block_] of a flow-control clause, indicating how this branch
is optimized. When using `@fast`    you are asking the compiler to make this
branch fast, even if other branches would be impacted negatively.

This will:
 * Order code to reduce latency for this fast-path.
 * Inline code as much as possible. (This is the default.)

> [!note]
> In most cases `@fast`    will have very little impact on performance.
> Instead look for `@lean`    for a much larger impact.


### @fold

On: [_call-operator_]

Force a function call to be evaluated (constant-folded) during the
translation phase of the compiler; even if the function is annotated
with `@with_effect(+phase_variant)`.

> [!note]
> Normally any function annotated with `@with_effect(+phase_variant)` is
> translated so that it is executed during runtime.

All arguments passed to a `@fold` call must already be available as
compile-time values. An error is raised if an argument requires
compile-time evaluation that has not otherwise been requested or
performed (by guaranteed constant-folding).

> [!note]
> The `@fold` annotation does not propagate to argument
> dependencies; it only forces evaluation of the annotated function call
> itself.


### @implies(effect, ...)

On: enum-name(std.effect)

.std.effect = enum {
    foo
    @implies(foo) bar
}

Following the example above; when the annotation `@effect(+bar)` is used,
it is as-if `@effect(+foo)` is also implied.


### @keyword

On: enum-name

The enum name is added as a keyword in the language, becoming a literal for
value of that enum-type.


### @lean

On: [_code-block_]

Used on a [_code-block_] of a flow-control clause, indicating how this branch
is optimized. When using `@lean`    you are asking the compiler to produce
code that impact the other branches positively.

This will:
 * Call functions rather than inline functions.
 * Reduce code-size.
 * Reduce register pressure.


### @metatype

On: [_function-definition_]

This function's [_fqname_] is added to type-definition keyword list.

This function is called when a type-definition is found. The arguments
to this functions are:

 * The list of type attributes
 * The template argument list
 * The type-inheritance list
 * The [_code-block_]

It returns a function that in-turn returns an actual type, which is
added to the overload-set as a type template.


### @left(precedence [, operator])

On: enum-name(std.operator)

Left associative

 - precedence: An integer priority, lower numbers bind closer.
 - operator: The name or syntax-pattern of the operator.


### @no_return

On: [_function_definition_]

This function will not return, used for functions like `std.terminate()`.
Meaning code after this function call will never execute.


### @prefix(precedence, operator)

On: enum-name(std.operator)

Prefix operator
 - precedence: An integer priority, lower numbers bind closer.
 - operator: The name or syntax-pattern of the operator.


### @public

On: [_variable-definition_], [_function-definition_], [_type-definition_]

This variable can be named outside of the module.


### @right(precedence, operator)

On: enum-name(std.operator)

Right associative

 - precedence: An integer priority, lower numbers bind closer.
 - operator: The name or syntax-pattern of the operator.


### @shared(allocator)

On: [_variable-definition_]

Allocate the object on the heap, and make it sharable to other shared variables
using reference counting.


### @section(link-name)

On: [_variable-definition_]

In case of a global variable place it in a specific section of the executable.


### @static

On: [_variable-definition_], [_stack-definition_]

This function is a class memember function, instead of instance member function.


### @suffix(precedence, operator)

On: enum-name(std.operator)

Suffix operator

 - precedence: An integer priority, lower numbers bind closer.
 - operator: The name or syntax-pattern of the operator.



### @tag

On: enum-name(std.unit)

`@tag` declares a unit whose domain contains exactly one dimension and whose
conversion factor is 1. The resulting unit behaves algebraically like any other
unit, but is intended for nominal type tagging rather than physical measurement.

A `@tag` does not introduce a scaling factor like `@unit()` does. A tagged value
therefore retains its underlying scalar type and may be used where its
non-tagged scalar type is expected. In such cases, the tag is implicitly dropped
from the type.


### @thread_local

On: [_variable-definition_]

Allocate the variable in the thread-local region.


### @unique(allocator)

On: [_variable-definition_]

Allocate the object on the heap, only one variable can own this object, but the
ownership can be moved to another variable


### @unit(unit-expression)

On: enum-name(std.unit)

The [_unit-expression_] for conversion to the new unit.


### @weak

On: [_variable-definition_]

A variable that references a shared variable. This variable becomes empty when
the shared variable's reference count becomes zero.










