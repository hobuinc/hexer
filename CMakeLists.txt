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

  set(CMAKE_BUILD_TYPE Debug CACHE STRING 
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel" FORCE)
  endif()
    message(STATUS "Setting Hexer build type - ${CMAKE_BUILD_TYPE}")
endif()

# TODO: Still testing the output paths --mloskot
set(HEXER_BUILD_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

# Output directory in which to build RUNTIME target files.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY}) 


set(BUILD_STATIC FALSE CACHE BOOL "Build LASzip statically" )
###############################################################################
# Platform and compiler specific settings

if(WIN32)
  if (MSVC)
    if (MSVC80 OR MSVC90 OR MSVC10)
      add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
      add_definitions(-D_CRT_SECURE_NO_WARNINGS)
      add_definitions(-D_CRT_NONSTDC_NO_WARNING)
      add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    endif()
  endif()
endif()


find_package(Boost 1.48 )
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})

  # make these available for the user to set.
  mark_as_advanced(CLEAR Boost_INCLUDE_DIR) 
  mark_as_advanced(CLEAR Boost_LIBRARY_DIRS) 
  link_directories(${Boost_LIBRARY_DIRS})

  # Default to using static, multithreaded libraries for 
  # linking under MSVC. 
  if(WIN32)
    if (MSVC)
      set(Boost_USE_STATIC_LIBS   ON)
      set(Boost_USE_MULTITHREADED ON)
    endif(MSVC)
  endif(WIN32)
  

  include_directories( ${Boost_INCLUDE_DIRS}/)

endif(Boost_FOUND)

set(WITH_DRAWING FALSE CACHE BOOL "Choose to build Cairo-based SVG drawing")

if(WITH_DRAWING)
    
    find_package(cairo)
endif(WITH_DRAWING)

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

# Locations are changeable by user to customize layout of laszip installation
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