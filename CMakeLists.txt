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


# 
# We need bison to build the grammar/yacc file.
#
find_package(BISON)
if (BISON_FOUND)
  message("Bison was found...")
else (BISON_FOUND)
  message("Bison is needed to build the grammar.")
endif (BISON_FOUND)

#
# We need flex to build the lexer.
#
find_package(FLEX)
if (FLEX_FOUND)
  message("Flex found...")
else (FLEX_FOUND)
  message("Flex is needed to build the token lexer.")
endif (FLEX_FOUND)

enable_testing()

add_subdirectory(src)

