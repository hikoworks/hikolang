# Generator functions

A generator function yields multiple values. It can be used as the range
expression of range based for-loop. Or the generator can be iterated manually.

If a function has one or more `yield` statement then that function is a
generator-function. A generator function returns an generate-object when called.

The generate-object has the following member functions:
 - `next()` Continue the generator and get the `yield` value. Throws
   a `stop_iteration` error when the generator returns. Throws other errors
   when thrown from the generator.

A generator function is immediately paused when it is called.

When the `next()` function is called on the generate-object the function is
continued up to and including the `yield` statement, the expression-value of
the `yield` statement is returned from `next()` and the generator function is
paused again.

If generator function returns, the `stop_iteration` error is thrown from the
`next()` function, a generator function can not `return` a value.

If a generator function `throw` or `trap` an error then the `next()` function
will throw/trap that error.

After any error the generator's function's frame is destroyed and any `next()`
call will result in `destroyed` trap.
