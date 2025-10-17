# Copyright 2025 Jonathan S. Arney
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https://github.com/jarney/gyoji/blob/master/LICENSE
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

set(ANALYSIS_PUBLIC_HEADERS
    gyoji-analysis.hpp
)
set(ANALYSIS_SOURCES
    analysis.cpp
    analysis-type-resolution.cpp
    analysis-unreachable.cpp
    analysis-return-values.cpp
    analysis-borrow-checker.cpp
    ${ANALYSIS_PUBLIC_HEADERS}
)

add_library(gyoji-analysis ${ANALYSIS_SOURCES})
target_include_directories(gyoji-analysis
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/misc
            ${PROJECT_SOURCE_DIR}/src/context
            ${PROJECT_SOURCE_DIR}/src/mir
            ${PROJECT_SOURCE_DIR}/src/analysis
)
install(
    TARGETS
        gyoji-analysis
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${ANALYSIS_PUBLIC_HEADERS}
        DESTINATION include/gyoji-analysis
)

add_executable(test_analysis test_analysis.cpp)
target_include_directories(test_analysis
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/analysis
    )
target_link_libraries(test_analysis gyoji-analysis gyoji-mir gyoji-context gyoji-misc)
add_test(NAME test_analysis COMMAND test_analysis ${CMAKE_SOURCE_DIR})

