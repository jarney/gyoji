
set(TYPES_PUBLIC_HEADERS
    jlang-types.hpp
)
set(TYPES_SOURCES
    types.cpp
    ${TYPES_PUBLIC_HEADERS}
)

add_library(jlang-types ${TYPES_SOURCES})
target_include_directories(jlang-types
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/frontend
            ${PROJECT_SOURCE_DIR}/src/misc
            ${PROJECT_SOURCE_DIR}/src/errors
            ${PROJECT_SOURCE_DIR}/src/types
)
install(
    TARGETS
        jlang-types
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${TYPES_PUBLIC_HEADERS}
        DESTINATION include/jlang-types
)

add_executable(test_types test_types.cpp)
target_include_directories(test_types PUBLIC ${PROJECT_SOURCE_DIR}/types)
target_link_libraries(test_types jlang-types jlang-frontend jlang-errors jlang-misc)
add_test(NAME test_types COMMAND test_types)
