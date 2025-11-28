# documentation

## Syntax

`/**` _text_ `*/`

_text_ := _see below_

## Semantics

### Left margin

Because markdown requires the text to be aligned to the left we need to determine
the left margin of the documentation.

 - Skip the line after the leading `/**`,
 - Find the next line containing text.
 - The left-margin is the first character of the text, exluding leading white-space and stars `*`.

If a line starts earlier than this left-margin, it is as-if this line does start at the left-margin.
Text that starts later is indented from the point of view of markdown.


### Paragraphs

Like markdown paragraphs are separated 
