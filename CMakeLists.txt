project ("mbed-edge" C)
cmake_minimum_required (VERSION 2.8)

SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SET (TARGET_GROUP production CACHE STRING "Build target group")

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set (ROOT_HOME ${CMAKE_CURRENT_LIST_DIR})

find_package (Threads REQUIRED)

# Utility cmake functions
include (cmake/common.cmake)

# Mbed Edge basic configuration
# Set the TRACE_LEVEL before including the basic config file
if (DEFINED ENV{MEPT_BLE_TRACE_LEVEL})
    set (TRACE_LEVEL $ENV{MEPT_BLE_TRACE_LEVEL})
    add_definitions("-DMBED_TRACE_LINE_LENGTH=8192")
endif()
include (cmake/edge_configure.cmake)

# Mbed Edge include directories
include (cmake/edge_include_directories.cmake)


include(git_details.cmake)

ADD_DEFINITIONS(-D__STDC_FORMAT_MACROS)

if (TARGET_GROUP STREQUAL test)
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage")
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-c++14-compat -g -O0 -fprofile-arcs -ftest-coverage")
  SET (CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs")
  SET (CPPUTEST_HOME "${CMAKE_CURRENT_LIST_DIR}/lib/cpputest")
  add_subdirectory (test)
else ()
  if (ENABLE_COVERAGE EQUAL 1)
    message ("Enabling coverage")
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -fprofile-arcs -ftest-coverage")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fprofile-arcs -ftest-coverage")
    SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -lgcov")
  endif ()
endif ()

add_subdirectory (edge-client)
add_subdirectory (lib)
add_subdirectory (edge-core)
add_subdirectory (edge-rpc)
add_subdirectory (pt-client)
add_subdirectory (mept-ble)

