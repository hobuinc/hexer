# These defines are conditionally set in the Jamfiles but I did nothing to try
# to perpetuate them in this build:
#   DATE_TIME_INLINE
#   BOOST_THREAD_POSIX
#

include_directories(".")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(WIN32)
    #add_definitions(-DBOOST_TEST_DYN_LINK)
    #add_definitions(-DBOOST_TEST_NO_MAIN)
	
	# disable compiler warnings
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /w")
	
else()
    
	add_definitions(-DBOOST_TEST_DYN_LINK)

endif()


set(HEXERBOOST_SOURCES
    ./libs/smart_ptr/src/sp_collector.cpp
    ./libs/smart_ptr/src/sp_debug_hooks.cpp
    ./libs/regex/src/c_regex_traits.cpp
    ./libs/regex/src/cpp_regex_traits.cpp
    ./libs/regex/src/cregex.cpp
    ./libs/regex/src/fileiter.cpp
    ./libs/regex/src/instances.cpp
    ./libs/regex/src/internals.hpp
    ./libs/regex/src/posix_api.cpp
    ./libs/regex/src/regex.cpp
    ./libs/regex/src/regex_raw_buffer.cpp
    ./libs/regex/src/regex_traits_defaults.cpp
    ./libs/regex/src/static_mutex.cpp
    ./libs/regex/src/usinstances.cpp
    ./libs/filesystem/src/path_traits.cpp
    ./libs/filesystem/src/unique_path.cpp
    ./libs/filesystem/src/codecvt_error_category.cpp
    ./libs/filesystem/src/path.cpp
    ./libs/filesystem/src/operations.cpp
    ./libs/filesystem/src/utf8_codecvt_facet.cpp    
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
    ./libs/exception/src/clone_current_exception_non_intrusive.cpp
    ./libs/system/src/error_code.cpp
    ./libs/date_time/src/gregorian/date_generators.cpp
    ./libs/date_time/src/gregorian/greg_month.cpp
    ./libs/date_time/src/gregorian/greg_weekday.cpp
    ./libs/date_time/src/gregorian/gregorian_types.cpp
    ./libs/date_time/src/posix_time/posix_time_types.cpp
    ./libs/test/src/compiler_log_formatter.cpp
    ./libs/test/src/debug.cpp
    ./libs/test/src/exception_safety.cpp
    ./libs/test/src/execution_monitor.cpp
    ./libs/test/src/framework.cpp
    ./libs/test/src/interaction_based.cpp
    ./libs/test/src/logged_expectations.cpp
    ./libs/test/src/plain_report_formatter.cpp
    ./libs/test/src/progress_monitor.cpp
    ./libs/test/src/results_collector.cpp
    ./libs/test/src/results_reporter.cpp
    ./libs/test/src/test_main.cpp
    ./libs/test/src/test_tools.cpp
    ./libs/test/src/unit_test_log.cpp
    ./libs/test/src/unit_test_main.cpp
    ./libs/test/src/unit_test_monitor.cpp
    ./libs/test/src/unit_test_parameters.cpp
    ./libs/test/src/unit_test_suite.cpp
    ./libs/test/src/xml_log_formatter.cpp
    ./libs/test/src/xml_report_formatter.cpp        
)
if(WIN32)
  list(APPEND HEXERBOOST_SOURCES
       ./libs/regex/src/w32_regex_traits.cpp
       ./libs/filesystem/src/windows_file_codecvt.hpp
       ./libs/filesystem/src/windows_file_codecvt.cpp
       ./libs/filesystem/src/codecvt_error_category.cpp
       ./libs/thread/src/future.cpp
       ./libs/thread/src/win32/tss_pe.cpp)
else()
  list(APPEND HEXERBOOST_SOURCES 
      ./libs/thread/src/future.cpp
      ./libs/thread/src/pthread/once.cpp
      ./libs/thread/src/pthread/thread.cpp)
endif(WIN32)

add_library(${HEXERBOOST_LIB_NAME} STATIC ${HEXERBOOST_SOURCES})

