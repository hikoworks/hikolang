# integer-literal

## Syntax

\[+-\]__?__ \[0-9\]__+__ __|__\
\[+-\]__?__ `0` \[bB\] \[0-1\]__+__ __|__\
\[+-\]__?__ `0` \[oO\] \[0-7\]__+__ __|__\
\[+-\]__?__ `0` \[dD\] \[0-9\]__+__ __|__\
\[+-\]__?__ `0` \[xX\] \[0-9a-fA-F\]__+__ __|__

## Semantics
An integer literal is a constant value that is directly represented in the source code.

The number without a prefix is a decimal integer. The prefix `0b` or `0B`
indicates a binary integer. The prefix `0o` or `0O` indicates an octal integer.
The prefix `0d` or `0D` indicates a decimal integer. The prefix `0x` or `0X`
indicates a hexadecimal integer.

The length of the integer literal is unlimited[1], and is passed as a string to
a literal conversion function. The conversion function is responsible for
converting the string to the appropriate integer type. The conversion function
is not specified in this document, but it is expected to be a standard library
function.

