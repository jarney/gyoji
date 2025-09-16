# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.15...4.0)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  JLang
  VERSION 1.0
  LANGUAGES CXX)

option(BUILD_DOC "Build documentation" ON)
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)


# We need flex and bison
# in order to build the parser.
find_package(BISON)
find_package(FLEX)

enable_testing()

add_subdirectory(misc)
add_subdirectory(frontend)
add_subdirectory(backend)
#add_subdirectory(codegen)

add_executable(cparse main.cpp)

target_include_directories(cparse
    PUBLIC
        misc
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
        jlang-misc
	-L/usr/lib/llvm-18/lib -lLLVM-18
)

add_test(NAME test-syntax COMMAND ${CMAKE_SOURCE_DIR}/test-syntax.sh ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR})

install(
    TARGETS
        cparse
        RUNTIME DESTINATION bin
)

