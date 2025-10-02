#!/bin/bash

#set -e
#set -x

# For testing control-flow constructs:
# This test builds the control-flow graph
# for the test-cases and outputs it as PNG
# so we can visually inspect the results and
# easily diagnose problems in the control-flow
# generation logic.

CMAKE_BINARY_DIR=build
CMAKE_SOURCE_DIR=.
if [ $# -ne 0 ] ; then
    CMAKE_BINARY_DIR=$1
    CMAKE_SOURCE_DIR=$2
fi
mkdir -p ${CMAKE_BINARY_DIR}/test-cfg-dir
TEST_CFG_DIR=${CMAKE_BINARY_DIR}/test-cfg-dir

JCC=${CMAKE_BINARY_DIR}/src/cmdline/jcc

TEST_FILES="semantics-if-else"

echo "Checking token stream output."
let failed=0
failed_message=""
for TEST_FILE in ${TEST_FILES} ; do
    echo -n "    ${test_file}"

    # 1. Process file into .o and llvm IR (.o.ll)
    echo "Processing ${CMAKE_SOURCE_DIR}/tests/${TEST_FILE}.j"
    
    ${JCC} \
	${CMAKE_SOURCE_DIR}/tests/${TEST_FILE}.j \
	${TEST_CFG_DIR}/${TEST_FILE}.o
    if [ $? -ne 0 ] ; then
	failed_message="Failure failed compiling object file ${TEST_CFG_DIR}/${TEST_FILE}.o"
        failed=1
    fi
    
    # 2. Process .ll into a dot call-graph (intermediate)
    opt-18 --passes=dot-cfg --cfg-dot-filename-prefix=${TEST_CFG_DIR}/${TEST_FILE} ${TEST_CFG_DIR}/${TEST_FILE}.o.ll >/dev/null
    if [ $? -ne 0 ] ; then
	failed_message="Failure generating dot graph ${TEST_CFG_DIR}/${TEST_FILE}.o.ll"
	failed=1
    fi

    # 3. Process DOT files (one for each function in translation unit)
    # into call graph PNG files for inspection.
    for file in $(ls ${TEST_CFG_DIR}/${TEST_FILE}*.dot) ; do
	dot ${file} -Tpng -o ${file}.png
	if [ $? -ne 0 ] ; then
	    failed_message="Failed generating dot file ${file}.png"
	    failed=1
	fi
    done

    if [ ${failed} -ne 0 ] ; then
        echo " : FAILED ${failed_message}"
    else
	echo " : PASSED"
    fi

done

if [ ${failed} -ne 0 ] ; then
    exit 1
fi
exit 0
