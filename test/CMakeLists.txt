#########################################################################
#
#  CMAKE
#
#########################################################################

###
#  Compile Options
#  ---------------
#  Compilation options to apply to the targets.
###
add_compile_options(-fsanitize=address -fno-omit-frame-pointer)

###
#  Linker Options
#  --------------
#  Linker options to apply to the targets.
###
add_link_options(-fsanitize=address -fno-omit-frame-pointer)

#########################################################################
#
#  PATHS
#
#########################################################################

###
#  Source Paths
#  ------------
#  The paths to the source files of the tests.
###
set(TEST_SRC_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/")

set(TEST_SRC_LIST_THREAD_POOL "${TEST_SRC_DIR_PATH}/CppTask_ThreadPool_Tests.cpp")
set(TEST_SRC_LIST_TASK        "${TEST_SRC_DIR_PATH}/CppTask_Task_Tests.cpp")
				 
#########################################################################
#
#  TARGET
#
#########################################################################

###
#  Target
#  ------
#  The target(s) to build.
###
add_executable(CppTask_Test_ThreadPool ${TEST_SRC_LIST_THREAD_POOL})
add_executable(CppTask_Test_Task       ${TEST_SRC_LIST_TASK})

###
#  Dependencies
#  ------------
#  Dependencies required by this project.
###
find_library(GTEST_LIBRARY NAMES gtest REQUIRED)
find_library(GMOCK_LIBRARY NAMES gmock REQUIRED)

set(TEST_LIB_LIST ${GTEST_LIBRARY}
                  ${GMOCK_LIBRARY}
                  libcpptask_Static)

target_link_libraries(CppTask_Test_ThreadPool ${TEST_LIB_LIST})
target_link_libraries(CppTask_Test_Task       ${TEST_LIB_LIST})

###
#  Tests
#  -----
#  Executables to run as tests.
###
add_test(CppTask_Test_ThreadPool CppTask_Test_ThreadPool)
add_test(CppTask_Test_Task       CppTask_Test_Task)