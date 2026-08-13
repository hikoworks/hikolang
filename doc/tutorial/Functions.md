Tutorial - Functions
====================



```
foo = fn(a, b) {
    return fn(c, d) {
        return a + b + c + d
    }
}

foo = foo(a) {
    return fn(b, c, d) {
        return a + b + c + d
    }
}

foo(1, 2)(3, 4)
foo(1)(2, 3, 4)
```


```
vector = class[T : type] {
    __init__ = fn(size : int) {}
};

b := vector[int[0..<100]]
b.append(1)
```

```
foo_t class[] {
    __init__ = fn(size : int) {}
}

bar_t struct {
    __init__(size : int) {}
}

var c = foo_t[](5)
var d = bar_t(5)
```

```
@metatype
class = fn(p : parameters, d : definition) {
    for (mf :& in d.member_functions()) {
        if (not (mf.is_public() or mf.is_private())) {
            mf.set_private()
        }
    }

    return d
}

@metatype
struct = fn(p : parameters, d : definition) {
    for (mf :& in d.member_functions()) {
        if (not (mf.is_public() or mf.is_private())) {
            mf.set_public()
        }
    }

    for (em :& in d.enum_members()) {
        syntax_error(view.source_location(), "Enum member {} not valid in a struct", em.name())
    }

    return d
}

```