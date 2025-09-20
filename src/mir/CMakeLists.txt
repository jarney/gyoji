
set(TYPES_PUBLIC_HEADERS
    jlang-mir/types.hpp
)
set(TYPES_SOURCES
    types.cpp
    ${TYPES_PUBLIC_HEADERS}
)

add_library(jlang-mir ${TYPES_SOURCES})
target_include_directories(jlang-mir
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/frontend
            ${PROJECT_SOURCE_DIR}/src/misc
            ${PROJECT_SOURCE_DIR}/src/context
            ${PROJECT_SOURCE_DIR}/src/mir
)
install(
    TARGETS
        jlang-mir
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${TYPES_PUBLIC_HEADERS}
        DESTINATION include/jlang-mir
)

add_executable(test_types test_types.cpp)
target_include_directories(test_types PUBLIC ${PROJECT_SOURCE_DIR}/types)
target_link_libraries(test_types jlang-mir jlang-frontend jlang-context jlang-misc)
add_test(NAME test_types COMMAND test_types ${CMAKE_SOURCE_DIR})

