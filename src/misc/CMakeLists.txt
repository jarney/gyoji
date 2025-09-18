
set(MISC_PUBLIC_HEADERS
    jlang-misc/jstring.hpp
    jlang-misc/test.hpp
    jlang-misc/xml.hpp
)
set(MISC_SOURCES
    jstring.cpp
    xml.cpp
    ${MISC_PUBLIC_HEADERS}
)

add_library(jlang-misc ${MISC_SOURCES})
target_include_directories(jlang-misc
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/misc
)
install(
    TARGETS
        jlang-misc
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${MISC_PUBLIC_HEADERS}
        DESTINATION include/jlang-misc
)

add_executable(test_jstring test_jstring.cpp)
target_include_directories(test_jstring PUBLIC ${PROJECT_SOURCE_DIR}/misc)
target_link_libraries(test_jstring jlang-misc)
add_test(NAME test_jstring COMMAND test_jstring)

add_executable(test_xml test_xml.cpp)
target_include_directories(test_xml PUBLIC ${PROJECT_SOURCE_DIR}/misc)
target_link_libraries(test_xml jlang-misc)
add_test(NAME test_xml COMMAND test_xml)

