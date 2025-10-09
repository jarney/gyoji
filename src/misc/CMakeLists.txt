
set(MISC_PUBLIC_HEADERS
    gyoji-misc/input-source.hpp
    gyoji-misc/input-source-file.hpp
    gyoji-misc/jstring.hpp
    gyoji-misc/jstring.hpp
    gyoji-misc/test.hpp
    gyoji-misc/xml.hpp
)
set(MISC_SOURCES
    jstring.cpp
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

