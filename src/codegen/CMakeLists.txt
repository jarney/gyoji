set(CODEGEN_PUBLIC_HEADERS
        jlang-codegen/jbackend-llvm.hpp
        jlang-codegen/jbackend-llvm-operators.hpp
        jlang-codegen/jbackend-llvm-types.hpp
        jlang-codegen/jsemantics.hpp
)

set(CODEGEN_SOURCES
        jbackend-llvm.cpp
        jbackend-llvm-operators.cpp
        jbackend-llvm-types.cpp
        jsemantics.cpp
        llvm-translation-unit-visitor.cpp
        ${CODEGEN_PUBLIC_HEADERS}
)

add_library(jlang-codegen ${CODEGEN_SOURCES})
target_include_directories(jlang-codegen
    PUBLIC
        ${PROJECT_SOURCE_DIR}/misc
        ${PROJECT_SOURCE_DIR}/errors
        ${PROJECT_SOURCE_DIR}/frontend
        ${PROJECT_SOURCE_DIR}/backend
        ${PROJECT_SOURCE_DIR}/codegen
        ${CMAKE_BINARY_DIR}
	/usr/include/llvm-18
	/usr/include/llvm-c-18
)
install(
    TARGETS
        jlang-codegen
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${CODEGEN_PUBLIC_HEADERS}
        DESTINATION include/jlang-codegen
)

