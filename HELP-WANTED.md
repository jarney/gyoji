# Help Wanted!

This is a work in progress and help is wanted to contribute
and make the project better.  If you are interested in learning about
compilers or contributing to the project, these are projects that can be
worked on somewhat independently of other work.  This is intended
to be a very accessible project for a dedicated enthusiast.  Building
compilers is very fun and rewarding and I'd love to have the help.

If you are interested,
please contact me through GitHub by raising an issue or a pull-request.
E-mail is ok too, but I don't really like spam, so I'm going to discourage that
form of communication for this project.

## Syntax Testing and verification

In order to make the system as robust as possible, it is important
to have a suite of tests to verify the behaviour of the system.
Specifically, it would be great to have a set of example files
written in the language as both "good" and "bad" examples of syntax
that may be presented for compilation and verify whether they
do or do not compile as expected.

### Purpose:
The purpose of this project is to build a test-suite for the parser and syntax tree
so that we have good confidence that the data in the parse tree accurately represents the
source-code.

### Resources: frontend/
This project would mainly involve creating new files (probably under the "test" directory)
and building test jigs to verify their parse behavior in various scenarios.

## Website

The purpose of this project is to communicate about the project and its goals
and hopefully encourage more interest in using and participating in the project.

Specifically, highlight the nice attributes of the language such as:
* High-quality compiler using C-like syntax.
* Like a half-way point between C and C++.  Sort-of like C++ before people got carried away with it.
* Borrow-checker of similar quality to Rust.
* Minimalist in terms of dependencies
* General-purpose in the sense that it makes very few assumptions about
  the platform, libraries, and architecture of the target.
* Not syntax-compatible with C, but should be very familiar to C and C++ developers.

## Type Conversion and casting

As it currently stands, there are no facilities for
converting types to one another except for a very primitive
"widening" that allows integers to be promoted to larger
sizes.  This is safe, but is unrealistically restrictive.

Type cast operations should be introduced to allow, for example,
certain conversions from signed to unsigned integer types
and to and from floating-point integer types.  Each of these
conversions should be handled carefully so that the authors
of programs can know precisely how the conversions will be
handled.  For example, in a conversion from a signed to an
unsigned value, the sign information may be lost and it's
important to be specific about what the conversion will
do both in a numeric sense and in a 'bitwise' sense to
the values it manipulates.  Similarly for sign-extending
values and truncation.

## Borrow Checker

The purpose of this project is to provide a Rust-like borrow-checker
using the base logic outlined in the Polonius project.  The algorithm
is sound, and can be implemented on this MIR to provide safety guarantees
in a manner similar to rust.

## Bootstrapping

The purpose of this project is to build a minimal version of the
compiler using the language itself.  Of course, this assumes
that there is enough of the language working that we can
express all of the complicated things it takes to build the
language, so this is a pretty late-stage project.





