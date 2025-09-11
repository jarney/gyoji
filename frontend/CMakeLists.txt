set(FRONTEND_INCLUDES
        ${PROJECT_SOURCE_DIR}/misc
        ${PROJECT_SOURCE_DIR}/frontend
        ${CMAKE_BINARY_DIR}
)

set(FRONTEND_PUBLIC_HEADERS
    jlang-frontend/ast.hpp
    jlang-frontend/jsyntax.hpp
    jlang-frontend/namespace.hpp
)

set(FRONTEND_SOURCES
    ast.cpp
    jlang.l
    jlang.y
    jsyntax.cpp
    namespace.cpp
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
