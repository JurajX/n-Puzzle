add_exe(
    TEST
    NAME test_puzzle
    SRCS ${CMAKE_CURRENT_SOURCE_DIR}/resources/test_data.hpp
    LINK_LIBRARIES puzzle
    OPTIM_OPTIONS "-O3"
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED
    CXX_EXTENSIONS_OFF
)
