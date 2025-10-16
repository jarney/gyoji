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

set(TYPES_PUBLIC_HEADERS
    gyoji-mir.hpp
    gyoji-mir/functions.hpp
    gyoji-mir/types.hpp
    gyoji-mir/symbols.hpp
    gyoji-mir/operations.hpp
)
set(TYPES_SOURCES
    mir.cpp
    functions.cpp
    types.cpp
    type.cpp
    type-member.cpp
    type-method.cpp
    symbols.cpp
    operation.cpp
    ${TYPES_PUBLIC_HEADERS}
)

add_library(gyoji-mir ${TYPES_SOURCES})
target_include_directories(gyoji-mir
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/frontend
            ${PROJECT_SOURCE_DIR}/src/misc
            ${PROJECT_SOURCE_DIR}/src/context
            ${PROJECT_SOURCE_DIR}/src/mir
)
install(
    TARGETS
        gyoji-mir
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${TYPES_PUBLIC_HEADERS}
        DESTINATION include/gyoji-mir
)

