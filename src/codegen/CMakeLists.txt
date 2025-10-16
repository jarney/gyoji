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
set(CODEGEN_PUBLIC_HEADERS
         gyoji-codegen.hpp
)

set(CODEGEN_SOURCES
        codegen.cpp
        gyoji-codegen-llvm.cpp
        ${CODEGEN_PUBLIC_HEADERS}
)

add_library(gyoji-codegen ${CODEGEN_SOURCES})
target_include_directories(gyoji-codegen
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/misc
        ${PROJECT_SOURCE_DIR}/src/mir
        ${PROJECT_SOURCE_DIR}/src/context
        ${PROJECT_SOURCE_DIR}/src/frontend
        ${PROJECT_SOURCE_DIR}/src/codegen
        ${CMAKE_BINARY_DIR}
	/usr/include/llvm-18
	/usr/include/llvm-c-18
)
install(
    TARGETS
        gyoji-codegen
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${CODEGEN_PUBLIC_HEADERS}
        DESTINATION include/gyoji-codegen
)

