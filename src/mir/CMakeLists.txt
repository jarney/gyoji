
set(TYPES_PUBLIC_HEADERS
    jlang-mir.hpp
    jlang-mir/functions.hpp
    jlang-mir/types.hpp
    jlang-mir/symbols.hpp
    jlang-mir/operations.hpp
)
set(TYPES_SOURCES
    mir.cpp
    functions.cpp
    types.cpp
    type.cpp
    type-member.cpp
    symbols.cpp
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

