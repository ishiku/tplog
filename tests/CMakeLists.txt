# source: https://github.com/pananton/cpp-lib-template/blob/main/tests/CMakeLists.txt

cmake_minimum_required(VERSION 3.28)
project(tplog-tests)

#----------------------------------------------------------------------------------------------------------------------
# general settings and options
#----------------------------------------------------------------------------------------------------------------------

include("../cmake/win_copy_deps_to_target_dir.cmake")
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" ${PROJECT_IS_TOP_LEVEL})

if(${PROJECT_IS_TOP_LEVEL})
    enable_testing()
endif()

#----------------------------------------------------------------------------------------------------------------------
# testing framework
#----------------------------------------------------------------------------------------------------------------------

include(FetchContent)
FetchContent_Declare( 
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        "v1.16.0"
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)

set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(googletest)

#----------------------------------------------------------------------------------------------------------------------
# tests dependencies
#----------------------------------------------------------------------------------------------------------------------

if(${PROJECT_IS_TOP_LEVEL})
    find_package(tplog REQUIRED)
endif()

#----------------------------------------------------------------------------------------------------------------------
# tests sources
#----------------------------------------------------------------------------------------------------------------------

set(sources
    dummy_test.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

#----------------------------------------------------------------------------------------------------------------------
# tests target
#----------------------------------------------------------------------------------------------------------------------

add_executable(tplog-tests)
target_sources(tplog-tests PRIVATE ${sources})

target_link_libraries(tplog-tests
    PRIVATE
        tplog::tplog
        gtest_main)

if(NOT PROJECT_IS_TOP_LEVEL)
    win_copy_deps_to_target_dir(tplog-tests tplog::tplog)
endif()

include(GoogleTest)
gtest_discover_tests(tplog-tests)