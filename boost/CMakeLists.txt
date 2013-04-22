# These defines are conditionally set in the Jamfiles but I did nothing to try
# to perpetuate them in this build:
#   DATE_TIME_INLINE
#   BOOST_THREAD_POSIX
#

include_directories(".")

if(WIN32)
    #add_definitions(-DBOOST_TEST_DYN_LINK)
    #add_definitions(-DBOOST_TEST_NO_MAIN)
	
	# disable compiler warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
	
else()
    
	add_definitions(-DBOOST_TEST_DYN_LINK)

endif()


set(HEXERBOOST_SOURCES
    ./libs/date_time/src/gregorian/date_generators.cpp
    ./libs/date_time/src/gregorian/greg_month.cpp
    ./libs/date_time/src/gregorian/greg_weekday.cpp
    ./libs/date_time/src/gregorian/gregorian_types.cpp
    ./libs/date_time/src/posix_time/posix_time_types.cpp
    ./libs/exception/src/clone_current_exception_non_intrusive.cpp
    ./libs/filesystem/src/path_traits.cpp
    ./libs/filesystem/src/unique_path.cpp
    ./libs/filesystem/src/codecvt_error_category.cpp
    ./libs/filesystem/src/path.cpp
    ./libs/filesystem/src/operations.cpp
    ./libs/filesystem/src/utf8_codecvt_facet.cpp
    ./libs/iostreams/src/file_descriptor.cpp
    ./libs/iostreams/src/mapped_file.cpp
    ./libs/program_options/src/cmdline.cpp
    ./libs/program_options/src/config_file.cpp
    ./libs/program_options/src/convert.cpp
    ./libs/program_options/src/options_description.cpp
    ./libs/program_options/src/parsers.cpp
    ./libs/program_options/src/positional_options.cpp
    ./libs/program_options/src/split.cpp
    ./libs/program_options/src/utf8_codecvt_facet.cpp
    ./libs/program_options/src/value_semantic.cpp
    ./libs/program_options/src/variables_map.cpp
    ./libs/program_options/src/winmain.cpp
)
if(WIN32)
  list(APPEND HEXERBOOST_SOURCES
       ./libs/thread/src/win32/thread.cpp
       ./libs/filesystem/src/windows_file_codecvt.hpp
       ./libs/filesystem/src/windows_file_codecvt.cpp
       ./libs/filesystem/src/codecvt_error_category.cpp
       ./libs/thread/src/future.cpp
       ./libs/thread/src/win32/tss_dll.cpp
       ./libs/thread/src/win32/tss_pe.cpp)
else()
  list(APPEND HEXERBOOST_SOURCES
       ./libs/thread/src/future.cpp
       ./libs/thread/src/pthread/once.cpp
       ./libs/thread/src/pthread/thread.cpp)
endif(WIN32)

add_library(${HEXERBOOST_LIB_NAME} STATIC ${HEXERBOOST_SOURCES})

