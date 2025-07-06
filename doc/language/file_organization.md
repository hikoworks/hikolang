# File Organization

This document outlines the organization of files in the project.

## Search Paths

The search paths for a project are as follows and in this order:
 - The current directory, or the directory passed as the non-option argument to
   the compiler.
 - The paths specified in the `--search-path=<paths>` options.
 - The paths created by downloading packages specified in the `.hkp`
   files, recursively.

The search path is a `;` separated list of:
 - directories,
 - .zip files,
 - git repository URLs

The `.hkp` file includes the `import package` declaration which specifies the
packages to be downloaded and their versions. The compiler will download the
packages and add them to the search path.


> [!NOTE]
> It is a reportable error if a repository URL is used more than once from
> different branches.

> [!NOTE]
> It is a reportable error if the same module is found in the search path.

## File Structure
The file structure of a project or package is completely arbitrary. Each
.hkm file in the directory hierarchy is read by the compiler to determine
its module name and what modules it imports.

Directories and files that have a dot `.` prefix are ignored by the compiler.
The compiler uses this to place downloaded packages in a `.hkm/packages`
directory.

## A .hkm source file
A .hkm file is a source file that contains code that implements a module or
application.

The first statement in a .hkm file is either a `module` or `application`
statement. This is followed by zero or more `import` statements. This forms
the prologue of the file. Any other statements in the file terminates the
prologue and starts the body of the file.

The compiler will read the prologue of each .hkm file in the directory hierarchy
for each package listed in the search path. From this it will determine
which files need to be compiled and in which order.

A `module` or `application` statement may include an expression for conditional
compilation which will be evaluated during the prologue scan to determine if the
file should be compiled for the current platform, architecture, build-phase, or
other conditions.

> [!NOTE]
> It is a reportable error if two .hkm files have the same module or
> application name after evaluation of the conditional compilation expression.

## Application executable
An application executable is created by compiling a .hkm file that contains an
`application` statement. The executable is named with the name specified in the
`application` statement and is placed next to the .hkm file.

Like the `module` statement, the `application` statement may include an
expression for conditional compilation which will be evaluated during the
prologue scan to determine if the file should be compiled for the current
platform, architecture, build-phase, or other conditions.

There are a set of special conditions which must be part of the conditional
compilation expression for an application to be compiled in the current build
phase. These conditions are:
 - `download`: This application is used for downloading data from the
   internet before generating code.
 - `generate` : This application is used for generating code.

## Build Phases

 1. **Package Download Phase**: The compiler will download the packages
    specified in the `.hkp` file, recursively. This includes downloading
    the source code from the specified URLs, and .zip files and adding them
    to the search path. Each package will be placed in a
    `.hkm/packages/<package_name>` directory of the current package being
    compiled. The package name is derived from the URL or .zip file name.
 2. **Per Package**: For each package in depth-first order:
    - Compile the package with the `download` build phase, any application with
      the `download` condition will be compiled and executed in alphabetical
      order.
    - Compile the package with the `generate` build phase, any application with
      the `generate` condition will be compiled and executed in alphabetical
      order.
    - Compile the package normally.
