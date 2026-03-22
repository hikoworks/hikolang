# Unified functions




```
foo <-| struct(T : type)
     inherit_from(bar, baz)
{
     _list : list[T];

     fn push_back(x : T) {
          _list.insert(len(list), x)
     }
};

i8[] = int[0..=255]

a : const i8 := 5 // int[5..=5]
a :: const ref i8 := c
a :^ foo := c
a :! foo := c

a[T : type] : T = T(5)

a := i8[](5)
a := c :: i8
a := c :^ i8
a := c :! i8

A := struct {
     foo := fn()
     {
          return ref a
     }

     a = 5
}

b := A()
print(b.foo) // 5
b.foo = 3

b := i8(6)
a := b

d := 5
d += 1

(a, b) := (b, a)

a = 12
a = 24 // error

incr(a, 4)

// constant
bar = i8(5)
// bar <- i8(5)
// seal bar

// variable
qux <- i8(10)

a <=> b

<->

```
