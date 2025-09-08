# TODO list

## Compilation
* Type parameters (i.e. generics)
  * How to write a 'type safe' linked list or other structure
    where the type of the members are checked at compile-time.
  * Do we need a 'minimalist' version of templates without all the metaprogramming bloat?

* Figure out how to handle header files and declarations/imports.
  Should we require a pre-processor, or can we do without?
  
* Some syntax for constructors/destructors/methods.

* Do we need keywords for 'super' and 'this'.

* What's our feeling about inheritance?

* What about traits and interfaces?

* What about generics and type parameters?

* Is this just going to end up being a messy bloat like C++?

* Where's the cutoff point between simple syntax and complete functionality?

## Code generation
* Start building basic LLVM code generation capabilities.

## Syntax verifications
* Build some unit-tests to verify that the syntax is what we expect and
  the operator precedence rules work correctly.

## Semantic processing
* Build a more appropriate intermediate semantic representation of the parsed
  file that is better suited to code generation.

* Build some tests that verify compatibility with
  most of the expectations of the C-style
  expressions we know and love by using the preprocessor
  to execute the same expression in C and Jlang
  to make sure we get the same results in both cases.

## Start writing the standard library in JLang
* Basic string manipulation functions
* Containers: map, list, string, vector, set

# Error handling
* For common syntax errors, figure out how to suggest a good
  solution to them and actually have useful error messages.
  
