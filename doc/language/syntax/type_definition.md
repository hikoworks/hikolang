# type-definition

[_attributes_]: attributes.md
[_argument-declaration-list_]: argument_declaration_list.md
[_code-block_]: code_block.md
[_fqname_]: fqname.md
[_identifier_]: identifier.md
[_underlying_]: underlying.md
[_function-definition_]: function_definition.md

## Syntax

_type_definition_ :=\
      _meta_type_ __(__ `:` [_underlying_] __)?__ [_code-block_]\
    __|__ _meta_type_ `(` [_argument_declaration_list_](argument_declaration_list.md) `)` __(__ `:` [_underlying_] __)?__ [_code-block_]

_meta_type_ := [_fqname_](fqname.md)

_underlying_ := [_fqname_] __|__ `(` [_type-list_] `)`

## Semantics

A function with the `meta_type` attribute will add the name of the function
as a keyword that can be used to introduce a type definition. The keyword
in this case is a fully qualified name, which is searched in the same way
normal functions are searched. See
about the `meta_type` attribute.


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

## Annotation

### [[abi(x)]]

On a type this will change how the memory layout is for an object.
 
 abi         | Description
 :---------- | :---------
 `c`         | Use the C ABI, keeping members aligned and in original order.
 `reorder`   | Reorder members from large to small struct, maintaining alignment. (default)
 `pack`      | Keep members in original order, but ignore alignment. Access to members is valid.
 `compress`  | Use niche-mask to further compress members in a struct. Access to members may require shifts and masks. 
