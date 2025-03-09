# Function

```bnf
function := 'func' name '(' argument? ( ',' argument )* ','? ')' ( '->' type )?
argument := argument_with_type | argument_with_value
argument_with_type := name ':' type_expression qualifier? ( '=' value_expression )?
argument_with_value := value_expression
qualifiers := "in" | "out" | "inout" | "copy"
```

## Qualifiers

 * in : value is passed by constant reference or by constant copy. (this is the default).
 * out : value is passed by mutable reference, write only.
 * inout : value is passed by mutable reference, read/write.
 * copy : value is copied by-value and is mutable.


