# TODO list

This is just a big random list of things that I'd still like
to do with the language.

## Syntax processing
* Build more unit-tests to verify that the syntax is what we expect and
  the operator precedence rules work correctly.

* Check for various compiler errors and handle recovery a bit better.
  Probably defer this until we have more 'examples' of what goes wrong
  frequently and how to recover better.

* Error message context doesn't account for 'tabs' when formatting
  output for error arrows.  This is a bit complicated because we need
  to decide how big a 'tab' is in order to format it correctly in all cases.

* Support for a pre-processor like CPP so we can build and use
  header files.  Need to allow #line directives to be used to
  update the 'context' so that error messages are attributed to
  the correct source.  This will be slightly tricky in the lexer
  to make sure we don't get offset from the proper location.

* Allow declarations to come from translation units (current)
  and in future, annotate the .o and .a files with 'extra' ELF
  data that allows us to 'compile' against a library without
  a header file (because the relevant declarations are already
  present in the ELF).  This requires reading the ELF headers
  and specific sections.  Annotate with specific compiler ABI
  tag so we can structure it in a way that survives future versions
  and is backward-compatible.

## Borrow checker
* Implement the inputs to the borrow checker, going through
  the polonius rules and putting the 'facts' together
  that it will need.

* Make sure we can control the 'copy' of references.  Maybe it's
  enough just to prevent references from being copied
  implicitly in structures, so a class containing a reference
  just can't be copied, but other classes are copied
  automatically by copying members.  We shouldn't have to worry
  about providing a 'copy' or 'move' for classes.

* Write up a list of safety guarantees to provide and a 'proof' that
  the rules of the language enforce these rules.  i.e. use-before-initialize
  would have a definition and the rules of the language should be 'provable'
  to meet this definition.  Same for borrow rules.

* Figure out the 'reasoning' engine we want to use for it.
  Probably some type of HornSAT solver, possibly even one that
  we can incorporate into the standard library so we can
  write the borrow checker in Gyoji itself instead of in C
  Look into CFG-SSA and the "dominating" relation for graphs.
  This is going to provide a good foundation for establishing
  reasoning and flowing facts through the flow control graph.

## Semantic processing
  * Support const-ness of variables and protect them
    from assignments.

  * Merge blocks for assignments in control-flow statements.
    i.e. the PHI operation in LLVM (trinary operations)

  * Finish/audit syntax tree parsing
    * Global variables are still missing.
    * Trinary operations are still missing.
    * Cast and conversion operations
      * Cast is just a raw bitcast of bits from one type to another (but only for primitive types)
      * Cast of pointers to other pointers, but this is only available in unsafe mode.
      * References cannot be cast or converted at all because this would interfere with borrow rules.
      * Convert allows sign-extending and zero-extending number
      * Convert allows type conversion of floating-point numbers.
      * Classes can't be cast at all, this is unsafe.

  * Include enough semantic information like 'live' and 'dead'
    storage as well as use and mutation of borrwowed items
    so that we have enough information to satisfy the borrow checker.

  * Write a load of unit-tests for various operations to ensure
    that they behave the way we expect with operator precedence
    and other stuff.  Focus on 'internal' consistency (i.e. we know
    what to expect)

  * Build a global errors table and build out what we need
    to verify errors by checking whether a particular error
    is raised in an example fragment so we can verify that we're
    covering all of the various error conditions both at the semantic
    and syntax level.

  * End result should be a unit-test that compiles a sample
    function, links against it, and runs it to verify its
    behavior for simple if/else and a few basic operations
    like arithmetic on u32.  We can support other operations
    later as time allows, but we should have basic calculation
    and conditional support for this first pass.

  * Support class single inheritance
    * Maybe just not support this at all.  Inheritance is
      kind-of evil anyway and most people favor composition
      nowadays anyway.
    * Each inherited class is just another class
      with a member that is the supertype.
      Initialization can proceed in the obvious way by
      initializing the supertype member.

  * Enums since those aren't supported yet.
    * Enum = symbol with constant typed integers.
    * Scope and namespace concerns.
    * Should 'lower' to just 'constant integer' values.

  * Build some tests that verify compatibility with
    most of the expectations of the C-style
    expressions we know and love by using the preprocessor
    to execute the same expression in C and Jlang
    to make sure we get the same results in both cases.

    We have the basics of this, but we should try to
    be more complete in terms of actually
    verifying that it works the way we would expect
    with things like operator precedence and such.

## Bootstrapping
  * Figure out what parts of the compiler we can *already* write
    in J.  Need to pick out specific points that can easily be
    implemented.

## Marketing (after we've bootstrapped)
  * Figure out how to compile down to WASM modules so we can
    build a website that has the compiler inside it.

# Code Generation
  * We support composite types as return-values from functions,
    and arguments to them, but they don't follow the C ABI
    because LLVM doesn't do this for us.  We need to figure out
    hopefully just in the code-generation layer how to actually
    lower the MIR into ABI-compliant code for structure arguments
    and return-values.

# Compiler stability/elegance

* Size of "tree.o"
  Investigate why the libjlang-frontend.a is so damn big.
  It mostly seems to be "tree.o" which isn't too surprising since
  it's really the biggest cpp file, but it doesn't seem complicated
  so it's a bit of a surprise why the code generated is so big.

* Re-name 'resolver' to 'lowering'.  Also, possibly break up
  the function resolver because it's gotten too big.

* Factor out the "docs" and valgrind tests so that
  we can make doxygen, graphviz, and gcov into
  optional dependencies.

## Compilation: Resolve some design questions
Some rules for classes and such:
* Single inheritance only
* Generics just "erasure" level constructs
  a-la Java to ensure we're using types
  consistently but not generating additional
  code for them.

* Interface types are basically 'vtables' on-demand
  for classes when we "implement" an interface.
  Multiple inheritance of interfaces (like the Java model)
  
* Do we need keywords for 'super' and 'this'
  We do need these, but we should be super-careful
  about them leaking for memory safety reasons.

* This design should give us just enough OO stuff
  without cluttering the design and causing too much
  bloat.

## Start writing the standard library in Gyoji

* Basic string manipulation functions

* Containers: map, list, string, vector, set

* Unix/Posix system calls, wrap them in a good way
  to allow memory safety all the way down
  to the system-call level.

---------------------------------------------------------------

