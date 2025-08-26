# string

## Big Endian layout

```
struct string {
    u64 _data
    u64 _size
    u64 _capacity
};

fn is_short(let self := string) -> bool {
    return (self._capacity & 1) == 0
}

fn capacity(let self := string) -> u64 {
    if (self.is_short()) {
        return 23
    } else {
        return self._capacity >> 1
    }
}

fn size(let self := string) -> u64 {
    if (self.is_short()) {
        return 23 - (self._capacity >> 1)
    } else {
        return self._size
    }
}

fn data(let self := string) -> ptr[char] {
    if (self.is_short()) {
        return std.address_of(char, self)
    } else {
        return self._ptr
    }
}
```
