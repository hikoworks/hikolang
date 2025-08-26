# Ranged integer

The basic integer used in the language.

```
struct int[range : interval[long]] {
    let N = make_number_of_digits(range, register_type)
    let is_signed = range.low < 0
    var _digits : array[register_type, N]
}
```

```
fn foo(x)
{
    return x * 2
}

var my_type = array[long, int[10..=12]];
var foo = my_type();

fn bar(x : int[5..=6]) {
    foo[x * 2] = 42
}

var my_enum = enum {
    red, green, blue
};

var my_color_table_type = my_type
my_color_table_type.index_type = my_enum
var my_color_table = my_color_table_type();
my_color_table[red] = 5


```