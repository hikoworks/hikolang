# File Organization

This document outlines the organization of files in the project.

## Repository
A repository is a directory hierarchy containing `*.hkm` files, which are the
source files. The hierarchy is completely free-form, and the developer is
free to organize the files as they see fit.

The compiler will scan the directory hierarchy for every `*.hkm` file, but
will ignore files and directories that start with a dot `.`.

`*.hkm` files are of three different types:
 - **Application**: A file that declares and contains code for an executable
   program.
 - **Package**: A file that declares an anchoring point for a module hierarchy.
   And allows code to be imported by another repository. 
 - **Module**: A file that declares a module that contains code that can be
   used by other modules, applications, and packages.

Each repository must have at least one `*.hkm` file that declares itself as
either a `package` or an `application`. A repository may contain multiple
packages and applications.

### Host repository
The host repository is repository that is being build by the compiler.
Dependencies are not host repositories themselves.

The compiler will create a `.hkdeps/` directory in the host repository to clone
all the repositories that are declared as dependencies recursively.

Each cloned-repository is located in `.hkdeps/<name>-<hash>/`:
 - The `<name>` is the last part of the URL, `*.zip` file name, or the name of a
   local directory. Excluding the extension.
 - The `<hash>` is a hash of the full URL/path and branch of the repository,
   using the following algorithm:
    1. SHA-256 hash of:
      * **git**: The full URL of the repository, a dash `-`, and the branch name.
      * **zip**: The full URL of the .zip file.
      * **local directory**: The full path of the directory.
    3. Encode the lower 51 bits with base36, with the following alphabet:
       `0123456789abcdefghijklmnopqrstuvwxyz` (lowercase letters and digits).

### Remote repositories
Each `*.hkm` file may declare a remote repository using the `repository`
declaration. This declaration specifies a URL to a remote repository that
should be cloned, or downloaded.

It is allowed for a remote repository to fail to clone or download, as long as
another local or remote repository can be found that contains the same packages.

### Local repositories
From the point of view of a package manager, it does not make sense for a
a source file to declare a local repository. As when the repository is
distributed to other developers, any local repository will not be available.

A local repository is declared by passing the path to the repository as a
`--repository=<paths>` option or `HKREPOSITORY=<paths>` environment variable to
the compiler. Multiple paths may be specified, separated by a `:` character.

When resolving packages, the compiler will ignore the version number of any
package in a local repository and prioritice it above remote repositories.

## Build Steps

 1. **Clone**: The compiler will clone every repository declared with
    `repository` in any `.hkm` file recursively; into the `.hkdeps/` directory
    of the host repository.
 2. **Download**: The compiler will build and execute every application
    with the `download` condition in depth-first order. This allows repositories
    to download data from remote servers. The data should be stored in the
    `.hkdeps/<name>-<hash>/` directory of the host repository.
 3. **Generate**: The compiler will build and execute every application
    with the `generate` condition in depth-first order. This allows repositories
    to generate code (and use data from previous `download` step).
 4. **Test**: The compiler will build and execute every application
    with the `test` condition in the host repository. A `test-report.xml` file
    will be generated at the root of the host repository.
 5. **Compile**: The compiler will build every application with the
    `compile` condition in the host repository.

To test or compile recursively, use the `--test-recursive` or
`--compile-recursive` options respectively.

Executables in cloned repositories are stored in the
`.hkdeps/<name>-<hash>/bin/` of the host repository. Excutables from the host
repository are stored in the `bin/` directory of the host repository.

Executables in cloned repositories which are executed during the `download`,
`generate`, and `test` steps are executed with the current working directory set
to `.hkdeps/<name>-<hash>/` of the host repository.

When recusively testing, the `test-report.xml` file in the host repository will
contain the results of all tests in the host repository and cloned repositories.
