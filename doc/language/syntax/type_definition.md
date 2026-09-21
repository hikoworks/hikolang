# type-definition

[_attributes_]: attributes.md
[_argument-declaration-list_]: argument_declaration_list.md
[_code-block_]: code_block.md
[_fqname_]: fqname.md
[_identifier_]: identifier.md
[_underlying_]: underlying.md

## Syntax

_type_definition_ :=
    _meta_type_ [_underlying_]__?__ [_code-block_]


_meta_type_ := [_fqname_](fqname.md)


## Semantics

A function with the `meta_type` attribute will add the name of the function
as a keyword that can be used to introduce a type definition. The keyword
in this case is a fully qualified name, which is searched in the same way
normal functions are searched. See [_function-definition_](function_definition.md)
about the `meta_type` attribute.

The following meta-types are availble in the language:
 - `enum`: Creates a type that as a value is one of a set of names. Names can
   be added to the enum at any point of the program, even when the type
   becomes frozen.
 - `variant`: Creates a type that as value is one of a set of name that can also
   hold payload of a set of values.
 - `struct`: Creates a type that is a set of value members.




```
fn struct(template_arguments, inheritence_list, attributes, members)
{
    var r = __builtin_function__(template_arguments)

    r.append({{ r = __builtin_type__() }})

    for (view type : inheritence_list) {
        r.append({{ r.add_base_type(${type}) }})
    }

    for (view member : members) {
        if (member is __builtin_variable_definition__) {
            r.append({{ r.add_member_variable(${member}) }})

            if ("property" in member.attributes) {
                syntax_assert(member.name[0] == '_', "Property {} must start with underscore", member.name)
                let member_name = __builtin_identifier__(member.name)
                let getter_name = __builtin_identifier__(member.name[1..<])
                let getter = {{ fn ${getter_name}() { return ${member_name} } }}
                r.append({{ r.add_member_function(${getter}) }})
            }

        } else if (member is __builtin_function_definition__) {
            r.append({{ r.add_member_function(${member}) }})

        } else {
            syntax_error("Unexpected member {} in struct", member.name)
        }
    }

    r.append({{ return r }})
    return r
}
```

