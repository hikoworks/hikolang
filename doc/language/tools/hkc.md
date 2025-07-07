# hic.exe

## Synopsis

```bash
hkc [options]
hkc [options] <host-repository>
```

## Options

At least one of the following commands must be specified:
 - `--version`: Show the version of the compiler.
 - `--help`: Show help information for the specified command. Including another
             command together with `--help` will show help for those commands.
 - `--list`: List all:
   * repositories that are cloned,
   * their packages with their versions and if they are active/omitted,
   * any applications that will be built,
   * which libraries they link against.
   * And a list of any dependencies that are not met.
 - `--clone`: Clone the specified remote repositories.
              Many commands will automatically clone the required repositories.
 - `--build`: Compile the specified source files.
 - `--test`: Run tests defined in the source files.
 - `--clean`: Clean up generated files.
 - `--sign`: Sign the compiled applications with a digital signature.
 - `--distribute`: Package the compiled applications into a distributable format.

General options:
 - `--build-type=<type>[,<type>]`: Specify the build type (e.g., `debug`, `release`).
 - `-O <level>`, `--optimize=<level>`: Enable optimizations during compilation.
   * `0`: No optimizations.
   * `1`: Basic optimizations.
   * `2`: More aggressive optimizations.
   * `s`: Optimize for size.
 - `--repository=<path>[:<path>]`: Specify local repositories to include in the
   build. Packages in these repositories will override packages in remote
   repositories, ignoring version numbers. Multiple `--repository` options may
   be specified.
 - `--test-recursive`: Run tests recursively in all cloned repositories.
 - `--build-recursive`: Compile all applications recursively in all cloned repositories.
 - `--fetch`: Fetch the latest changes from remote repositories before building.
   Local repositories are always up to date. `.zip` files are not downloaded
   again if they are already downloaded.
 - `-L <path>[:<path>]`, `--library-path=<path>[:<path>]`: Specify a path to
   search for _ffi_ (Foreign Function Interface) libraries. The libraries
   to link against must be specified in the source code using the
   `library` declaration.

Environment variables:
 - `HKREPOSITORY`: Same as `--repository`, but can be set in the environment.
   Repositories are additive with the `--repository` option.

## Description
