project(hexer)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)

include(${hexer_SOURCE_DIR}/cmake/hexer_utils.cmake)

set(HEXER_VERSION 1.2.1 CACHE STRING "Hexer version")

DISSECT_VERSION()
GET_OS_INFO()
SET_INSTALL_DIRS()

SET(HEXER_LIB_SOVERSION "1.0.1")

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
set(CMAKE_MODULE_PATH "${hexer_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

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
set(HEXER_BUILD_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Output directory in which to build RUNTIME target files.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build LIBRARY target files
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY})

# Output directory in which to build ARCHIVE target files.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${HEXER_BUILD_OUTPUT_DIRECTORY}) 

option(HEXER_EMBED_BOOST "Use embedded rather than system boost" FALSE)

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
endif()

# from http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

if (NOT HEXER_EMBED_BOOST)
	find_package(Boost 1.52 COMPONENTS program_options thread iostreams filesystem system unit_test_framework random)
endif()

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})

    # make these available for the user to set.
    mark_as_advanced(CLEAR Boost_INCLUDE_DIR) 
    mark_as_advanced(CLEAR Boost_LIBRARY_DIRS) 
    link_directories(${Boost_LIBRARY_DIRS})
else()
	set(BOOST_ROOT "${PROJECT_SOURCE_DIR}/boost")

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

	include_directories("${Boost_INCLUDE_DIRS}/")
	add_subdirectory(boost)
    message(STATUS "External boost not found, using embedded boost tree")
endif()


set(WITH_DRAWING FALSE CACHE BOOL "Choose to build Cairo-based SVG drawing")
if(WITH_DRAWING)
    find_package(cairo)
    set(HEXER_HAVE_CAIRO 1)
endif(WITH_DRAWING)

find_package(GDAL 1.9.0)
if (GDAL_FOUND)
    set(HEXER_HAVE_GDAL 1)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

###############################################################################
# Installation settings

set(HEXER_OUTPUT_LIB_DIR ${hexer_BINARY_DIR}/${LIB_INSTALL_DIR})
set(HEXER_OUTPUT_BIN_DIR ${hexer_BINARY_DIR}/${BIN_INSTALL_DIR})
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
configure_file(${hexer_defines_h_in} ${hexer_defines_h})
HEXER_ADD_INCLUDES("" "" ${hexer_defines_h})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

configure_file(
  "${PROJECT_SOURCE_DIR}/gitsha.cpp.in"
  "${PROJECT_SOURCE_DIR}/src/gitsha.cpp")

add_subdirectory(src)
add_subdirectory(apps)
