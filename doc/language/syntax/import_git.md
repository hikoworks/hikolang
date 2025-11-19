# import-git

## Syntax

`import` `git` _git-url_ *git-rev*__?__
    __(__ `if` [_guard_expression_](guard_expression.md) __)?__ `;`

git-url := [_string_literal_](string_literal.md)

git-rev := [_string_literal_](string_literal.md)


## Semantics

Clone a git repository into `.hkdeps/<name>-<hash>/` of the host-repository.
Modules located in the git repository can then be imported into any module
in the current repository.

The first _string_literal_ is the URL of the git repository, and the second
_string_literal_ is the [git-rev](https://git-scm.com/docs/git-rev-parse.html)
to checkout.

When the `.hkdeps/<name>-<hash>/` directory for a repository does not yet
exist; the compiler will clone the repository into that directory then
checkout the git-rev.

Otherwise when the `.hkdeps/<name>-<hash>/` directory contains a clone
of the repository and points to the same git-rev and the git-rev is a
points to a _tag_ or a _sha_. Then this means that the correct revision
is checked out, so nothing is changed.

Otherwise when the `.hkdeps/<name>-<hash>/` directory contains a clone
of the repository and points to a different git-rev or the git-rev points
to a _branch_. The repository is cleaned by removing files that are not
part of the repository, then the remote is fetched and the git-rev is
checked out. 

A failure to clone is a warning, it is possible that other checkouts may
satisfy imported packages.

A failure to update is an error, because it may mean that local
modifications of files are conflicting with the update.

A _tag_ is expected to not change and therefor no update is done. You
can override this using the `--force-update` compiler option.

A cleanup is needed when the source files are updated, as out-of-repository
files may have been created by those source files. With the `--force-clean` will
clean these files even if the repository has not been updated.

### Build guard (optional)

The optional [_guard_expression_](guard_expression.md) is evaluated during the
prologue-scan phase of compilation, this checks if the `import` statment
should be processed or ignored.

