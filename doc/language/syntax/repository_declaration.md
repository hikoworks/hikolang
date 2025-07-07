# repository-declaration

## Syntax

`repository` `git` [_string_literal_](string_literal.md) [_string_literal_](string_literal.md)__?__ `;`
`repository` `zip` [_string_literal_](string_literal.md) `;`

## Semantics
A `repository` declaration is used to declare a repository that contains packages
that can be imported by other packages or applications.

The compiler will clone, copy or download each repository (recursively) into the
`_hkdeps` directory of the application being compiled. This means that all
dependencies recursively will be collected into a single directory structure.

If a package is declared in multiple repositories, the compiler will ignore
duplicates with a lower version number.

Repositories can also be specified in the `--search-path` option or `HKPATH`
environment variable, these are called local repositories. Packages found in the
local repository will override the packages in the remote repositories, even if
the remote repository has a higher version number. This is useful for local
development of packages.

### _hkdeps Directory Structure
The `_hkdeps` directory is structured as follows:

```
_hkdeps/
    <name>-<hash>/
        <file1.hkm>
        <file2.hkm>
        ...
```

The `<name>` is the last part of the URL or path of the repository, excluding
the extension.

The `<hash>` is calculated as follows:
 1. Concatonate the URL and optional branch name with a `-` in between.
 2. Calculate the SHA-256 hash of the resulting string.
 3. Encode the lower 60-bits of the hash into 10 characters using base64url
    encoding.

The base64url alphabet that is very compatible with file names is as follows:
```
ABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyz
0123456789-_
```

### git
The `git` argument specifies a git repository URL and an optional branch name.

The compiler will clone and checkout the specified branch of the repository with
depth 1 into the `_hldeps/<module-name>-<hash>` directory of the application
that is being compiled.

When passing the `--fetch` option to the compiler, the compiler will
fetch the latest changes from the remote repository and update the files in the
`_hkdeps/<module-name>-<hash>` directory, if it already exists. 

### zip
The `zip` argument specifies a URL to a zip file that contains the module files.

The compiler will download the zip file and extract it into the
`_hkdeps/<module-name>-<hash>` directory of the application that is being
compiled.
