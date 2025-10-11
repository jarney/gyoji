#!/bin/bash

if [ $# -ne 1 ] ; then
    CMAKE_BUILD_DIRECTORY=$1
else
    CMAKE_BUILD_DIRECTORY=build
fi

mkdir -p ${CMAKE_BUILD_DIRECTORY}/gcov
gcovr --exclude ${CMAKE_BUILD_DIRECTORY}/gyoji.y.cpp \
     --exclude ${CMAKE_BUILD_DIRECTORY}/gyoji.y.hpp \
     --exclude ${CMAKE_BUILD_DIRECTORY}/gyoji.l.cpp \
     --exclude ${CMAKE_BUILD_DIRECTORY}/gyoji.l.hpp \
     --exclude 'src/.*/test_.*.cpp' \
     --html ${CMAKE_BUILD_DIRECTORY}/gcov/report.html \
     --html-details
