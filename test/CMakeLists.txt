include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

macro(HEXER_ADD_TEST _name)
  add_executable(${_name} ${_name}.cpp ${LIB_SRCS} ${LAZPERF_SRCS})
  target_link_libraries(${_name} PRIVATE GTest::gtest GDAL::GDAL ${H3_LIB_NAME})
  target_include_directories(${_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
  target_include_directories(${_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  add_test(NAME ${_name} COMMAND $<TARGET_FILE:${_name}>)
endmacro(HEXER_ADD_TEST)

configure_file(test_main.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/test_main.hpp)
configure_file(${hexer_defines_h} ${CMAKE_CURRENT_BINARY_DIR}/hexer/hexer_defines.h)

HEXER_ADD_TEST(gridtest)
HEXER_ADD_TEST(pathstest)
