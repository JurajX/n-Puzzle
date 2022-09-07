set(
    HDRS
    ${CMAKE_SOURCE_DIR}/src/Puzzle.hpp
)

install(
    FILES ${HDRS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT development
)

add_library(
    puzzle SHARED
    ${CMAKE_SOURCE_DIR}/src/Puzzle.cpp
    ${HDRS}
)
add_compile_options(
    TARGET puzzle
    OPTIM_OPTIONS "-O3"
)

set_target_properties(
    puzzle
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(
    puzzle
    PUBLIC
    fmt::fmt
)

target_include_directories(
    puzzle PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
)
