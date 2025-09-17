set(FRONTEND_INCLUDES
        ${PROJECT_SOURCE_DIR}/misc
        ${PROJECT_SOURCE_DIR}/frontend
        ${CMAKE_BINARY_DIR}
)

set(FRONTEND_PUBLIC_HEADERS
    jlang-frontend/frontend.hpp
    jlang-frontend/tree.hpp
    jlang-frontend/syntax-node.hpp
    jlang-frontend/token-stream.hpp
    jlang-frontend/namespace.hpp
    jlang-frontend/yacc-context.hpp
    jlang-frontend/lex-context.hpp
    jlang-frontend/input-source.hpp
    jlang-frontend/input-source-file.hpp
    jlang-frontend/forward.hpp
    jlang-frontend/parser.hpp
)

set(FRONTEND_SOURCES
    jlang.l
    jlang.y
    tree.cpp
    token-stream.cpp
    syntax-node.cpp
    namespace.cpp
    yacc-context.cpp
    lex-context.cpp
    input-source.cpp
    input-source-file.cpp
    parser.cpp
    ${FRONTEND_PUBLIC_HEADERS}
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
        ${FRONTEND_PUBLIC_HEADERS}
        DESTINATION include/jlang-frontend
)


add_executable(test_namespace test_namespace.cpp)
target_include_directories(test_namespace PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_namespace
    jlang-frontend
    jlang-misc
)
add_test(NAME test_namespace COMMAND test_namespace)

add_executable(test_syntax test_syntax.cpp)
target_include_directories(test_syntax PUBLIC ${FRONTEND_INCLUDES})
target_link_libraries(test_syntax
    jlang-frontend
    jlang-misc
)
add_test(NAME test_syntax COMMAND test_syntax ${CMAKE_SOURCE_DIR})
