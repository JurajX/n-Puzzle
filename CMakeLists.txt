cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "Directed Graph Linearisation")
project(
    ${PROJECT_NAME} VERSION 0.0.0
    DESCRIPTION "A brain teaser."
    LANGUAGES C CXX
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(initiate)
initiate()

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)

if(MAKE_DOCS)
    if(NOT DEFINED MAKE_DOC_PATH)
        set(MAKE_DOC_PATH ${CMAKE_CURRENT_LIST_DIR}/build/docs)
    endif()

    add_docs(
        NAME ${PROJECT_NAME}
        FILE_DIRS src
        EXAMPLE_PATH examples
        OUTPUT_DIRECTORY ${MAKE_DOC_PATH}
        ENABLE_PREPROCESSING MACRO_EXPANSION
    )
endif(MAKE_DOCS)
