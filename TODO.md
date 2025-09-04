# TODO list

## Compilation
* Figure out how to handle header files and declarations/imports.
  Should we require a pre-processor, or can we do without?
* Add 'unsafe' keyword to distinguish dangerous operations.
* Fixed-length buffer declarations.
* Figure out a good function pointer syntax since we don't really have one at this point.
  This is difficult because we need to distinguish between types and values with some keyword construct.
* FOR loops (semicolon style since we don't have iterators yet and perhaps that would imply something too tightly coupled with standard library?)
  Should we make the iterator style for loop a macro like #foreach ?

## Code generation
* Build a more appropriate intermediate representation of the parsed
  file that is better suited to code generation.
* Start building basic LLVM code generation capabilities.

## Syntax verifications
* Build some unit-tests to verify that the syntax is what we expect and
  the operator precedence rules work correctly.

## Semantic verifications
* Build some tests that verify compatibility with
  most of the expectations of the C-style
  expressions we know and love by using the preprocessor
  to execute the same expression in C and Jlang
  to make sure we get the same results in both cases.

## Start writing the standard library in JLang
* Basic string manipulation functions
* Containers: map, list, string, vector, set
