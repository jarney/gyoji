
set(ANALYSIS_PUBLIC_HEADERS
    gyoji-analysis.hpp
)
set(ANALYSIS_SOURCES
    analysis.cpp
    analysis-type-resolution.cpp
    analysis-unreachable.cpp
    analysis-borrow-checker.cpp
    ${ANALYSIS_PUBLIC_HEADERS}
)

add_library(gyoji-analysis ${ANALYSIS_SOURCES})
target_include_directories(gyoji-analysis
    PUBLIC
            ${PROJECT_SOURCE_DIR}/src/misc
            ${PROJECT_SOURCE_DIR}/src/context
            ${PROJECT_SOURCE_DIR}/src/mir
            ${PROJECT_SOURCE_DIR}/src/analysis
)
install(
    TARGETS
        gyoji-analysis
        LIBRARY DESTINATION lib
)
install(
    FILES
        ${ANALYSIS_PUBLIC_HEADERS}
        DESTINATION include/gyoji-analysis
)

add_executable(test_analysis test_analysis.cpp)
target_include_directories(test_analysis
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src/analysis
    )
target_link_libraries(test_analysis gyoji-analysis gyoji-mir gyoji-context gyoji-misc)
add_test(NAME test_analysis COMMAND test_analysis ${CMAKE_SOURCE_DIR})

