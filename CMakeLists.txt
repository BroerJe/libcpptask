#########################################################################
#
#  CMAKE
#
#########################################################################

###
#  Minimum Version
#  ---------------
#  The CMake version required.
###
cmake_minimum_required(VERSION 3.25)

###
#  CMake Configuration
#  -------------------
#  Configuration settings for CMake.
###
set(CMAKE_CXX_STANDARD 17)

###
#  Compile Options
#  ---------------
#  Compilation options to apply to the targets.
###
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
endif()

###
#  Linker Options
#  --------------
#  Linker options to apply to the targets.
###
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_link_options(-fsanitize=address -fno-omit-frame-pointer)
endif()

###
#  Project Info
#  ------------
#  General simple information about our project.
###
project(libcpptask VERSION 1.0.0
                   DESCRIPTION "A simple C++ async task library"
                   LANGUAGES CXX)

set(LIB_SO_VERSION 1)

###
#  Testing
#  -------
#  Testing flag to run with ctest.
###
enable_testing()

#########################################################################
#
#  PATHS
#
#########################################################################

###
#  Install Paths
#  -------------
#  GNU system paths for installation.
###
include(GNUInstallDirs)

###
#  Source Paths
#  ------------
#  The paths to the source files to use.
###
set(SRC_DIR_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(INCLUDE_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/libcpptask")

set(SRC_LIST_PRIVATE "${SRC_DIR_PATH}/CppTask_Task.cpp"
                     "${SRC_DIR_PATH}/CppTask_ThreadPool.cpp"
                     "${SRC_DIR_PATH}/CppTask_ThreadPool.h")

set(SRC_LIST_PUBLIC "${INCLUDE_DIR_PATH}/CppTask_ITask.h"
                    "${INCLUDE_DIR_PATH}/CppTask_Task.h"
                    "${INCLUDE_DIR_PATH}/CppTask_Exception.h")

###
#  Public API Path
#  ---------------
#  The path to the public API include folder.
###
set(PUBLIC_API_DIR_PATH "${INCLUDE_DIR_PATH}")

###
#  Test Paths
#  ----------
#  The paths to the test files to include.
###
set(TEST_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/test/")

#########################################################################
#
#  TARGET
#
#########################################################################

###
#  Static Library Target
#  ---------------------
#  The target for the static library to build.
###
add_library(${PROJECT_NAME}_Static STATIC ${SRC_LIST_PRIVATE}
                                          ${SRC_LIST_PUBLIC})

set_target_properties(${PROJECT_NAME}_Static
                      PROPERTIES
                      PREFIX "lib"
                      OUTPUT_NAME "cpptask"
                      SUFFIX ".a"
                      VERSION ${PROJECT_VERSION})

###
#  Shared Library Target
#  ---------------------
#  The target for the shared library to build.
###
add_library(${PROJECT_NAME}_Shared SHARED ${SRC_LIST_PRIVATE}
                                          ${SRC_LIST_PUBLIC})

set_target_properties(${PROJECT_NAME}_Shared
                      PROPERTIES
                      PREFIX "lib"
                      OUTPUT_NAME "cpptask"
                      SUFFIX ".so"
                      VERSION ${PROJECT_VERSION}
                      SOVERSION ${LIB_SO_VERSION})

###
#  Preprocessor
#  ------------
#  Add preprocessor defines to the targets.
###
#add_compile_definitions(libcpptask_THREAD_POOL_FORCED_THREAD_COUNT=1)

###
#  Install
#  -------
#  Target installation.
###
install(TARGETS ${PROJECT_NAME}_Shared
                ${PROJECT_NAME}_Static
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${PUBLIC_API_DIR_PATH}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING
        PATTERN "*.h")

#########################################################################
#
#  TEST
#
#########################################################################

###
#  Test
#  ----
#  Include tests.
###
add_subdirectory(${TEST_DIR_PATH})