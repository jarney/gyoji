set(CODEGEN_PUBLIC_HEADERS
         gyoji-codegen.hpp
)

set(CODEGEN_SOURCES
        codegen.cpp
        gyoji-codegen-llvm.cpp
        ${CODEGEN_PUBLIC_HEADERS}
)

add_library(gyoji-codegen ${CODEGEN_SOURCES})
target_include_directories(gyoji-codegen
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
        gyoji-codegen
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${CODEGEN_PUBLIC_HEADERS}
        DESTINATION include/gyoji-codegen
)

