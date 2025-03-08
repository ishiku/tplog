# source: https://github.com/pananton/cpp-lib-template/blob/main/CMakeLists.txt

cmake_minimum_required(VERSION 3.28)
project(tplog
    DESCRIPTION "C++ training project. Logging library."
    LANGUAGES CXX)

include(cmake/read_project_version.cmake)
include(cmake/set_if_undefined.cmake)
include(cmake/enable_addr_sanitizer.cmake)
include(cmake/enable_thread_sanitizer.cmake)
include(GNUInstallDirs)

read_project_version()

#----------------------------------------------------------------------------------------------------------------------
# general settings and options
#----------------------------------------------------------------------------------------------------------------------

option(TPLOG_BUILD_TESTS "Build tplog tests" OFF)
option(TPLOG_BUILD_EXAMPLES "Build tplog examples" OFF)
option(TPLOG_BUILD_DOCS "Build tplog documentation" OFF)
option(TPLOG_INSTALL "Generate target for installing tplog" ${PROJECT_IS_TOP_LEVEL})

option(TPLOG_TIDY "run clang-tidy" OFF)
option(TPLOG_SANITIZE_ADDRESS "Enable address sanitizer in tests" OFF)
option(TPLOG_SANITIZE_THREAD "Enable thread sanitizer in tests" OFF)

set_if_undefined(TPLOG_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/tplog" CACHE STRING
    "Install path for tplog package-related CMake files")

if(DEFINED TPLOG_SHARED_LIBS)
    set(BUILD_SHARED_LIBS ${TPLOG_SHARED_LIBS})
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set_if_undefined(CMAKE_CXX_VISIBILITY_PRESET hidden)
set_if_undefined(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if (TPLOG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    message(STATUS "Enabled clang-tidy")
endif ()

if (TPLOG_SANITIZE_ADDRESS AND TPLOG_SANITIZE_THREAD)
    message(FATAL_ERROR "TPLOG_SANITIZE_ADDRESS and TPLOG_SANITIZE_THREAD are mutually exclusive")
endif ()

if (TPLOG_SANITIZE_ADDRESS)
    enable_addr_sanitizer(tplog)
elseif (TPLOG_SANITIZE_THREAD)
    enable_thread_sanitizer(tplog)
endif ()

add_library(tplog)
add_library(tplog::tplog ALIAS tplog)

#----------------------------------------------------------------------------------------------------------------------
# tplog dependencies
#----------------------------------------------------------------------------------------------------------------------

# Search for your dependencies here

#----------------------------------------------------------------------------------------------------------------------
# tplog sources
#----------------------------------------------------------------------------------------------------------------------

include(GenerateExportHeader)
set(export_file_name "export_shared.h")

if(NOT BUILD_SHARED_LIBS)
    set(export_file_name "export_static.h")
endif()

generate_export_header(tplog EXPORT_FILE_NAME include/tplog/${export_file_name})

configure_file ( "${PROJECT_SOURCE_DIR}/cmake/version.h.in"  "${PROJECT_BINARY_DIR}/include/tplog/version.h")

set(sources
    include/tplog/export.h
    include/tplog/tplog.h
    src/tplog.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

#----------------------------------------------------------------------------------------------------------------------
# tplog target
#----------------------------------------------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

target_sources(tplog PRIVATE ${sources})
target_compile_definitions(tplog PUBLIC "$<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:TPLOG_STATIC_DEFINE>")

target_include_directories(tplog
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>")

set_target_properties(tplog PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION})

if(TPLOG_INSTALL AND NOT CMAKE_SKIP_INSTALL_RULES)
    configure_package_config_file(cmake/tplog-config.cmake.in tplog-config.cmake
        INSTALL_DESTINATION "${TPLOG_INSTALL_CMAKEDIR}")

    write_basic_package_version_file(tplog-config-version.cmake
        COMPATIBILITY SameMajorVersion)

    install(TARGETS tplog EXPORT tplog_export
        RUNTIME COMPONENT tplog
        LIBRARY COMPONENT tplog NAMELINK_COMPONENT tplog-dev
        ARCHIVE COMPONENT tplog-dev
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
    install(DIRECTORY include/
        TYPE INCLUDE
        COMPONENT tplog-dev)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/tplog/${export_file_name}"
        COMPONENT tplog-dev
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tplog")

    set(targets_file "tplog-shared-targets.cmake")

    if(NOT BUILD_SHARED_LIBS)
        set(targets_file "tplog-static-targets.cmake")
    endif()

    install(EXPORT tplog_export
        COMPONENT tplog-dev
        FILE "${targets_file}"
        DESTINATION "${TPLOG_INSTALL_CMAKEDIR}"
        NAMESPACE tplog::)

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/tplog-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/tplog-config-version.cmake"
        COMPONENT tplog-dev
        DESTINATION "${TPLOG_INSTALL_CMAKEDIR}")

    if(MSVC)
        set(pdb_file "")
        set(pdb_file_destination "")

        if(BUILD_SHARED_LIBS)
            set(pdb_file "$<TARGET_PDB_FILE:tplog>")
            set(pdb_file_destination "${CMAKE_INSTALL_BINDIR}")
        else()
            # TARGET_PDB_FILE does not work for pdb file generated for static library build, determining it manually
            set(pdb_file "$<TARGET_FILE_DIR:tplog>/$<TARGET_FILE_PREFIX:tplog>$<TARGET_FILE_BASE_NAME:tplog>.pdb")
            set(pdb_file_destination "${CMAKE_INSTALL_LIBDIR}")
        endif()

        install(FILES "${pdb_file}"
            COMPONENT tplog-dev
            CONFIGURATIONS Debug RelWithDebInfo
            DESTINATION "${pdb_file_destination}"
            OPTIONAL)
    endif()
endif()

#----------------------------------------------------------------------------------------------------------------------
# other targets
#----------------------------------------------------------------------------------------------------------------------

if(TPLOG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(TPLOG_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(TPLOG_BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    doxygen_add_docs(docs include)
endif()