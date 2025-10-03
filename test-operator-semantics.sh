#!/bin/bash

CMAKE_BINARY_DIR=build
CMAKE_SOURCE_DIR=.
if [ $# -ne 0 ] ; then
    CMAKE_BINARY_DIR=$1
    CMAKE_SOURCE_DIR=$2
fi
TEST_JCC_DIR=${CMAKE_BINARY_DIR}/test-operator-semantics-dir
mkdir -p ${TEST_JCC_DIR}

TEST_FILES="semantics-simple-values"

for TEST_FILE in ${TEST_FILES} ; do
    echo -n "Testing ${TEST_FILE} "

    # Preprocess the test file into J code.
    clang -E ${CMAKE_SOURCE_DIR}/tests/semantics-simple-values.c > ${TEST_JCC_DIR}/${TEST_FILE}.j
    
    # Build the code using jcc
    ${CMAKE_BINARY_DIR}/src/cmdline/jcc \
		       ${TEST_JCC_DIR}/${TEST_FILE}.j \
		       ${TEST_JCC_DIR}/${TEST_FILE}.j.o
    if [ $? -ne 0 ] ; then
	echo "${TEST_FILE} failed to compile with jcc"
	echo "FAILED"
	exit 1
    fi
    
    # Build the equivalent construct using C.
    clang -c \
	  -DC_LANG \
	  ${CMAKE_SOURCE_DIR}/tests/${TEST_FILE}.c \
	  -o ${TEST_JCC_DIR}/${TEST_FILE}.c.o
    if [ $? -ne 0 ] ; then
	echo "${TEST_FILE} failed to compile comparison with clang"
	exit 1
    fi

    # Link them into the 'main' for the test.
    clang -o \
	  ${TEST_JCC_DIR}/test-semantics-simple-values \
	  ${CMAKE_SOURCE_DIR}/tests/test-semantics-simple-values.c \
	  ${TEST_JCC_DIR}/semantics-simple-values.j.o \
	  ${TEST_JCC_DIR}/semantics-simple-values.c.o
    if [ $? -ne 0 ] ; then
	echo "${TEST_FILE} failed to compile test with clang"
	exit 1
    fi

    # Now run the test.  If it returns 0,
    # everything was fine.  If it returns != 0
    # then some value was incorrect
    echo "Running it"
    ${TEST_JCC_DIR}/test-semantics-simple-values
    if [ $? -ne 0 ] ; then
	echo "FAILED"
	exit 1
    else
	echo "SUCCESS"
    fi
    
done
