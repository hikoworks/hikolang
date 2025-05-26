# hic.exe

## Synopsis

```bash
hic [options] <command>
```

## Options

Possible commands are:
 - `build`: Compile the specified source files.
 - `test`: Run tests defined in the source files.
 - `clean`: Clean up generated files.
 - `version`: Show the version of the compiler.
 - `help`: Show help information for the specified command.

 General options:
 - `--build-type=<type>[,<type>]`: Specify the build type (e.g., `debug`, `release`).
 - `--optimize`: Enable optimizations during compilation.
 - `--path=<path>[;<path>]`: Specify paths to search for imported modules.


## Description
hic is the compiler for the hikolang programming language.

The compiler can compile multiple programs in a single command,
which can reduce the time it takes to compile multiple files.
