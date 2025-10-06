#!/bin/bash

if [ $# -ne 1 ] ; then
    CMAKE_BUILD_DIRECTORY=$1
else
    CMAKE_BUILD_DIRECTORY=build
fi

mkdir -p ${CMAKE_BUILD_DIRECTORY}/gcov
gcovr --exclude ${CMAKE_BUILD_DIRECTORY}/jlang.y.cpp \
     --exclude ${CMAKE_BUILD_DIRECTORY}/jlang.y.hpp \
     --exclude ${CMAKE_BUILD_DIRECTORY}/jlang.l.cpp \
     --exclude 'src/.*/test_.*.cpp' \
     --html ${CMAKE_BUILD_DIRECTORY}/gcov/report.html \
     --html-details
