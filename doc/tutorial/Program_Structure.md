Tutorial - Program Structure
============================

Smallest program
----------------

The smallest program is a single file with the extension `.hkm` with a `program`
declaration. The `program` declaration tells the compiler that it should create
an executable with the given name.

```
// file: small.hkm
program "small"
```


Compilation
-----------

When running `hkc` without arguments, the compiler will find all files with
the `.hkm` extension in the current directory (or git-repository) and compile
all the files with a `program` declaration.

When compilation is successful the executables are stored in the `./bin/debug/`
directory. The example here will create an executable `small` or `small.exe`
depending on the operating system:

When you run this executable from the terminal, this program will exits
immediately with exit-code: `0`.


Hello World
-----------

The next program will be the common first tutorial program that prints `Hello
World`. It contains:

 - an `import` declaration which imports the `std` (standard library) package,
 - a `program` declaration with the name of the executable `"hello"`, and
 - it calls the `print()` function from the namespace `std` to
   print the text `Hello World` on the terminal.


```
// file: hello.hkm
import std
program "hello"

std.print("Hello World")
```

> [!NOTE]
> The name of a package and the name of a namespace are orthogonal. But
> often, like in this example, equal. Most functions exported by the `std`
> package are in the `std` namespace.


Comments and Documentation
--------------------------

A comment explains the "why" and "how" of a piece of code to a programmer that
reads your code. Documentation describes the API to programmers who use your
library.

There are three kinds:
 - Line comment; start with `//` and continues to the end of the line
 - Block comment; start with `/*` and ends at `*/`
 - Documentation; start with `/**` and ends at `*/`

```
/** Add the answer to the question of: Life, the Universe, and the rest.
 *
 * @param x The value to add the "answer" to.
 * @return The sum of the given value and the "answer"
 */
fn add_answer(x) {
    // Add 42, which is the answer to the question of:
    //     Life, the Universe, and the rest.
    // See: The Hitch Hikers Guide to the Galaxy.
    return x + 42
}
```

Documentation has a syntax on its own which is based on Doxygen.

