# Architecture

The following diagram shows how the various parts of the
compiler are organized.  Each of the components are designed
to be independent of one another, exposing interfaces
and all tied together in the "compiler" application
using dependency-injection techniques.  This way,
any part of the compiler can be modified and
augmented without having to unwind the entire system.

This makes the compiler/language system itself
modular and reduces the load on any individual
contributer to understand the whole flow too deeply.

```
+-----------+                      +---------------+
| Input .j  |                      |Error reporting|
+-----------+                      +---------------+
      |                            |Input Context  |
+----------------------+           +---------------+
| Front-end (lex/parse)|             |
| namespace resolution |-------------+
+----------------------+             |
               | AST tree            |
       +------------------+          | Error/context api
       | Syntax back-ends |          |
       +------------------+          |
           |            \            |
           |             \ Semantic Translation Unit
           |              \          |
+------------------+       \         |
| formatters       |    +------------------+        +-----------------------------+
+------------------+    | Semantic Rules   |--------|Borrow Check (Polonius clone)|
        | AST Tree      +------------------+        +-----------------------------+
+---------------+               |          \
|format-identity|               |           \          +---------------+
|format-pretty  |               |            ----------|Type resolution|
|format-tree    |               |                      +---------------+
+---------------+               |                           |
        |               +------------------+ /--------------+
  +-----------+         | Code Generation  |/
  |Text output|         +------------------+
  +-----------+                 | LLVM
                        +------------------+
                        | ELF (.o) output  |
                        +------------------+
```

The architecture of the compiler is as follows:
* Front-end
  Lexical and syntactical analysis is the first stage.  This involves
  reading from input as bytes and producing a set of "tokens"
  which are recognized by a set of regular expressions.  The lexical
  analysis phase is lossless in the sense that it consumes and
  records all data read as data associated with these tokens. 
  This includes whitespace and comments.  This is useful
  later in some back-ends which can reproduce the input exactly (for testing)
  or produce the output as a "pretty-print" of the input (formatters).
  The lexical stage also includes handling namespaces and types.  This is
  because the syntax would be ambiguous with only identifiers, so the ability
  to distinguish between types and variables is important.
  The syntactical analysis uses the traditional LALR(1) algorithm
  to recognize the syntax and produce a parse tree.  The output of this
  stage is an abstract syntax tree which is used later to build the data-structures
  for the semantic representation of the language.

  Note that because of this structure, other front-ends for the language are possible
  including representations with entirely different syntax so for example, a
  "python-like" representation or a "rust-like" representation can be parsed into
  the same semantic tree.

* Syntax back-ends
  After the syntax has been parsed, one or more backends can be applied directly
  to the syntax tree.  Some of these are pure transformations of the input
  such as code formatters and XML tree output for debugging purposes.
  Other back-ends pass the information to the next stage of processing, the semantic
  stage.  The semantic stage may also provide a number of back-ends
  to perform code-generation, making use of all of the semantic information.

* Semantic analysis is next.  At this stage, the syntax tree is processed into
  a semantic representation of the language.  This semantic representation
  discards the whitespace and original tokens and organizes the input program
  in a way that closely matches the semantics.  At this layer, several additional
  analyses can be perfored to ensure consistency and correctness as well as
  preparing the program for a code generation phase later.  Specifically,
  this semantic analysis applies rules such as:
  
  * Ensuring types match
  * Resolving expression operations so that they are applied to the correct types.
  * Performing borrow-checking
  * Namespace and visibility rules.
  * Generic programming and type parameters.

* Code generation
  During this stage of processing, a code generation back-end produces
  executable output (ELF object files, for example) using LLVM or other
  similar code generation utilities.

  Some back-ends may even be possible allowing the generated code to
  be run within other machine environments such as a bash or python shell
  although there is probably very little utility in doing that, it might
  be cool to try.

# Plans/Projects

The structure of the project is such that independent projects can be worked on and completed independently.
This allows collaboration on the project in a way that brings us closer to the goals.  Each of
the projects below are things that contribute to the long-term success of the project, so if you're
interested in working on one of them, please raise an issue and help out.

## Syntax Testing and verification

### Purpose:
The purpose of this project is to build a test-suite for the parser and syntax tree
so that we have good confidence that the data in the parse tree accurately represents the
source-code.

### Resources: frontend/
The goals of this project are to ensure that each possible production of the syntax is exercised.
The result should be an executable and set of test cases such that after running the executable
against the test cases, a report of all of the productions that were used in the test-cases
is written.  Success is a verification that every possible production in the syntax BNF
is hit at least once.

## Doxygen

### Purpose
The purpose of this project is to set a good baseline of Doxygen
hygene in the project in order to encourage collaboration.  The
Doxygen should be built using the same CMake and tests/CI
should depend on it so we ensure that we keep the level
of document coverage high.  This is important to allow good
collaboration on the project.

## Unicode support

### Purpose:
The goal of this project is to support UTF-8 in the syntax, specifically,
identifiers, string literals, and comments should support UTF-8 cleanly
along with unit-tests to verify compliance.

### Resources: frontend/

## Website

### Purpose
The purpose of this project is to communicate about the project and its goals.
Specifically:
* High-quality compiler using C-like syntax.
* Like a half-way point between C and C++.  Sort-of like C++ before people got carried away with it.
* Borrow-checker of similar quality to Rust.
* Not syntax-compatible with C, but should be very familiar to C and C++ developers.

## Type System

### Purpose:
The purpose of this project is to produce a type system that is
capable of expressing the important aspects of types, specifically
composite types.  It should be able to answer several questions about
the code.
* What is the tree of types all the way down to primitive types (e.g. classes contain classes contain integers).
* What types can be converted to one another (i.e. pointers and inheritances).
* What visibility restrictions are placed on types and what rules can/should we impose on them.
  Note that this does not imply actually enforcing those rules since the type system
  should only be a representation of the type data, but should not actually
  be concerned with implementing visibility rules.
* What methods/constructors/destructors are associated with the types
  so that code-generation can use this to insert them at scope end, for example.
  Again, this should not perform that work, but should provide data to the
  code generation layer so that it knows what to do.

## Lowering and intermediate representations.

### Dependencies: Type System

### Purpose:
The purpose of this project is to build an intermediate representation of the
source file that is suitable for code-generation and borrow checking.  This
representation should 'lower' the code from the syntax level to the semantic
level by, for example, eliminating 'while', 'for', 'if' and using
exclusively 'branch' and 'goto'.  All syntax at this level should be
de-sugared so that it is close to an 'abstract' machine representation
suitable for code generation.

### Resources:
This intermediate representation should already have the type
system data available and should be "lowered" to the point that
the only operations are on primitive types.

## Borrow Checker

### Dependencies: Type System, Intermediate Representation

### Purpose
The purpose of this project is to provide a Rust-like borrow-checker
using the base logic outlined in the Polonius project.  The algorithm
is sound, and can be implemented on this IR to provide safety guarantees
in a manner similar to rust.

## Code Generation : Primary

### Dependencies: Type System, Intermediate Representation

### Purpose:
The purpose of this project is to generate code using the LLVM
library so that we can support x86, arm, wasm, and a variety
of other hardware architectures.

## Code Generation : JS
### Purpose:
The purpose of this project is to provide a compiled output
in JavaScript so that we can build a website that
runs the code.

## Bootstrapping

### Purpose
The purpose of this project is to build a minimal version of the
compiler using the language itself.  Of course, this assumes
that there is enough of the language working that we can
express all of the complicated things it takes to build the
language, so this is a pretty late-stage project.

# Features



Changes from C syntax:
* Types can be qualified with namespaces
* sizeof only works on types, not on expressions.
* typeof is a new built-in that will convert an expression to a type.  This was done because allowing expressions directly as arguments to sizeof would make types syntactially ambiguous with expressions.  In C, this is handled by having the lexer be type-aware.  In order to make sure the lexer is independent of the parser, the syntax was altered in order to make sure that it is unambiguous.  For example, consider sizeof(a).  If 'a' is declared as a type, then this works one way, but if it's declared as a variable, it works differently.  The syntax sizeof(typeof(a)); makes it unambiguous.
* All control flow requires braces.  This was done in order to avoid the "dangling else" ambiguity in the grammar.  While this is a well known ambiguity in the C syntax, it is still better to be explicit in associating the 'if' with the 'else'.
* Assignment expressions may not be used inline.  Assignment is always a "statement-level" expression, so "a=(b=c+2)" isn't valid.  This is ok because it is often difficult to read in this style and doesn't really add anything to the function of the language.
* Function-calls must be inline (?) Is this even possible with C grammar?  Can we not do sin(a)+sin(b)?  Must we always put these in intermediates?  What about get(a)++;
* Comma expressions are not a thing.  This is rarely used and little known.  It adds nothing to the language.
* Variable declaration is different mostly because this is required in order to get a non-ambiguous syntax.  The reason for this is that
  user-defined types cannot, in general, be distinguished from user-defined variable names without an additional syntax cue.
  For example char *a; might be a variable named "char" multiplied by a variable named "a".  We could try to rule this out by
  altering the statement expression syntax to permit only "assignment" and "function call" at the top-level, but that
  would preclude expressions like *a++ = *b++ becuase you could still do 'x*y = d' which could be construed as
  a pointer to x.  The "var" keyword tells us that we are definitely looking for a type name next, similar to
  the 'typedef' keyword.

Additional features:
* Namespace declarations
* Visibility declarations (TODO)
* unsafe mode: Disallow unsafe things in unsafe mode.
* Ref-counted smart pointers in unsafe mode.
* Objects and methods
  * The intention here is to keep just enough of C++
    to make it useful, but to do away with the things
    that make C++ hard to work with.  Operator overloading
    and exceptions are part of what make C++ really bloated
    and unpredictable.  By keeping only the basic "object" aspects,
    we can make it useful without too much bloat.
  * Scope guards
  * Method calls
* Initializers are all default.  There is no "uninitialized" data
  because all data by default has a well-defined initial state.
  No constructors are permitted for global or static variables,
  their initial state is entirely known when the program starts.
  This avoids constructor race conditions.  This also makes sense because
  there is no such thing as "going out of scope" for this data, so nothing
  like this is allowed.  There are too many race conditions and other
  strangeness that can arise from multiple parties referring to global
  variables in each others' contexts.
  This means you can't have any objects with constructors or destructors
  stored in global variables.
  
------------------------------------------------------------------------------

Features:
* Built-in lossless formatting
* Built-in refcounted pointer semantics.
* Array indexing is always boundary-checked in safe mode.
* Unsafe mode which allows
  * Pointer arithmetic
  * Removal of bounds checks
* Safe mode which locks down unsafe activities.
* Object-style scope-guards
* Dependency-free operation (no standard library required, very small overhead for embedded/kernel operations)
* Standard library separates OS from purely language libraries.
* Standard library has:
  * Lists, Maps, Trees, Graphs
  * Algorithms like A*, topological sort, generic for use in all contexts.
* Type system supports erasure, allowing us to not generate code if it already exists (SHA-1 of code-generated tree so we can re-use common routines)
* Dynamic dispatch?  Interfaces Java-style?  Type inference and casting?  How to handle the 'duck-typing' aspect and dynamic?
* Functions as first-class things?  Composeable at runtime?

-------------------------------------------------------------------------

Goals:
* I would like to have a type-safe and memory safe version of C.
  This means a lot of different things to different people, so a bit of explanation is required.
  Complete memory safety is a myth.  Although it's true that languages like Rust provide
  semantics in "safe" mode which allow the memory to be "memory safe", they do so at the cost
  of a difficult mental model, the borrow checker.  While the borrow-checker and region-style
  semantic reasoning does provide assurances of safety, they come at the cost of
  understandability of the model that restricts some legitimate use-cases which are
  perfectly memory safe, but if the compiler can't prove it, they are disallowed.

  Rust cannot prove many common cases, and it might be argued that a better
  reasoning system might someday hope to prove that all memory safe
  code is, in fact, safe.  This cannot happen, however, because Rice's theorem,
  a consequence of Turing's Stopping theorem, show that compilers cannot,
  in general, prove all cases[1].

  The other problem that Rust has is that is's suffering from bloat.  The
  executables end up being very large because the compiler and the core
  libraries are tightly coupled.  The whole "panic" and unwinding system
  is built-in and comes along with a bunch of garbage that isn't always (usually)
  wanted or needed.  You could argue that it's fine because you can
  always #![no_std] and #[no_panic] stuff away, but the fact that it's there
  by default is a bit troubling.  This is particularly true when
  functions like "sqrt()" may panic and unless the programmer knows
  to protect against this, can be a big problem for reliability.  The
  authors of Rust would argue that it's better to panic() rathre than
  have undefined behaviour, but the fact that "sqrt()" on a negative
  number may be undefined in a mathematical sense, it need not be
  undefined in a computational sense.  Returning a NaN would seem
  to be a better choice than a panic() in these cases, but making
  philosophical changes to the core standard library at this stage
  are tall asks.

  Instead, this language follows a different path.  Minimalism.

  The choice to use ref-counted pointers for memory safety may
  raise some red flags to some who are performance conscious,
  but before you panic, remember a few things.
      * Many systems use ref-counted pointers as a way to achieve memory safety.
      * Ref-counting allows you to avoid the non-determinism associated with GC.
      * When high-performance is needed, having pointer de-reference inside
        tight loops is already a bad idea.  When performance is a concern,
        bulk operations should be used ANYWAY.

  This means that for MOST operations, the performance associated with ref-counting
  isn't really a big deal, and for those operations where it is important,
  bulk operations should be used in order to avoid refcounts.
