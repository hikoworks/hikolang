Tutorial - Conversions
======================

To be able to use these ranged integers it is required to convert them,
here is an example:

```
var sum = 0 : int[0..=100]      // 0 := int[0..=0], sum := int[0..=100]
for (let a in 0..<10) {         // a := int[0..=9]
    try sum += a
}
repr(sum)
```

`0` in this case is a `int[0..=0]`, it would not be possible to add anything
to this integer. This is why it is converted to a `int[0..=100]` so we can
add upto a value of 100.



