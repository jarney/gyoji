add_subdirectory(misc)
add_subdirectory(errors)
add_subdirectory(frontend)
add_subdirectory(backend)
#add_subdirectory(codegen)

add_executable(cparse main.cpp)

target_include_directories(cparse
    PUBLIC
        misc
        errors
        frontend
        backend
        codegen
	/usr/include/llvm-18
	/usr/include/llvm-c-18
)

#        jlang-codegen
target_link_libraries(cparse
    PUBLIC
        jlang-backend
        jlang-frontend
        jlang-errors
        jlang-misc
	-L/usr/lib/llvm-18/lib -lLLVM-18
)

add_test(NAME test-syntax COMMAND ${CMAKE_SOURCE_DIR}/test-syntax.sh ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})

install(
    TARGETS
        cparse
        RUNTIME DESTINATION bin
)

