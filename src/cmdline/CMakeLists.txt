set(CMDLINE_LINK_LIBRARIES
        gyoji-frontend
        gyoji-analysis
        gyoji-codegen
        gyoji-mir
        gyoji-context
        gyoji-misc
	-L/usr/lib/llvm-18/lib -lLLVM-18
)

set(CMDLINE_INCLUDE_DIRECTORIES
	/usr/include/llvm-18
	/usr/include/llvm-c-18
)


add_executable(jformat-identity jformat-identity-main.cpp jformat-identity.cpp)
target_include_directories(jformat-identity PUBLIC ${CMDLINE_INCLUDE_DIRECTORIES})
target_link_libraries(jformat-identity PUBLIC ${CMDLINE_LINK_LIBRARIES})

add_executable(jformat-tree jformat-tree-main.cpp jformat-tree.cpp)
target_include_directories(jformat-tree PUBLIC ${CMDLINE_INCLUDE_DIRECTORIES})
target_link_libraries(jformat-tree PUBLIC ${CMDLINE_LINK_LIBRARIES})

add_executable(jns jnamespaces-main.cpp)
target_include_directories(jns PUBLIC ${CMDLINE_INCLUDE_DIRECTORIES})
target_link_libraries(jns PUBLIC ${CMDLINE_LINK_LIBRARIES})

add_executable(jtokenize jtokenize.cpp)
target_include_directories(jtokenize PUBLIC ${CMDLINE_INCLUDE_DIRECTORIES})
target_link_libraries(jtokenize PUBLIC ${CMDLINE_LINK_LIBRARIES})

add_executable(jcc jcc.cpp)
target_include_directories(jcc PUBLIC ${CMDLINE_INCLUDE_DIRECTORIES})
target_link_libraries(jcc PUBLIC ${CMDLINE_LINK_LIBRARIES})

install(
    TARGETS
        jcc
        jformat-identity
        jformat-tree
	jtokenize
        RUNTIME DESTINATION bin
)

