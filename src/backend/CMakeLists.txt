#        jlang-backend/jbackend-format-pretty.hpp
#        jbackend-format-pretty.cpp

set(BACKEND_PUBLIC_HEADERS
        jlang-backend/jbackend.hpp
        jlang-backend/jbackend-format-identity.hpp
        jlang-backend/jbackend-format-tree.hpp
)

set(BACKEND_SOURCES
        jbackend.cpp
        jbackend-format-identity.cpp
        jbackend-format-tree.cpp
        ${BACKEND_PUBLIC_HEADERS}
)
# NOT ready:
#        jbackend-toc.cpp
#        jlang-backend/jbackend-toc.hpp

add_library(jlang-backend ${BACKEND_SOURCES})

target_include_directories(jlang-backend
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/misc
            ${PROJECT_SOURCE_DIR}/src/errors
            ${PROJECT_SOURCE_DIR}/src/frontend
            ${PROJECT_SOURCE_DIR}/src/backend
            ${CMAKE_BINARY_DIR}
    )
install(
    TARGETS
        jlang-backend
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${BACKEND_PUBLIC_HEADERS}
        DESTINATION include/jlang-backend
)

set_source_files_properties(
    jbackend-format-tree.cpp
    PROPERTIES OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/jlang-frontend-built.cpp
)
set_source_files_properties(
    ${CMAKE_BINARY_DIR}/jlang-frontend-built.cpp
    PROPERTIES GENERATED true
)
                            
add_library(jlang-backend-libtmp ${CMAKE_BINARY_DIR}/jlang-frontend-built.cpp)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/jlang-frontend-built.cpp
    DEPENDS jlang-frontend
    COMMAND touch ${CMAKE_BINARY_DIR}/jlang-frontend-built.cpp
)
