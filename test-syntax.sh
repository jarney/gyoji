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
syntax-typedef.j
syntax-comments.j
syntax-access-qualifier.j
syntax-pointer.j
syntax-function-declaration.j
syntax-function-definition.j
syntax-function-unsafe-block.j
syntax-class.j
llvm-decl-var.j
type-resolution.j
valid-syntax-expression-primary.j
valid-syntax-function-simple-return.j
valid-syntax-identifiers.j
valid-syntax-ifelse.j
valid-syntax-typedefs.j
valid-syntax-variable-declaration.j
"
echo "Checking token stream output."
let failed=0
for test_file in ${TEST_FILES} ; do
    echo -n "    ${test_file}"
    ${CMAKE_BINARY_DIR}/src/frontend/test_token_stream ${CMAKE_SOURCE_DIR}/tests/${test_file} >${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}
    diff ${CMAKE_SOURCE_DIR}/tests/${test_file} ${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}
    if [ $? -ne 0 ] ; then
        echo " : FAILED"
        failed=1
    else
        echo " : PASSED"
    fi
done

echo "Checking that the identity transformation does not change the file."
let failed=0
for test_file in ${TEST_FILES} ; do
    echo -n "    ${test_file}"
    ${CMAKE_BINARY_DIR}/src/cmdline/jformat-identity ${CMAKE_SOURCE_DIR}/tests/${test_file} >${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}
    diff ${CMAKE_SOURCE_DIR}/tests/${test_file} ${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}
    if [ $? -ne 0 ] ; then
        echo " : FAILED"
        failed=1
    else
        echo " : PASSED"
    fi
done

let test_memory_scale=1
if [ $test_memory_scale -ne 0 ] ; then
    rm -f ${CMAKE_BINARY_DIR}/test-syntax-dir/memory_usage.csv.tmp
    for test_file in ${TEST_FILES} ; do
        echo ${test_file}
        valgrind --leak-check=full \
                 ${CMAKE_BINARY_DIR}/src/cmdline/jformat-identity \
                 ${CMAKE_SOURCE_DIR}/tests/${test_file} >/dev/null \
                 2>${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}.valgrind
        
        memory_usage=$(cat ${CMAKE_BINARY_DIR}/test-syntax-dir/${test_file}.valgrind | \
            grep 'bytes allocated' | \
            sed 's/.*frees, //g' | \
            sed 's/ .*//g' | \
            sed 's/,//g')
        file_size=$(ls -la tests/${test_file} |awk '{print($5);}')
        echo "${file_size},${memory_usage},${test_file}" >>${CMAKE_BINARY_DIR}/test-syntax-dir/memory_usage.csv.tmp
    done
    cat ${CMAKE_BINARY_DIR}/test-syntax-dir/memory_usage.csv.tmp | \
        sort -n >${CMAKE_BINARY_DIR}/test-syntax-dir/memory_usage.csv
    rm -f ${CMAKE_BINARY_DIR}/test-syntax-dir/memory_usage.csv.tmp
fi

if [ $failed -ne 0 ] ; then
    echo "One or more format-identity tests failed"
fi

exit $failed



