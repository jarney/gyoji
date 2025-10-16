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

set(GYOJI_ERRORS_PUBLIC_HEADERS
    gyoji-context.hpp
    gyoji-context/errors.hpp
    gyoji-context/token-stream.hpp
)
set(GYOJI_ERRORS_SOURCES
    compiler-context.cpp
    source-reference.cpp
    errors.cpp
    token-stream.cpp
    ${GYOJI_ERRORS_PUBLIC_HEADERS}
)

add_library(gyoji-context ${GYOJI_ERRORS_SOURCES})
target_include_directories(gyoji-context
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/context
            ${PROJECT_SOURCE_DIR}/src/misc
)
install(
    TARGETS
        gyoji-context
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${GYOJI_ERRORS_PUBLIC_HEADERS}
        DESTINATION include/gyoji-context
)

add_executable(test_errors test_errors.cpp)
target_include_directories(test_errors PUBLIC ${PROJECT_SOURCE_DIR}/errors)
target_link_libraries(test_errors gyoji-context)
add_test(NAME test_errors COMMAND test_errors)
