set(GTEST_DIR gtest/gtest-10-06-21)

set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

macro(HEXER_ADD_TEST _name)
  add_executable(${_name} ${_name}.cpp ${LIB_SRCS} ${LAZPERF_SRCS})
  target_link_libraries(${_name} gtest ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${GDAL_LIBRARY})
  target_include_directories(${_name} PRIVATE ${GDAL_INCLUDE_DIR})
  target_include_directories(${_name} PRIVATE ${GTEST_DIR}/include)
  target_include_directories(${_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
  target_include_directories(${_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
  add_test(NAME ${_name} COMMAND ${HEXER_BINARY_DIR}/test/${_name}
    WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../..")
endmacro(HEXER_ADD_TEST)

configure_file(test_main.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/test_main.hpp)
add_subdirectory(${GTEST_DIR})

HEXER_ADD_TEST(gridtest)

