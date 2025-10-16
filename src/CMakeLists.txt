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
add_subdirectory(misc)
add_subdirectory(context)
add_subdirectory(frontend)
add_subdirectory(mir)
add_subdirectory(analysis)
add_subdirectory(cmdline)
add_subdirectory(codegen)

add_test(NAME test-syntax COMMAND ${CMAKE_SOURCE_DIR}/test-syntax.sh ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})
add_test(NAME test-cfg COMMAND ${CMAKE_SOURCE_DIR}/test-cfg.sh ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})
add_test(NAME test-operator-semantics COMMAND ${CMAKE_SOURCE_DIR}/test-operator-semantics.sh ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})

