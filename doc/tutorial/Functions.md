Tutorial - Functions
====================



```
fn foo(a, b) {
    return fn(c, d) {
        return a + b + c + d
    }
}

fn foo(a) {
    return fn(b, c, d) {
        return a + b + c + d
    }
}

foo(1, 2; 3, 4)
foo(1, 2)(3, 4)

foo(1; 2, 3, 4)
foo(1)(2, 3, 4)
```


```
class vector[T : type] {
    fn __init__(size : int) {}
};

var b = vector[int](5)
```

```
class foo_t[] {
    fn __init__(size : int) {}
}

struct bar_t[] {
    fn __init__(size : int) {}
}

var c = foo_t[](5)
var d = foo_t(5)
```

```
fn class(d : definition) {
    for (ref mf in d.member_functions()) {
        if (not (mf.is_public() or mf.is_private())) {
            mf.set_private()
        }
    }

    return d
}

fn struct(d : definition) {
    for (ref mf in d.member_functions()) {
        if (not (mf.is_public() or mf.is_private())) {
            mf.set_public()
        }
    }

    for (view em in d.enum_members()) {
        syntax_error(view.source_location(), "Enum member {} not valid in a struct", em.name())
    }

    return d
}

fn foo_t[] {
    return class {
        fn __init__(size : int) {}
    }
}

var c = foo_t[](5)
var d = foo_t(5)
```