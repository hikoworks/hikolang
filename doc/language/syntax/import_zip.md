# import-zip

## Syntax

`import` `zip` _zip-url_ [_build_guard_](build_guard.md)__?__ `;`

zip-url := [_string_literal_](string_literal.md)

## Semantics

Downloads and extracts a zip file into `.hkdeps/<name>-<hash>/` of the
host-repository. Modules located in the zip file can then be imported into any
module in the current repository.

The _string_literal_ is the URL of the zip file to download.

> [!NOTE]
> It is not an error if the zip-file fails to download or extract,
> unless none of the repositories satisfy module-import requirements.
