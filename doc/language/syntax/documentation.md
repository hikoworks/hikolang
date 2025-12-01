# documentation

## Syntax

`/**` _text_ `*/`

_text_ := _see below_

## Semantics

### Markdown & Doxygen

The text is interpreted as being formatted as a combination of Markdown
and Doxygen. 

### Left margin

Because markdown requires the text to be aligned to the left we need to
determine the left margin of the documentation.

 - Skip the line after the leading `/**`,
 - Find the next line containing text.
 - The left-margin is the first character of the text, exluding leading
   white-space and stars `*`.

If a line starts earlier than this left-margin, it is as-if this line does start
at the left-margin. Text that starts later is indented from the point of view of
markdown.

Once the left margin is calculated it will be used to ignore that many leading
whitespace and stars `*` on each line. This way stars `*` used for unnumbered
lists still work correctly.

### First Paragraph

The first paragraph of text is used as the title, as if it was prefixed with
the `@brief` command. The first paragraph does not need to appear directly
after the opening `/**`. You may also explicitly use the `@brief` command.

### Commands

#### @brief

Set the title of the documentation.

#### @pre

`@pre` *description*

*description* := The description of the pre-condition.

#### @post

`@post` *description*

*description* := The description of the post-condition.

#### @param

`@param` *value_type*__?__ `$`__?__ *name* *description* 

*value_type* := `[` `in` __|__ `out` __|__ `in,out` `]`

*name* := The parameter name

*description* := The description of the parameter.

If the *name* is prefixed with `$` then this describes a hidden context
argument. If it is documented then the functions that call the documented
function automatically inherit the documentation of this hidden context
argument.

#### @return

`@return` *description*

*description* := The description of the return value

#### @retval

`@retval` *value* *description*

*value* := A returned value

*description* := The description of the returned value

#### @throw

`@throw` *error* *description*

*error* := The returned error-name

*description* := The reason the error could be thrown.

#### @trap

`@trap` *error* *description*

*error* := The returned trap-name

*description* := The reason the error was trapped.