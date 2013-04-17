
include_directories(
    .
    ../include
    ../include/hexer)

set(HEXTEST hextest)

set(HEXER_UTILITIES
    ${HEXER} 
    )
    
if(WIN32)
if(NOT BUILD_STATIC)
    add_definitions("-DHEXER_DLL_IMPORT=1")
endif()
endif()


if(HEXTEST)
    add_executable(${HEXTEST} Tester.cpp lasfile.hpp)
    target_link_libraries(${HEXTEST} ${HEXER_LIB_NAME} )
endif()

install(TARGETS ${HEXER_UTILITIES}
    RUNTIME DESTINATION ${HEXER_BIN_DIR}
    LIBRARY DESTINATION ${HEXER_LIB_DIR}
    ARCHIVE DESTINATION ${HEXER_LIB_DIR})
