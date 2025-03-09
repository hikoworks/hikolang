


 * By default a type can be copied and moved using a memcpy.
 * If a type has a user-defined destructor then a value can only be moved using memcpy.
 * If a type has a user-defined copy constructor/assignment then a value can be copied using those functions.

