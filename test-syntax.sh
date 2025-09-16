#!/bin/bash

CMAKE_BINARY_DIR=build
CMAKE_SOURCE_DIR=.
if [ $# -ne 0 ] ; then
    CMAKE_BINARY_DIR=$1
    CMAKE_SOURCE_DIR=$2
fi
mkdir -p ${CMAKE_BINARY_DIR}/test-syntax-dir

TEST_FILES="
syntax-empty.j
llvm-decl-var.j
"

echo "Checking that the identity transformation does not change the file."
let failed=0
for test_file in ${TEST_FILES} ; do
    echo -n "    ${test_file}"
    ${CMAKE_BINARY_DIR}/cparse format-identity ${CMAKE_SOURCE_DIR}/tests/${test_file} >${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}
    diff ${CMAKE_SOURCE_DIR}/tests/${test_file} ${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}
    if [ $? -ne 0 ] ; then
        echo " : FAILED"
        failed=1
    else
        echo " : PASSED"
    fi
done
if [ $failed -ne 0 ] ; then
    echo "One or more tests failed"
fi
exit $failed
