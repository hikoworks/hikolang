# Symbol search



## Order

 1. Direct namespace-path
 1. Search in the current local scope, then in parent scopes.
 1. Search at file level
 1. Argument Depended Lookup
 1. Search the current namespace, then in parent scopes.


## Function Overload Set

When searching a function call, at each step in the order above,
collect each matching function.



## Universal call rewrite


## Argument Depended Lookup

A single identifier function call does an aditional search for member functions
of types of each of the arguments.




## Direct namespace-path

Names that start with one or more dots `.` create a specific path so,
searching is required.
