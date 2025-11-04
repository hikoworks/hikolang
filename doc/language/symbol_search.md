# Symbol search

## Search order

 0. Resolve direct paths:
    - If the symbol starts with a dot `.` only look for it in the root
      namespace.
    - If the symbol starts with multiple dots `.` go up the stack of namespaces
      and look for it only there.
 1. Search for the symbol in the current scope, then in parent scopes.
 2. If inside of a member function; search for the symbol in the current type.
 3. Search in the current namespace, then in parent namespaces, including the
    root namespace.

## Method rewrite

A member function call like `foo.bar.baz(1)`, could be either a real member function `baz()`
or a callable member variable. This is why we don't directly rewrite it to a normal function.

If a member function or member variable is not found, rewrite it as a normal function, like 
into `baz(foo.bar, 1)`. Then search the symbol of the normal function in the normal
search order.

## Function Overload Set

When searching for a function all steps are executed, even if a matching
function is found early.

For each function found, find the highest scoring function based on the
arguments and return type.

How to score function signatures:
 - The first two arguments have a 1.5 multiplier.
 - A return type has a 0.5 multiplier.
 - Arguments with the exact type of the coercion has a 1.5 multipler.
 - 1,000,000,000 points for an argument that is coerced with `:=`.
 - 1,000,000 points for an argument that coerced with `:^` or `:!`.
 - 1,000 points for an argument that is coerced with `:`.
 - -1,000 points for each unused defaulted argument.
 - No points for arguments that have no coersion.

## Universal call rewrite


## Argument Depended Lookup

A single identifier function call does an aditional search for member functions
of types of each of the arguments.




## Direct namespace-path

Names that start with one or more dots `.` create a specific path so,
searching is required.
