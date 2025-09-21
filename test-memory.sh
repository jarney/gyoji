#!/bin/bash

valgrind --leak-check=full \
         ./build/src/cmdline/jformat-identity \
         tests/llvm-decl-var.j \
         >/dev/null 2>build/valgrind-jformat-identity-leakcheck.log
grep 'no leaks are possible' build/valgrind-jformat-identity-leakcheck.log
if [ $? -ne 0 ] ; then
    echo "Memory leakage detected in jformat-identity"
    exit 1
fi

valgrind --leak-check=full \
         ./build/src/cmdline/jformat-tree \
         tests/llvm-decl-var.j \
         >/dev/null 2>build/valgrind-jformat-tree-leakcheck.log
grep 'no leaks are possible' build/valgrind-jformat-tree-leakcheck.log
if [ $? -ne 0 ] ; then
    echo "Memory leakage detected in jformat-tree"
    exit 1
fi
echo "PASSED"

exit 0
