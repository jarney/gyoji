# Gyoji Architecture

The architecture is described here.  A compiler is a complicated
machine, so there is more than one way to view the system.
Each section below describes the same system with a different
perspective.

## Components
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
                                      Compiler Context
+-----------+                      +-----------------+
| Input .j  |                      | Error reporting |
+-----------+                      +-----------------+
      |   Frontend                 | Token stream    |
+----------------------+-------+   +-----------------+
| +-----------------------+    |     |
| | Lex/Parse             |    |     | Error/context api
| | namespace resolution  |    |-----+ 
| +-----------------------+    |
|   |          | Syntax tree   |
|   |  +---------------------+ |
|   |  | Function Resolution | |
|   |  | Type Resolution     | |
|   |  | (Lowering to MIR)   | |
|   |  +---------------------+ |
|   |                  \       |
+------------------------------+
    |                    \   
    |                     \
+------------------+       \                            AnalysisPass
| formatters       |    +------------------+        +------------------------------+
+------------------+    | MIR              |--------| Borrow Check (Polonius-like) |
        | AST Tree      +------------------+        | Unreachable Code             |
	|                                           | Other semantic checks        |
+---------------+               |                   +------------------------------+
|format-identity|               |          
|format-pretty  |               |          
|format-tree    |               |           
+---------------+               |
        |               +------------------------+
  +-----------+         | Code Generation(LLVM)  |
  |Text output|         +------------------------+
  +-----------+                 | LLVM
                        +------------------+
                        | ELF (.o) output  |
                        +------------------+
```

The components defined in the system are:
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
  * Generic programming and type parameters.

* Analysis passes
  The ananlysis passes are additional checks on the semantics of
  what was parsed (i.e. what the code you wrote actually means).
  At this stage, for example, we check to make sure that the types
  have been completely defined (i.e. check that there are no un-resolved
  forward declarations that are actually used).  We also perform
  memory safety checks like making sure types match up and that
  the 'borrows' follow the borrowing rules.  We also perform
  checks to make sure that array access is within bounds
  and other rules that would impact type or memory safety.
  
  * Performing borrow-checking

* Code generation
  During this stage of processing, a code generation back-end produces
  executable output (ELF object files, for example) using LLVM or other
  similar code generation utilities.

  Some back-ends may even be possible allowing the generated code to
  be run within other machine environments such as a bash or python shell
  although there is probably very little utility in doing that, it might
  be cool to try.

## Lifetimes

The following is a "lifetime" diagram that indicates the lifetimes
involved in the compiler stages.  This is important because the compiler
is designed such that more than one front-end and back-end can be
used.  In order to facilitate this capability, it is important to understand
which parts of the compiler will have access to other components at
different stages throughout the compile lifecycle.  For instance,
the error handling of the compiler is a feature that is used
all throughout the program whereas the parse tree is only used
in the initial stages and is not directly accessible in
later stages of code generation.

High-level lifetimes and interactions:
```
Compiler Context     Front-end     MIR     Analysis      Code Generation
|<------------------ Read Tokens
|                       |
|<------------------ Write errors
|                       |
|                    Write MIR---->+
|                                  |
|                                  +----->Semantic
|                                         analysis
|<--------------------------------------- Emit errors
|                                  |
|                                  |
|                                  |
|                                  |
|                                  +-------------------->Consume MIR
|                                                        Produce .o
|                                                         |
|                                                         |
|                                                         +---------------->
|
+--->Report errors to user
     End program.

```

## Lifetimes (more granular view)

```
+-------------------------- Compiler Context
| +------------------------ TokenStream Input context
| |                                    provides context to the error handler.
| |
| | +---------------------- Errors : Error reporting library
| | |
| | |
| | |
| | |
| | |
| | |
| |-|--------------------- Parser : Reads from input                    Parsing
| | |                                      * Parses input into a
| | |                                        parse tree
| | |                                      * Records input data
| | |                                        in token stream.
| | | +------------------- TranslationUnit
| | | |
| | | | +----------------- NamespaceContext
| | | | |
| | | | | +--------------- mir::Types                               Type Lowering
| | | | | | 
| | | | | | +------------  mir::Functions                       Function lowering
| | | | | | |
| | | | | | |                                                   Type consistency
| | | | | | |
| | | | | | |
| | | | +----------------- ~NamespaceContext
| | | |   | |
| | | +-------------------  ~TranslationUnit
| | |     | |
| | |     | | +----------  Analysis                              Static Analysis
| | |     | | |                                                  Borrow Checking
| | |     | | |                                                  Visibility rules
| | |     | | |
| | |     | | |
| | |     | | +---------- ~Analysis
| | |     | |
| | |     | +------------- LLVMBackend                           Code Generation
| | |     | |
| | |     | +------------- ~LLVMBackend
| | |     |
| | |     +--------------- ~types::Types
| | |     
| | +---------------------- ~Errors
| |
| +------------------------ ~TokenStream
|
+-------------------------- ~CompilerContext
```

As you can see from this diagram, the only
things that last the entire program scope are
the token stream and error reporting facilities.
All other aspects of the program have more limited
lifetimes.  This enforces some separation of
concerns because some parts of the system simply will
not have access to others due to their lifetimes.

Lifetime notes:

* TokenStream: While this may be considered
               a part of the front-end, the reality is that
               every front-end will read input from the
               source.  This represents the source un-touched
               by any particular grammar front-end we choose to place
               on it and as such, this is agnostic of
               the syntax being parsed, making it possible to
               parse multiple styles of syntax.
               
* Errors       Errors need access to the token stream
               so it can produce context-aware messages
               pointing out the exact location in source
               of the various error conditions.

* Compiler Context: The compiler context is a container
                    for the objects that live for the entire
                    lifecycle of the compilation and wraps their
                    lifetimes to ensure that their scope lasts
                    for as long as they need to.
                    Other lifetimes such as the MIR are limited
                    and are "produced" by the front-end
                    and their lifetime is "handed off" to the
                    back-end which uses and then consumes it.

* MIR : It is the responsibility of the front-end to produce the MIR
        representation.  The responsibility of the front-end is to
        produce a parse tree AND THEN TO CONSUME IT after producing
        the MIR for the back-end.  The ultimate result of parsing
        it to consume input, placing it faithfully into the TokenStream
        and to produce the MIR.  The parse tree is simply a PART OF
        the front-end and is not directly exposed beyond the MIR
        to the analysis or code-generation stages.  All context relevant
        for analysis, code-generation, and debugging must be placed into
        the MIR so that it can be used in the later stages.

        As such, the MIR representation is really the "core" of the
        language with the front-end simply providing syntactical sugar
        to make it easier to manipulate.

* Analysis: The analysis is independent of the code generation system.
            It is responsible for any semantic analysis and operates
            purely on the MIR.  The borrow-checking, visibility rules,
            control-flow analysis (unreachable code), etc.
            are all performed at this stage on the MIR so that the result
            is MIR that is ready for code-generation which should not,
            at this point, produce errors because is makes guarantees on
            the validity of the constructions.

## Library Dependency Stack

This diagram shows how the components
may depend on one another.  This is important
in order to establish dependencies in the build system
but also establishes some sense of what component is responsible
for what part of the system.

```
+--------------+
| Frontend     | 
|    Namespaces| +-------------+ 
|    Lowering  | | Analysi  s  |
+--------------+ |   Semantics | +---------+
    Create MIR   |   Borrowing | | Codegen |
    (immutable)  +-------------+ |  LLVM   |
                     Use MIR     +---------+
                                   Consume MIR
+-------------------------------------------------+
| Compiler Context                                |
+---------------------------+--+------------------+
            +-------------+ |  |            +-------------------+
            | Errors      | |  |            | misc: Utilities   |
            +-------------+ |  |            |       for strings |
               +------------+  |            +-------------------+
               | TokenStream   |
               +---------------+
```



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
