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

# We need flex and bison
# in order to build the parser.
find_package(BISON)
find_package(FLEX)

enable_testing()

add_subdirectory(misc)
add_subdirectory(frontend)
add_subdirectory(backend)
add_subdirectory(codegen)

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

target_link_libraries(cparse
    PUBLIC
        jlang-codegen
        jlang-backend
        jlang-frontend
        jlang-misc
	-L/usr/lib/llvm-18/lib -lLLVM-18
)


install(
    TARGETS
        cparse
        RUNTIME DESTINATION bin
)
