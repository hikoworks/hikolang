# import-declaration

## Syntax

`import` [_module_name_](module_name.md) __(__ `as` [_identifier_](identifier.md) __)?__ `;`

`import` `git` [_string_literal_](string_literal.md) [_string_literal_](string_literal.md)__*__ `;`
`import` `zip` [_string_literal_](string_literal.md) `;`

`import` `lib` [_string_literal_](string_literal.md) __(__ `if` [_condition_expression_](condition_expression.md) __)?__ `;`

## Semantics

An `import` declaration is used to import a module into the current file.

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

The optional _condition_expression_ is evaluated during the prologue-scan phase of
compilation, and is used to determine the correct library to link against.
