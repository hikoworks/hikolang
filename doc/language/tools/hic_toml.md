# hic.toml

The `hic.toml` file is a configuration file used to specify the dependencies of a Hic project or package. It is written in TOML (Tom's Obvious, Minimal Language) format.

## Section Structure

### \[package\]
The `[package]` section is where you define the metadata for your Hic package. This includes the name, version, and other details that describe the package.

```toml
[package]
name = "com.example.my_package"
description = "A sample Hic package"
version = "1.0.0"
```

#### name
The `name` field specifies the name of the package. It should be a valid module name, which typically follows the format `com.example.my_package`; reverse domain names are commonly used to avoid naming conflicts.

All modules-names in the package must have the prefix of the package name.

### \[dependencies\]

The `[dependencies]` section is where you define the external packages that your Hic project depends on. Each dependency is specified with a key-value pair, where the key is the name of the package and the value is an object containing the details of the dependency.

```toml
[dependencies]
com.example.foo = { git = "https://github.com/example/foo.git", branch = "v1.0" }
com.example.baz = { url = "https://example.com/baz-2.3.zip"}
com.example.qux = { path = "./local-qux" }
com.example.bar = {}
```

