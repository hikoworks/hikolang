# Ranged integer

The basic integer used in the language is a ranged integer.

```
struct int[range : z_interval];
```

## Builtin

The builtin `int` types match the register sizes of the CPU.
The maximum register size is the size of the limb-register, the limb-register
is the register size which is used for arbirtrary sized integer math on a CPU.

These `int` types match C, C++ types:

  | cstdint    | full                  | `.lo`        | `.hi`        |
  |:---------- |:--------------------- |:------------ |:------------ |
  | `int8_t`   | `int[-128..=127]`     | -128 ..< 0   | -128 ..< 256 |
  | `uint8_t`  | `int[0..=255]`        |    0 ..< 256 |    0 ..< 256 |
  | `int16_t`  | `int[-32768..=32767]` | 
  | `uint16_t` | `int[0..=65535]`      |
  | `int32_t`  | `int[-2³¹..=2³¹-1]`   | -2³¹ ..< 0   | -2³¹ ..< 2³¹ |
  | `uint32_t` | `int[0..=2³²-1]`      |    0 ..< 2³² |    0 ..< 2³² |
  | `int64_t`  | `int[-2⁶³..=2⁶³-1]`   | -2⁶³ ..< 0   | -2⁶³ ..< 2⁶³ |
  | `uint64_t` | `int[0..=2⁶⁴-1]`      |    0 ..< 2⁶⁴ |    0 ..< 2⁶⁴ |


syntax unit si prefix Q 10³⁰
syntax unit si prefix R 10²⁷
syntax unit si prefix Y 10²⁴
syntax unit si prefix Z 10²¹
syntax unit si prefix E 10¹⁸
syntax unit si prefix P 10¹⁵
syntax unit si prefix T 10¹²
syntax unit si prefix G 10⁹
syntax unit si prefix M 10⁶
syntax unit si prefix k 10³
syntax unit si prefix h 10²
syntax unit si prefix da 10¹
syntax unit si prefix d 10⁻¹

## Standard Library

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