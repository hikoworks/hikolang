# Object Protocol

## Special type members

 Member            | Description
 :---------------- |:----------------------------------------------------
  `::size()`       | The number of bytes for holding a value of the type.
  `::alignment()`  | Alignment of the member to allocate this type.
  `::stride()`     | The distance between elements in memory.



### ::size(), ::alignment(), ::stride()

Every type has these function so that the langauge can reserve the space for values
of this type.

