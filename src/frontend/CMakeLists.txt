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
set(FRONTEND_INCLUDES
        ${PROJECT_SOURCE_DIR}/src/misc
        ${PROJECT_SOURCE_DIR}/src/mir
        ${PROJECT_SOURCE_DIR}/src/context
        ${PROJECT_SOURCE_DIR}/src/frontend
        ${CMAKE_BINARY_DIR}
)

set(FRONTEND_PUBLIC_HEADERS_TOP
    gyoji-frontend.hpp
)
set(FRONTEND_PUBLIC_HEADERS_BOTTOM
    gyoji-frontend/tree.hpp
    gyoji-frontend/syntax-node.hpp
    gyoji-frontend/parse-literal-int.hpp
    gyoji-frontend/ns2.hpp
    gyoji-frontend/parse-result.hpp
    gyoji-frontend/forward.hpp
    gyoji-frontend/parser.hpp
    gyoji-frontend/type-lowering.hpp
    gyoji-frontend/function-lowering.hpp
    gyoji-frontend/function-scope.hpp
    gyoji-frontend/lex-context.hpp
)

set(FRONTEND_SOURCES
    gyoji.l
    gyoji.y
    tree.cpp
    syntax-node.cpp
    ns2.cpp
    parse-result.cpp
    lex-context.cpp
    parser.cpp
    type-lowering.cpp
    function-lowering.cpp
    function-scope.cpp
    parse-literal-int.cpp
    ${FRONTEND_PUBLIC_HEADERS_TOP}
    ${FRONTEND_PUBLIC_HEADERS_BOTTOM}
)


BISON_TARGET(gyoji_y gyoji.y ${CMAKE_BINARY_DIR}/gyoji.y.cpp
             DEFINES_FILE ${CMAKE_BINARY_DIR}/gyoji.y.hpp)
FLEX_TARGET(gyoji_l gyoji.l ${CMAKE_BINARY_DIR}/gyoji.l.cpp
            DEFINES_FILE ${CMAKE_BINARY_DIR}/gyoji.l.hpp)

add_library(gyoji-frontend ${FRONTEND_SOURCES} ${BISON_gyoji_y_OUTPUTS} ${FLEX_gyoji_l_OUTPUTS})
target_include_directories(gyoji-frontend
    PUBLIC
        ${FRONTEND_INCLUDES}
)

install(
    TARGETS
        gyoji-frontend
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${FRONTEND_PUBLIC_HEADERS_TOP}
        DESTINATION include/
)
install(
    FILES
        ${FRONTEND_PUBLIC_HEADERS_BOTTOM}
        DESTINATION include/gyoji-frontend
)


#add_executable(test_namespace test_namespace.cpp)
#target_include_directories(test_namespace PUBLIC ${FRONTEND_INCLUDES})
#target_link_libraries(test_namespace
#    gyoji-frontend
#    gyoji-mir
#    gyoji-context
#    gyoji-misc
#)
#add_test(NAME test_namespace COMMAND test_namespace)

add_executable(test_syntax test_syntax.cpp)
target_include_directories(test_syntax PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_syntax
    gyoji-frontend
    gyoji-mir
    gyoji-context
    gyoji-misc
)
add_test(NAME test_syntax COMMAND test_syntax ${CMAKE_SOURCE_DIR})

add_executable(test_scope test_scope.cpp)
target_include_directories(test_scope PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_scope
    gyoji-frontend
    gyoji-mir
    gyoji-context
    gyoji-misc
)
add_test(NAME test_scope COMMAND test_scope ${CMAKE_SOURCE_DIR})


add_executable(test_token_stream test_token_stream.cpp)
target_include_directories(test_token_stream PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_token_stream
    gyoji-frontend
    gyoji-mir
    gyoji-context
    gyoji-misc
)

add_executable(test_types test_types.cpp)
target_include_directories(test_types PUBLIC ${PROJECT_SOURCE_DIR}/types)
target_link_libraries(test_types gyoji-frontend gyoji-mir gyoji-context gyoji-misc)
add_test(NAME test_types COMMAND test_types ${CMAKE_SOURCE_DIR})

