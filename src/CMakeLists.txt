set(HEXER_HEADERS_DIR ../include/hexer)

set(HEXER_HPP
  "${HEXER_HEADERS_DIR}/Draw.hpp"
  "${HEXER_HEADERS_DIR}/HexGrid.hpp"
  "${HEXER_HEADERS_DIR}/HexInfo.hpp"
  "${HEXER_HEADERS_DIR}/HexIter.hpp"
  "${HEXER_HEADERS_DIR}/Hexagon.hpp"
  "${HEXER_HEADERS_DIR}/Mathpair.hpp"
  "${HEXER_HEADERS_DIR}/Path.hpp"
  "${HEXER_HEADERS_DIR}/Processor.hpp"
  "${HEXER_HEADERS_DIR}/Segment.hpp"
  "${HEXER_HEADERS_DIR}/gitsha.h"

 )

set(HEXER_CPP
    Draw.cpp
    HexGrid.cpp
    Hexagon.cpp
    Path.cpp
    Processor.cpp
    Segment.cpp
    "${PROJECT_BINARY_DIR}/src/gitsha.cpp"
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


# Standard include directory of Hexer library
include_directories(../include ../include/hexer . ${GDAL_INCLUDE_DIR} ${CAIRO_INCLUDE_DIR})


    add_library(${HEXER_LIB_NAME} SHARED ${HEXER_SOURCES})



target_link_libraries(  ${HEXER_LIB_NAME}
                        ${CAIRO_LIBRARIES}
                        ${GDAL_LIBRARY})


set_target_properties(${HEXER_LIB_NAME}
  PROPERTIES SOVERSION "${HEXER_LIB_SOVERSION}" )

###############################################################################
# Targets installation

install(TARGETS ${HEXER_LIB_NAME}
  RUNTIME DESTINATION ${HEXER_BIN_DIR}
  LIBRARY DESTINATION ${HEXER_LIB_DIR}
  ARCHIVE DESTINATION ${HEXER_LIB_DIR})

install(DIRECTORY ${HEXER_HEADERS_DIR}/
  DESTINATION ${HEXER_INCLUDE_DIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
