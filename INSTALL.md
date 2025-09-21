# Building from source

In order to build from source, some dependencies are needed:

```
make
g++
llvm-18
cmake
bison
flex
doxygen
graphviz
valgrind
```

In order to install these dependencies on an Ubuntu or Debian system, this should work:

```
apt-get install build-essential install bison flex cmake llvm valgrind
```

## Why dependencies are needed

* Valgrind:
  This is needed because the tests perform a 'valgrind' check on some tests
  in order to ensure that the compiler is clean and has no leaks.
  
* doxygen, graphviz:
  These are used to produce the doxygen output

* bison:
  This is the LALR(1) parser used to parse the .y into .cpp
  so we don't have to hand-code the parser.

* flex:
  This is the regular-expression tokenizer so we don't have
  to hand-code the lexical analysis.

* cmake/make:
  The build system relies on CMakeFiles.txt

* g++:
  In order to build the compiler on a platform that doesn't have
  the compiler (becuase it doesn't exist yet), we needed
  a bootstrapping source language.
  Ultimately, it should be possible to build the compiler
  using the language itself, but the language is still under
  development, so that's not happening yet.


* llvm-18:
  This is the library used to emit bytecode/machine instructions.


  