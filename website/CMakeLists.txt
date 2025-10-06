
set(WEBSITE_ROOT_FILES
    index.html
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
