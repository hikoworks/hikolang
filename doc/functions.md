# Functions

## Function definition

```
function foo(x: int, y: int[1, 5], z = 5)
    pre(x > 0)
    post(r : r > 0)
    -> int
{
    return x + y + z;
}

```