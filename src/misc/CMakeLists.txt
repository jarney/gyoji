# Copyright 2025 Jonathan S. Arney
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      https://github.com/jarney/gyoji/blob/master/LICENSE
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

set(MISC_PUBLIC_HEADERS
    gyoji-misc/input-source.hpp
    gyoji-misc/input-source-file.hpp
    gyoji-misc/jstring.hpp
    gyoji-misc/jstring.hpp
    gyoji-misc/getopt.hpp
    gyoji-misc/subprocess.hpp
    gyoji-misc/test.hpp
    gyoji-misc/xml.hpp
)
set(MISC_SOURCES
    jstring.cpp
    getopt.cpp
    subprocess.cpp
    input-source.cpp
    input-source-file.cpp
    xml.cpp
    ${MISC_PUBLIC_HEADERS}
)

add_library(gyoji-misc ${MISC_SOURCES})
target_include_directories(gyoji-misc
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/misc
)
install(
    TARGETS
        gyoji-misc
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${MISC_PUBLIC_HEADERS}
        DESTINATION include/gyoji-misc
)

add_executable(test_jstring test_jstring.cpp)
target_include_directories(test_jstring PUBLIC ${PROJECT_SOURCE_DIR}/misc)
target_link_libraries(test_jstring gyoji-misc)
add_test(NAME test_jstring COMMAND test_jstring)

add_executable(test_xml test_xml.cpp)
target_include_directories(test_xml PUBLIC ${PROJECT_SOURCE_DIR}/misc)
target_link_libraries(test_xml gyoji-misc)
add_test(NAME test_xml COMMAND test_xml)

add_executable(test_getopt test_getopt.cpp)
target_include_directories(test_getopt PUBLIC ${PROJECT_SOURCE_DIR}/misc)
target_link_libraries(test_getopt gyoji-misc)
add_test(NAME test_getopt COMMAND test_getopt)

add_executable(test_subprocess test_subprocess.cpp)
target_include_directories(test_subprocess PUBLIC ${PROJECT_SOURCE_DIR}/misc)
target_link_libraries(test_subprocess gyoji-misc)
add_test(NAME test_subprocess COMMAND test_subprocess)
