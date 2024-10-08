set(GTEST_DIR gtest/gtest-10-06-21)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

macro(HEXER_ADD_TEST _name)
  add_executable(${_name} ${_name}.cpp ${LIB_SRCS} ${LAZPERF_SRCS})
  target_link_libraries(${_name} gtest ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${GDAL_LIBRARY} ${H3_LIB_NAME})
  target_include_directories(${_name} PRIVATE ${GDAL_INCLUDE_DIR})
  target_include_directories(${_name} PRIVATE ${GTEST_DIR}/include)
  target_include_directories(${_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
  target_include_directories(${_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  add_test(NAME ${_name} COMMAND ${HEXER_BINARY_DIR}/${_name})
endmacro(HEXER_ADD_TEST)

configure_file(test_main.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/test_main.hpp)
configure_file(${hexer_defines_h} ${CMAKE_CURRENT_BINARY_DIR}/hexer/hexer_defines.h)
add_subdirectory(${GTEST_DIR})

HEXER_ADD_TEST(gridtest)
HEXER_ADD_TEST(pathstest)

