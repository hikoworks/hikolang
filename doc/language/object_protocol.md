# Object Protocol

## Special type members

 Member                   | Description
 :------------------------ |:----------------------------------------------------
  `::frozen`               | If the object is frozen
  `::__alloc_size()`       | The number of bytes for holding a value of the type.
  `::__alloc_alignment()`  | Alignment of the member to allocate this type.


### ::__alloc_size(), ::__alloc_alignment()

Every type has these function so that the langauge can reserve the space for
values of this type.


### ::frozen

A boolean member variable, that is only available during compile time, if the
value is frozen. This member is not part of the value but is tracked by the
compiler separately.

Its primary usage is by the compiler to freeze a value/type when it is used,
and for `__merge__()` to change how it merges types while frozen. Certain
types such as an enum allow members to be inserted after freezen if the
index-value has not yet been used and fits in the interval of the underlying
integer.
