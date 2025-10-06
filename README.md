# The Gyoji programming language

## What

Ths Gyoji programming language is intended to be a memory-safe low-level,
general-purpose programming language.  It is intended to be safer than C,
and offer high-level features of C++ without the un-neccessary complexity.
It is also intended to use many concepts from Rust like the borrow-checker
without going "too far" and making too many assumptions about the
"standard" library, the environment, or the way it should be used.

It uses the C ABI (largely thanks to LLVM) and does NOT introduce exceptions
or other constructs which would break ABI compatibility.  It does NOT
ever "panic" or halt execution without the programmer's explicit instruction
to do so.

The syntax will be "familiar" to users of C, Java, C++, borrowing many ideas
from prior art, but is not intended to be backward-compatible with any of them.

## What it is NOT

Gyoji is a programming language and NOT a build system.  Discovering
dependencies and compiling software is NOT the responsibility of
a programming language.  This is a question of separation of concerns
and the language is intended to stand on its own as a programming language
which is a set of instructions designed to tell a computer how to operate.

Gyoji is also not a package manager.  Packaging and distributing source
and object code is a separate concern also.  While the compiler does
produce binary code, the responsibility lies elsewhere in organizing
the results of the compilation for distribution.  There are many fine
examples of package managers and one need not have a new package manager
for every programming language.  Gyoji makes no assumptions or
restrictions on what package management techniques to use.

Gyoji is also not a standard library.  While there is (or soon will be)
a set of libraries written in the Gyoji library to perform certain
tasks, it is not the responsibility of a programming language
to prescribe things like lists, maps, sets, or algorithms.  Those
can be expressed in the Gyoji language and the language itself
makes no assumptions about what libraries are and are not used.
Again, this is a separation of concerns argument that allows
the language to be general-purpose.

While it is ABI compatible with C (i.e. you can call back and forth between them),
it is NOT syntax-compatible. (i.e. you can't just compile a random C file with
it even though much of the syntax matches up pretty well). It is a NON-goal to
be a "drop-in" replacement for GCC or CLANG.

## Why?

Ok, so this is where it gets interesting.  I love the C
programming language because of its relative simplicity,
but let's be honest.  It simply wasn't designed with
safety in mind, so I also kind-of hate it.   I also love C++,
but let's face it.  It's riddled with little traps
and undefined behaviors in the name of making things "abstract"
and allowing implementations to make their own decisions.
This abstraction, however, comes at very little benefit.  In contrast,
languages like Java are more prescriptive and define more
behaviors, yet is still able to function well and used
generally.  C++ also has a problem with complexity in
template metaprogramming which can be useful in limited
circumstances, but in my opinion has gotten out of hand.
Java is great, but I really don't like the GC and the
lack of low-level control.  Rust is cool too and has really
good solutions to a lot of these problems, but that also
comes at a great cost.  Rust, in my opinion, makes
far too many assumptions about the 'standard' and 'core'
libraries and is full of 'panic()' in the most unexpected
places.  While it is quite memory safe, it is not at all
'termination' safe.  For example, a sqrt of a negative
value can cause the system to 'panic()'.  This is not
at all what I would characterize as 'safe'.  Although
it does not allow access to invalid memory, abrupt
and unexpected termination is not a good thing.

So that leaves me with an urge to pull together
some of the best ideas of each of these languages
and try something new.

Maybe this is just an old man shouting into the wind,
but this is a project I have to do and would love
some help doing it.

