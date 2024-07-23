###############################################################################
# CMake settings
cmake_minimum_required(VERSION 3.5.0)

set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CLEAR CMAKE_VERBOSE_MAKEFILE )

###############################################################################
# Project setup
project(HEXER VERSION 1.90 LANGUAGES CXX)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
include(${HEXER_SOURCE_DIR}/cmake/hexer_utils.cmake)

set(HEXER_VERSION 1.4.0 CACHE STRING "Hexer version")

set(CMAKE_MODULE_PATH "${HEXER_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

DISSECT_VERSION()
GET_OS_INFO()
SET_INSTALL_DIRS()

SET(HEXER_LIB_SOVERSION "1.1.0")

# Name of C++ library
set(HEXER_LIB_NAME hexer)

message(STATUS "Configuring Hexer library - ${HEXER_LIB_NAME}")

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

# Path to additional CMake modules
if (CMAKE_MAJOR_VERSION GREATER 2)
    cmake_policy(SET CMP0042 NEW) # osx rpath
endif()

###############################################################################
# Platform and compiler specific settings

if(WIN32)
  if (MSVC)
    if (MSVC80 OR MSVC90 OR MSVC10 OR MSVC11 OR MSVC12 OR MSVC13 OR MSVC14 OR MSVC15)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
      add_definitions(-D_CRT_NONSTDC_NO_WARNING)
      add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	  set(HEXER_COMPILER_MSVC 1)
    endif()
  endif()
else()
endif()

# from http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

set(WITH_DRAWING FALSE CACHE BOOL "Choose to build Cairo-based SVG drawing")
if(WITH_DRAWING)
    find_package(Cairo)
    set(HEXER_HAVE_CAIRO 1)
endif(WITH_DRAWING)

find_package(GDAL 1.9.0)
if (GDAL_FOUND)
    set(HEXER_HAVE_GDAL 1)
endif()

if (NOT WIN32)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

###############################################################################
# Installation settings

set(HEXER_OUTPUT_LIB_DIR ${HEXER_BINARY_DIR}/${LIB_INSTALL_DIR})
set(HEXER_OUTPUT_BIN_DIR ${HEXER_BINARY_DIR}/${BIN_INSTALL_DIR})
make_directory(${HEXER_OUTPUT_LIB_DIR})
make_directory(${HEXER_OUTPUT_BIN_DIR})
if(WIN32)
  foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} CONFIG)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${HEXER_OUTPUT_LIB_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${HEXER_OUTPUT_BIN_DIR}")
    # ---[ Windows requires DLLs (shared libraries) to be installed in the same directory as executables
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${HEXER_OUTPUT_BIN_DIR}")
  endforeach(config)
else(WIN32)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${HEXER_OUTPUT_LIB_DIR}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${HEXER_OUTPUT_BIN_DIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTOYR "${HEXER_OUTPUT_LIB_DIR}")
endif(WIN32)

###############################################################################
# Processing of project directories

set(hexer_defines_h_in "${CMAKE_CURRENT_SOURCE_DIR}/hexer_defines.h.in")
set(hexer_defines_h "${CMAKE_CURRENT_BINARY_DIR}/include/hexer/hexer_defines.h")
include(CheckIncludeFiles)
check_include_files(execinfo.h HEXER_HAVE_EXECINFO_H)
configure_file(${hexer_defines_h_in} ${hexer_defines_h})
HEXER_ADD_INCLUDES("" "" ${hexer_defines_h})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

file(GLOB LAZPERF_SRCS
    lazperf/*.cpp
    lazperf/detail/*.cpp
)

file(GLOB LIB_SRCS
    hexer/*.cpp
)

file(GLOB APP_SRCS
    apps/*.cpp
)

add_executable(curse ${APP_SRCS} ${LIB_SRCS} ${LAZPERF_SRCS})

target_include_directories(curse
    PRIVATE
        .
        ${GDAL_INCLUDE_DIR}
)

target_link_libraries(curse
    PRIVATE
        ${CAIRO_LIBRARIES}
        ${GDAL_LIBRARY}
)

enable_testing()
add_subdirectory(test)
