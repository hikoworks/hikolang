# Modules

Each source file with the `.hkm` extension is a module. The first line in a
source file is the [_module-declaration_](syntax/module_declaration.md).

An example of some module declarations:

```
module com.example.test.settings if x86
module com.example.test package 1.0.0
module com.example.app program "foo"
```

Each module has a unique fully qualified name, the names of modules form
a tree structure. The sub-name on the left of the name go toward the root of
the tree.