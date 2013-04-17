set(HEXER_HEADERS_DIR ../include/hexer)

set(HEXER_HPP
  ${HEXER_HEADERS_DIR}/Draw.hpp
  ${HEXER_HEADERS_DIR}/GridInfo.hpp
  ${HEXER_HEADERS_DIR}/HexGrid.hpp
  ${HEXER_HEADERS_DIR}/Hexagon.hpp
  ${HEXER_HEADERS_DIR}/Mathpair.hpp
  ${HEXER_HEADERS_DIR}/Path.hpp
  ${HEXER_HEADERS_DIR}/Processor.hpp
  ${HEXER_HEADERS_DIR}/Segment.hpp
  
 )

set(HEXER_CPP
    Draw.cpp
    HexGrid.cpp
    Hexagon.cpp
    Path.cpp
    Processor.cpp
    Segment.cpp
)

set(HEXER_SOURCES
  ${HEXER_HPP}
  ${HEXER_CPP}
  )

if(WIN32)
if(NOT BUILD_STATIC)
    add_definitions("-DHEXER_DLL_EXPORT=1")
endif()
endif()

# Group source files for IDE source explorers (e.g. Visual Studio)
source_group("CMake Files" FILES CMakeLists.txt)
source_group("Header Files" FILES ${HEXER_HPP})
source_group("Source Files" FILES ${HEXER_CPP})

# Standard include directory of Hexer library
include_directories(../include ../include/hexer .)


if(BUILD_STATIC)
    add_library(${HEXER_LIB_NAME} STATIC ${HEXER_SOURCES})
else()
    add_library(${HEXER_LIB_NAME} SHARED ${HEXER_SOURCES})
endif()

target_link_libraries( ${HEXER_LIB_NAME} ${CAIRO_LIBRARIES})

set_target_properties(${HEXER_LIB_NAME}
  PROPERTIES SOVERSION "${HEXER_LIB_SOVERSION}" )
      
###############################################################################
# Targets installation

if (APPLE)
  set_target_properties(
    ${HEXER_LIB_NAME}
    PROPERTIES
    INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
endif()

install(TARGETS ${HEXER_LIB_NAME} 
  RUNTIME DESTINATION ${HEXER_BIN_DIR}
  LIBRARY DESTINATION ${HEXER_LIB_DIR}
  ARCHIVE DESTINATION ${HEXER_LIB_DIR})

install(DIRECTORY ${HEXER_HEADERS_DIR}
  DESTINATION ${HEXER_INCLUDE_DIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
