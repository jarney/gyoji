add_subdirectory(misc)
add_subdirectory(context)
add_subdirectory(frontend)
add_subdirectory(mir)
add_subdirectory(analysis)
add_subdirectory(cmdline)
add_subdirectory(codegen)

add_test(NAME test-syntax COMMAND ${CMAKE_SOURCE_DIR}/test-syntax.sh ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})
add_test(NAME test-jcc COMMAND ${CMAKE_SOURCE_DIR}/test-jcc.sh ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})

