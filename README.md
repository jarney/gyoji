

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
