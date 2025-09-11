#!/bin/bash
set -e
set -x

CMAKE_BINARY_DIR=build
CMAKE_SOURCE_DIR=.
if [ $# -ne 0 ] ; then
    CMAKE_BINARY_DIR=$1
    CMAKE_SOURCE_DIR=$2
fi
echo "Directories:"
echo ${CMAKE_BINARY_DIR}
echo ${CMAKE_SOURCE_DIR}

mkdir -p ${CMAKE_BINARY_DIR}/test-syntax-dir
${CMAKE_BINARY_DIR}/cparse format-identity ${CMAKE_SOURCE_DIR}/tests/llvm-decl-var.j >${CMAKE_BINARY_DIR}/test-syntax-dir/llvm-decl-var.j
diff ${CMAKE_SOURCE_DIR}/tests/llvm-decl-var.j ${CMAKE_BINARY_DIR}/test-syntax-dir/llvm-decl-var.j
