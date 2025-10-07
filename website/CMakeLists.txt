
function(html_process html_file)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/website/${html_file}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${html_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND cat header.html ${CMAKE_CURRENT_SOURCE_DIR}/${html_file} footer.html >${CMAKE_BINARY_DIR}/website/${html_file}
    )
endfunction()

html_process(index.html)
html_process(architecture.html)
html_process(help-wanted.html)
html_process(goals.html)
html_process(doxygen.html)
html_process(code-coverage.html)
html_process(support.html)

add_custom_target(website
	DEPENDS
            docs
            gcov-report
	    ${CMAKE_BINARY_DIR}/website/index.html
	    ${CMAKE_BINARY_DIR}/website/help-wanted.html
	    ${CMAKE_BINARY_DIR}/website/architecture.html
	    ${CMAKE_BINARY_DIR}/website/goals.html
	    ${CMAKE_BINARY_DIR}/website/doxygen.html
	    ${CMAKE_BINARY_DIR}/website/code-coverage.html
	    ${CMAKE_BINARY_DIR}/website/support.html
)

set(WEBSITE_ROOT_FILES
    ${CMAKE_BINARY_DIR}/website/index.html
    ${CMAKE_BINARY_DIR}/website/help-wanted.html
    ${CMAKE_BINARY_DIR}/website/architecture.html
    ${CMAKE_BINARY_DIR}/website/goals.html
    ${CMAKE_BINARY_DIR}/website/doxygen.html
    ${CMAKE_BINARY_DIR}/website/code-coverage.html
    ${CMAKE_CURRENT_SOURCE_DIR}/styles.css
)

set(WEBSITE_DOC_HTML_DIRECTORY
  "${CMAKE_BINARY_DIR}/doc_doxygen/html/"
)

set(WEBSITE_GCOV_DIRECTORY
  "${CMAKE_BINARY_DIR}/gcov/"
)

install(
    FILES
        ${WEBSITE_ROOT_FILES}
        DESTINATION website
)

install(
	DIRECTORY ${WEBSITE_DOC_HTML_DIRECTORY}
        DESTINATION website/doc_doxygen/html
)

install(
	DIRECTORY ${WEBSITE_GCOV_DIRECTORY}
        DESTINATION website/gcov
)
