
set(TYPES_PUBLIC_HEADERS
    gyoji-mir.hpp
    gyoji-mir/functions.hpp
    gyoji-mir/types.hpp
    gyoji-mir/symbols.hpp
    gyoji-mir/operations.hpp
)
set(TYPES_SOURCES
    mir.cpp
    functions.cpp
    types.cpp
    type.cpp
    type-member.cpp
    symbols.cpp
    operation.cpp
    ${TYPES_PUBLIC_HEADERS}
)

add_library(gyoji-mir ${TYPES_SOURCES})
target_include_directories(gyoji-mir
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/frontend
            ${PROJECT_SOURCE_DIR}/src/misc
            ${PROJECT_SOURCE_DIR}/src/context
            ${PROJECT_SOURCE_DIR}/src/mir
)
install(
    TARGETS
        gyoji-mir
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${TYPES_PUBLIC_HEADERS}
        DESTINATION include/gyoji-mir
)

