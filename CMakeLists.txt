#######################################################################
# CMakeLists.txt
# Copyright (c) 2020 -- 2021  Nicolas Ivan Hamblenne
#######################################################################

cmake_minimum_required(VERSION 3.14)
project(libncl LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_DEBUG_POSTFIX _g)

option(DevMode "Development mode" OFF)
option(FETCH_LIBNGC "Force fetching libngc" OFF)

if(DevMode)
    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            string(REGEX REPLACE "(.*)/[^/]*" "\\1" DEV_ROOT "${CMAKE_SOURCE_DIR}")
            set(CMAKE_INSTALL_PREFIX "${DEV_ROOT}" CACHE PATH "Install prefix" FORCE)
        endif()

        option(LIBNCL_TEST "Testing libncl" ON)
        option(LIBNCL_INSTALL "Installing libncl" ON)
    else()
        option(LIBNCL_TEST "Testing libncl" OFF)
        option(LIBNCL_INSTALL "Installing libncl" OFF)
    endif()

    include(cmake/CompilerWarnings.cmake)
else()
    if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
        if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
            set(CMAKE_INSTALL_PREFIX "/opt/nih" CACHE PATH "Install prefix" FORCE)
        endif()

        option(LIBNCL_INSTALL "Installing libncl" ON)
    else()
        option(LIBNCL_INSTALL "Installing libncl" OFF)
    endif()

    option(LIBNCL_TEST "Testing libncl" OFF)

    function(set_project_warnings project_name kind)
    endfunction()
endif()

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    list(INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_INSTALL_PREFIX}")
endif()

include(FetchContent)

if(NOT FETCH_LIBNGC)
    find_package(libngc QUIET)
endif()
if(libngc_FOUND)
    set_target_properties(ngc PROPERTIES MAP_IMPORTED_CONFIG_DEBUG RELEASE)
else()
    set(LIBNGC_INSTALL ON CACHE BOOL "Installing libngc" FORCE)
    message("Fetching libngc")
    FetchContent_Declare(libngc
        GIT_REPOSITORY https://github.com/nhamblenne/libngc
        GIT_TAG main)
    FetchContent_MakeAvailable(libngc)
endif()

add_subdirectory(src)

if(LIBNCL_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()
