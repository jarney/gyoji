
set(JLANG_ERRORS_PUBLIC_HEADERS
    jlang-errors/errors.hpp
)
set(JLANG_ERRORS_SOURCES
    errors.cpp
    ${JLANG_ERRORS_PUBLIC_HEADERS}
)

add_library(jlang-errors ${JLANG_ERRORS_SOURCES})
target_include_directories(jlang-errors
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/errors
            ${PROJECT_SOURCE_DIR}/src/misc
)
install(
    TARGETS
        jlang-errors
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${JLANG_ERRORS_PUBLIC_HEADERS}
        DESTINATION include/jlang-errors
)

add_executable(test_errors test_errors.cpp)
target_include_directories(test_errors PUBLIC ${PROJECT_SOURCE_DIR}/errors)
target_link_libraries(test_errors jlang-errors)
add_test(NAME test_errors COMMAND test_errors)
