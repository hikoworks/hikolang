# Unified functions


 - Function
 - Function through a pointer
 - Function on a type
 - Function on a type through a vtable
 - Lambda
 - Generator
 - Task


```

 +=============+
 | instance    |
 |             |
 | +=========+ |
 | | vptr    | |
 | +=========+ |
 +====== | ====+
         |
         V
       +==============+
       | vtable       |
       |              |
       | +========+   |     +-----------+        +----------+
       | |  fptr  |-------->| signature |<-------| template |
       | +========+   |     +-----------+        +----------+
       +===== | ======+                                :
              |                                        :
              |                                        :
              V                                        :
         +========+          generator                 :
         |  code  |<....................................
         +========+
```


## A function type.

## A function value.

 - Like a function pointer its value can change to a different implementation.
 - Like the instance of a lambda, containing captured variables.
 - 

```
// A function definition.
// The definition creates code for each combination of arguments (in this case 1).
// A function definition results in a `function_meta` type where different definitions can be added to.
var foo += fn (a: int, b: int) -> int {
     return a + b
}

// A function definition which add a overload to the same `foo : function_meta` value.
var foo += fn (a: float, b) -> int {
     return a + b;
}

// Syntactic sugar.
// Return type of overloads may be different. 
fn foo(a: string, b) -> string {
     return a + to(string, b);
}

// Function specialization
// Using a literal instead of a name.
fn foo(10: int, b: int) -> int
{
     return 100 + b;
}

// Function specialization
// Using an expression instead of a name.
let my_a = 20;
fn foo((my_a): int, b: int) -> int
{
     return 200 + b;
}

// A function declaration.
// Can be used as a function pointer.
// Pre and post condition are part of the declaration.
var bar_type = fn (a: int, b: int) -> int pre(a >= 0);

// Assign the function pointer from `foo(a: int, b: int) -> int`.
// The `bar_type` will extract the specific function pointer for
// the prototype.
var bar : bar_type = foo;

// Assign a function to a variable.
var bar = foo;

// Assign a different function
bar = baz;
```



// A normal function template (only 1 instance possible)
fn foo(a: int, b: int) -> int
{
     return a + b
}

// Overloaded function template (multiple instances)
fn foo(a: int, b)
{
     return a + b
}


```

```
// Function sugar

// Translated to
var foo = {(a: int, b: int) -> int
     |
     return a + b
}

// Translated to
var foo_type = struct {
};

fn 
```