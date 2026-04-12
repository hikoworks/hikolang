# String


## Status

 - No encoding
 - WTF-8 encoded
 - UTF-8 encoded
 - UTF-8 encoded / NFC normalized

## Conversions

### string.wtf8_from_wtf16(other: string, endian: endian_t)

Convert a WTF-16 encoded string into WTF-8, this is a lossless conversion
used for encoding file names.

### string.utf8_from_bytes(other: string, encoding: char_encoding)

### utf8_validate(ref self: string, fallback: bool, return: bool)

Inplace validate and fix the UTF-8 encoded string. Invalid UTF-8 code-units
are converted to the U+fe0d code-points, this may cause the string to become larger.
If `fallback` is true, invalid UTF-8 code-units use the iso-8859-1 code-page fallback.
