cmake_minimum_required(VERSION 3.28)
project(easy-peasy LANGUAGES CXX)

include("../../cmake/win_copy_deps_to_target_dir.cmake")
string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" PROJECT_IS_TOP_LEVEL)

if(PROJECT_IS_TOP_LEVEL)
    find_package(tplog REQUIRED)
endif()

set(sources main.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${sources})
target_link_libraries(${PROJECT_NAME} PRIVATE tplog::tplog)

if(NOT PROJECT_IS_TOP_LEVEL)
    win_copy_deps_to_target_dir(${PROJECT_NAME} tplog::tplog)
endif()