set(CODEGEN_PUBLIC_HEADERS
#        jlang-codegen/jbackend-llvm.hpp
#        jlang-codegen/jbackend-llvm-operators.hpp
#        jlang-codegen/jbackend-llvm-types.hpp
#        jlang-codegen/jsemantics.hpp
         jlang-codegen.hpp
)

set(CODEGEN_SOURCES
        codegen.cpp
        jlang-codegen-llvm.cpp
#        jbackend-llvm.cpp
#        jbackend-llvm-operators.cpp
#        jbackend-llvm-types.cpp
#        jsemantics.cpp
#        llvm-translation-unit-visitor.cpp
        ${CODEGEN_PUBLIC_HEADERS}
)

add_library(jlang-codegen ${CODEGEN_SOURCES})
target_include_directories(jlang-codegen
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/misc
        ${PROJECT_SOURCE_DIR}/src/mir
        ${PROJECT_SOURCE_DIR}/src/context
        ${PROJECT_SOURCE_DIR}/src/frontend
        ${PROJECT_SOURCE_DIR}/src/codegen
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

