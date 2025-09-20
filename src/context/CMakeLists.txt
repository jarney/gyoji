
set(JLANG_ERRORS_PUBLIC_HEADERS
    jlang-context.hpp
    jlang-context/errors.hpp
    jlang-context/token-stream.hpp
)
set(JLANG_ERRORS_SOURCES
    context.cpp
    errors.cpp
    token-stream.cpp
    ${JLANG_ERRORS_PUBLIC_HEADERS}
)

add_library(jlang-context ${JLANG_ERRORS_SOURCES})
target_include_directories(jlang-context
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/context
            ${PROJECT_SOURCE_DIR}/src/misc
)
install(
    TARGETS
        jlang-context
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${JLANG_ERRORS_PUBLIC_HEADERS}
        DESTINATION include/jlang-context
)

add_executable(test_errors test_errors.cpp)
target_include_directories(test_errors PUBLIC ${PROJECT_SOURCE_DIR}/errors)
target_link_libraries(test_errors jlang-context)
add_test(NAME test_errors COMMAND test_errors)
