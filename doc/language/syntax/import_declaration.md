# import-declaration

## Syntax

`import` _import_type_ [_compile_condition_](compile_condition.md)__?__ `;`

### import_type

[_module_name_](module_name.md) __(__ `as` [_identifier_](identifier.md) __)?__ __|__
`git` [_string_literal_](string_literal.md) [_string_literal_](string_literal.md)__*__ __|__
`zip` [_string_literal_](string_literal.md) __|__
`lib` [_string_literal_](string_literal.md)

## Semantics

The import-declaration is part of the prologue of a file, and is used to import
several types of resources into the current file, package or repository.
 - A module, so that its functions, types and variables can be used in the
   current file.
 - A git/zip repository so that modules in that repository can be imported int
   files in the current repository.
 - A library, so that applications and libraries that import a module from a
   package will be linked against that library.

The optional _condition_expression_ is evaluated during the prologue-scan phase
of compilation, and is used to determine the correct module, repository and
libraries to link against.


### module import
Imports a module, so that its functions, types and variables can be used in the
current file.

The _module_name_ is the name of the module to import, and it
must be a valid module name. The optional `as` clause allows the module to be
imported under a different name, which can be useful to avoid name clashes or to
make the code more readable.

If the module is located in a different repository, then the repository must be
imported using a `git` or `zip` by at least one file in the current repository.
Likely this in done in a `application` or `package` file.

When you import a module that is declarred as a `package`, all the modules
of that package are imported. This means that you can use any module in the
package without having to import it explicitly.

### git import
Clone a git repository into `.hkdeps/<name>-<hash>/` of the host-repository.
Modules located in the git repository can then be imported into any module
in the current repository.

The first _string_literal_ is the URL of the git repository, and the second
_string_literal_ is the branch/tag/commit to checkout.

> [!NOTE]
> It is not an error if the repository fails to clone, unless none of the
> repositories satisfy module-import requirements.

### zip import
Downloads and extracts a zip file into `.hkdeps/<name>-<has>/` of the
host-repository. Modules located in the zip file can then be imported into any
module in the current repository.

The _string_literal_ is the URL of the zip file to download.

> [!NOTE]
> It is not an error if the zip-file fails to download or extract,
> unless none of the repositories satisfy module-import requirements.


### lib import
Link any applications that imports a module or its package recrusivly to
a library with the name of the _string_literal_.

The library is either a archive/static library or a dynamic/shared library 
with the name of the _string_literal_.

