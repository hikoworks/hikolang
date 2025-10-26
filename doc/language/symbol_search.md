# Symbol search

## Order

 0. Resolve direct path:
    - If the symbol starts with a dot `.` only look for it in the root
      namespace.
    - If the symbol starts with multiple dots `.` go up the stack of namespaces
      and look for it only there.
 1. Search for the symbol in the current scope, then in parent scopes.
 2. If inside of a member function; search for the symbol in the current type.
 3. Search in the current namespace, then in parent namespaces, including the
    root namespace.
 4. If the symbol is a function call: find a member function in the type of each
    argument and return type.
 5. Rewrite a member function call to a normal function call, i.e.:
    `foo.bar.baz(1)` into `baz(foo.bar, 1)` then restart the search.


## Function Overload Set

When searching for a function all steps are executed, even if a matching
function is found early.

For each function found, find the highest scoring function based on the
arguments and return type.

How to score function signatures:
 - 

## Universal call rewrite


## Argument Depended Lookup

A single identifier function call does an aditional search for member functions
of types of each of the arguments.




## Direct namespace-path

Names that start with one or more dots `.` create a specific path so,
searching is required.
