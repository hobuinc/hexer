project(hexer)


SET(HEXER_VERSION_MAJOR "1")
SET(HEXER_VERSION_MINOR "0")
SET(HEXER_VERSION_PATCH "0")
set(VERSION "${HEXER_VERSION_MAJOR}.${HEXER_VERSION_MINOR}.${HEXER_VERSION_PATCH}")
SET(HEXER_LIB_SOVERSION "1.0.0")

# Name of C++ library
set(HEXER_LIB_NAME hexer)

message(STATUS "Configuring Hexer library - ${HEXER_LIB_NAME}")

set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)

###############################################################################
# CMake settings
cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_COLOR_MAKEFILE ON)

# Allow advanced users to generate Makefiles printing detailed commands
mark_as_advanced(CLEAR CMAKE_VERBOSE_MAKEFILE )

# Path to additional CMake modules
set(CMAKE_MODULE_PATH ${hexer_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

###############################################################################
# General build settings

# Default to release mode 
if(NOT MSVC_IDE)
  if(NOT CMAKE_BUILD_TYPE)

  set(CMAKE_BUILD_TYPE Release CACHE STRING 
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
  endif()
    message(STATUS "Setting Hexer build type - ${CMAKE_BUILD_TYPE}")
endif()

# TODO: Still testing the output paths --mloskot
set(HEXER_BUILD_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Output directory in which to build RUNTIME target files.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY}) 


set(BUILD_STATIC FALSE CACHE BOOL "Build Hexer statically" )
###############################################################################
# Platform and compiler specific settings

if(WIN32)
  if (MSVC)
    if (MSVC80 OR MSVC90 OR MSVC10 OR MSVC11)
      add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
      add_definitions(-D_CRT_NONSTDC_NO_WARNING)
      add_definitions(-D_SCL_SECURE_NO_WARNINGS)
	  set(HEXER_COMPILER_MSVC 1)
    endif()
  endif()
else()
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
endif()



set(BOOST_ROOT ${PROJECT_SOURCE_DIR}/boost)

# find_package would normally set the following variables
set(Boost_FOUND TRUE)
set(Boost_INCLUDE_DIRS ${BOOST_ROOT})

set(HEXERBOOST_LIB_NAME "hexerboost" CACHE STRING "use embedded boost")
mark_as_advanced(FORCE HEXERBOOST_LIB_NAME)

if (WIN32)
  set(Boost_LIBRARIES "${HEXERBOOST_LIB_NAME}.lib")
  link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
else()
  # Some Cmake versions are borken here. See
  # http://www.cmake.org/Bug/view.php?id=12258#c26851
  # set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
  # include_directories(BEFORE SYSTEM ${Boost_INCLUDE_DIRS}/)
  set(Boost_LIBRARIES "${HEXERBOOST_LIB_NAME}")
endif()

# this prevents auto-linking pragmas from depending on wrong lib names
add_definitions(-DBOOST_ALL_NO_LIB)

include_directories( ${Boost_INCLUDE_DIRS}/)
add_subdirectory(boost)


set(WITH_DRAWING FALSE CACHE BOOL "Choose to build Cairo-based SVG drawing")
if(WITH_DRAWING)
    find_package(cairo)
    set(HEXER_HAVE_CAIRO 1)
endif(WITH_DRAWING)

set(WITH_GDAL FALSE CACHE BOOL "Use GDAL/OGR to export hexagon density surfaces and/or paths")
if(WITH_GDAL)
    find_package(GDAL 1.9)
    set(HEXER_HAVE_GDAL 1)
endif(WITH_GDAL)

###############################################################################
# Installation settings

if(WIN32)
  set(DEFAULT_LIB_SUBDIR lib)
  set(DEFAULT_DATA_SUBDIR .)
  set(DEFAULT_INCLUDE_SUBDIR include)

  if (MSVC)
    set(DEFAULT_BIN_SUBDIR bin)
  else()
    set(DEFAULT_BIN_SUBDIR .)
  endif()
else()
  # Common locations for Unix and Mac OS X
  set(DEFAULT_BIN_SUBDIR bin)
  set(DEFAULT_LIB_SUBDIR lib)
  set(DEFAULT_INCLUDE_SUBDIR include)
endif()

# Locations are changeable by user to customize layout of Hexer installation
# (default values are platform-specific)
set(HEXER_BIN_SUBDIR ${DEFAULT_BIN_SUBDIR} CACHE STRING
  "Subdirectory where executables will be installed")
set(HEXER_LIB_SUBDIR ${DEFAULT_LIB_SUBDIR} CACHE STRING
  "Subdirectory where libraries will be installed")
set(HEXER_INCLUDE_SUBDIR ${DEFAULT_INCLUDE_SUBDIR} CACHE STRING
  "Subdirectory where header files will be installed")
set(HEXER_DATA_SUBDIR ${DEFAULT_DATA_SUBDIR} CACHE STRING
  "Subdirectory where data will be installed")

# Mark *_SUBDIR variables as advanced and dedicated to use by power-users only.
mark_as_advanced(HEXER_BIN_SUBDIR
  HEXER_LIB_SUBDIR HEXER_INCLUDE_SUBDIR HEXER_DATA_SUBDIR)

# Full paths for the installation
set(HEXER_BIN_DIR ${HEXER_BIN_SUBDIR})
set(HEXER_LIB_DIR ${HEXER_LIB_SUBDIR})
set(HEXER_INCLUDE_DIR ${HEXER_INCLUDE_SUBDIR})

###############################################################################
# Processing of project directories

add_subdirectory(src)
add_subdirectory(apps)

configure_file(
  "${PROJECT_SOURCE_DIR}/hexer_defines.h.in"
  "${HEXER_INCLUDE_DIR}/hexer/hexer_defines.h")

configure_file(
  "${PROJECT_SOURCE_DIR}/gitsha.cpp.in"
  "${PROJECT_SOURCE_DIR}/src/gitsha.cpp")  