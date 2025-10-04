# TODO list

## Syntax processing
* Build more unit-tests to verify that the syntax is what we expect and
  the operator precedence rules work correctly.

* Check for various compiler errors and handle recovery a bit better.
  Probably defer this until we have more 'examples' of what goes wrong
  frequently and how to recover better.

* Support const-ness of variables and protect them
  from assignments.

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

* Figure out the 'reasoning' engine we want to use for it.
  Probably some type of HornSAT solver, possibly even one that
  we can incorporate into the standard library so we can
  write the borrow checker in JLang itself instead of in C

## Semantic processing
  * Merge blocks for assignments in control-flow statements.
    i.e. the PHI operation in LLVM

  * Clean up the interface to basic blocks so it's easier to grok

  * Finish/audit remaining OP_ operations in MIR
    * Support them in syntax layer
    * Support them in codegen layer

  * Finish/audit syntax tree parsing
    * Global variables, etc.

  * Include enough semantic information like 'live' and 'dead'
    storage as well as use and mutation of borrwowed items
    so that we have enough information to satisfy the borrow checker.

  * Write a load of unit-tests for various operations to ensure
    that they behave the way we expect with operator precedence
    and other stuff.  Focus on 'internal' consistency (i.e. we know
    what to expect)
    
  * End result should be a unit-test that compiles a sample
    function, links against it, and runs it to verify its
    behavior for simple if/else and a few basic operations
    like arithmetic on u32.  We can support other operations
    later as time allows, but we should have basic calculation
    and conditional support for this first pass.

  * Support classes and enums since those aren't supported yet.
    * Class = structure with functions.
    * Enum = symbol with constant typed integers.

  * Build some tests that verify compatibility with
    most of the expectations of the C-style
    expressions we know and love by using the preprocessor
    to execute the same expression in C and Jlang
    to make sure we get the same results in both cases.

    We have the basics of this, but we should try to

## Bootstrapping
  * Figure out what parts of the compiler we can *already* write
    in J.  Need to pick out specific points that can easily be
    implemented.

## Marketing (after we've bootstrapped)
  * Figure out how to compile down to WASM modules so we can
    build a website that has the compiler inside it.

# Compiler stability/elegance

* Size of "tree.o"
  Investigate why the libjlang-frontend.a is so damn big.
  It mostly seems to be "tree.o" which isn't too surprising since
  it's really the biggest cpp file, but it doesn't seem complicated
  so it's a bit of a surprise why the code generated is so big.

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

## Start writing the standard library in JLang

* Basic string manipulation functions

* Containers: map, list, string, vector, set

* Unix/Posix system calls, wrap them in a good way
  to allow memory safety all the way down
  to the system-call level.

---------------------------------------------------------------

