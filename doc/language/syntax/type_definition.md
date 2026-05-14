# type-definition

## Syntax

_type_definition_ :=\
      `type` _meta_type_ _name_ __(__ `[` [_argument_declaration_list_](argument_declaration_list.md) `]` __)?__ _underlying_**?** [_attributes_]__*__ `{` [_block_](block.md) `}`\
    __|__ `type` _meta_type_ __(__ `[` [_argument_declaration_list_](argument_declaration_list.md) `]` __)?__ _underlying_**?** [_attributes_]__*__ `{` [_block_](block.md) `}`

_underlying_ := `:` [_type_expression_](type_expression.md) __|__ `(` [_type_list_](type_list.md) `)`

_name_ := [_identifier_](identifier.md)

_meta_type_ := [_identifier_](identifier.md)

## Semantics

This type definition calls a _meta-type_ function like:


### abi(x)

On a type this will change how the memory layout is for an object.

 - "c": Use the C ABI. (same as aligned)
 - "c++": Use the C++ ABI. (same as aligned)
 - "aligned": Keep members in a struct in given order, and align members.
 - "reorder": Reorder members in a struct, pack while maintaining alignment. (default)
 - "pack": Keep members in a struct in given order, ignore natural alignment of members. This will
           cause member access to use possible slower, but valid, unaligned access.
 - "compress": Use niche-mask to further compress members in a struct. Access to members may
               require shifts and masks. 




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

[_fqname_]: fqname.md
[_identifier_]: identifier.md
[_attributes_]: attributes.md
[_argument-declaration-list_]: argument_declaration_list.md
