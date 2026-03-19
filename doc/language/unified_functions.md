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

a <- i8(4)
b <- i8(6)
a <-> b
a <- b

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
