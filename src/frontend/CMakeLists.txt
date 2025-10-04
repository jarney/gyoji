set(FRONTEND_INCLUDES
        ${PROJECT_SOURCE_DIR}/src/misc
        ${PROJECT_SOURCE_DIR}/src/mir
        ${PROJECT_SOURCE_DIR}/src/context
        ${PROJECT_SOURCE_DIR}/src/frontend
        ${CMAKE_BINARY_DIR}
)

set(FRONTEND_PUBLIC_HEADERS_TOP
    jlang-frontend.hpp
)
set(FRONTEND_PUBLIC_HEADERS_BOTTOM
    jlang-frontend/tree.hpp
    jlang-frontend/syntax-node.hpp
    jlang-frontend/namespace.hpp
    jlang-frontend/parse-result.hpp
    jlang-frontend/forward.hpp
    jlang-frontend/parser.hpp
    jlang-frontend/type-resolver.hpp
    jlang-frontend/function-resolver.hpp
    jlang-frontend/lex-context.hpp
)

set(FRONTEND_SOURCES
    jlang.l
    jlang.y
    tree.cpp
    syntax-node.cpp
    namespace.cpp
    parse-result.cpp
    lex-context.cpp
    parser.cpp
    type-resolver.cpp
    function-resolver.cpp
    ${FRONTEND_PUBLIC_HEADERS_TOP}
    ${FRONTEND_PUBLIC_HEADERS_BOTTOM}
)


BISON_TARGET(jlang_y jlang.y ${CMAKE_BINARY_DIR}/jlang.y.cpp
             DEFINES_FILE ${CMAKE_BINARY_DIR}/jlang.y.hpp)
FLEX_TARGET(jlang_l jlang.l ${CMAKE_BINARY_DIR}/jlang.l.cpp
            DEFINES_FILE ${CMAKE_BINARY_DIR}/jlang.l.hpp)

add_library(jlang-frontend ${FRONTEND_SOURCES} ${BISON_jlang_y_OUTPUTS} ${FLEX_jlang_l_OUTPUTS})
target_include_directories(jlang-frontend
    PUBLIC
        ${FRONTEND_INCLUDES}
)

install(
    TARGETS
        jlang-frontend
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
        DESTINATION include/jlang-frontend
)


add_executable(test_namespace test_namespace.cpp)
target_include_directories(test_namespace PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_namespace
    jlang-frontend
    jlang-mir
    jlang-context
    jlang-misc
)
add_test(NAME test_namespace COMMAND test_namespace)

add_executable(test_syntax test_syntax.cpp)
target_include_directories(test_syntax PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_syntax
    jlang-frontend
    jlang-mir
    jlang-context
    jlang-misc
)
add_test(NAME test_syntax COMMAND test_syntax ${CMAKE_SOURCE_DIR})


add_executable(test_token_stream test_token_stream.cpp)
target_include_directories(test_token_stream PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_token_stream
    jlang-frontend
    jlang-mir
    jlang-context
    jlang-misc
)

add_executable(test_types test_types.cpp)
target_include_directories(test_types PUBLIC ${PROJECT_SOURCE_DIR}/types)
target_link_libraries(test_types jlang-frontend jlang-mir jlang-context jlang-misc)
add_test(NAME test_types COMMAND test_types ${CMAKE_SOURCE_DIR})

