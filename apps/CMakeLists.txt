
include_directories(
    .
    ../include
    ../include/hexer)

set(CURSE curse)

if(WIN32)
if(NOT BUILD_STATIC)
    add_definitions("-DHEXER_DLL_IMPORT=1")
endif()
endif()


if(CURSE)
    add_executable(${CURSE} curse.cpp lasfile.hpp las.hpp las.cpp OGR.hpp OGR.cpp)
    target_link_libraries(${CURSE} ${HEXER_LIB_NAME})
endif()

install(TARGETS ${HEXER_UTILITIES}
    RUNTIME DESTINATION ${HEXER_BIN_DIR}
    LIBRARY DESTINATION ${HEXER_LIB_DIR}
    ARCHIVE DESTINATION ${HEXER_LIB_DIR})
