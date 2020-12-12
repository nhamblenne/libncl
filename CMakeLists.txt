#######################################################################
# CMakeLists.txt
# Copyright (c) 2020 Nicolas Ivan Hamblenne
#######################################################################

cmake_minimum_required(VERSION 3.5)
project(libncl C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_DEBUG_POSTFIX _g)

option(DevMode "Development mode" OFF)
option(TEST_LIBNCL "Testing libncl" OFF)

string(REGEX REPLACE "(.*)/[^/]*" "\\1" DEV_ROOT "${PROJECT_SOURCE_DIR}")
string(REGEX REPLACE ".*/([^/]*)" "\\1" PROJECT_BASE_NAME "${PROJECT_SOURCE_DIR}")
if(DevMode)
    set(CMAKE_INSTALL_PREFIX "${DEV_ROOT}")
    include(cmake/CompilerWarnings.cmake)
else()
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "/opt/nih" CACHE PATH "Install prefix" FORCE)
    endif()
    function(set_project_warnings project_name kind)
    endfunction()
endif()
list(INSERT CMAKE_PREFIX_PATH 0 "${CMAKE_INSTALL_PREFIX}")

find_package(libngc)
set_target_properties(ngc PROPERTIES MAP_IMPORTED_CONFIG_DEBUG RELEASE)

add_subdirectory(src)
if(TEST_LIBNCL OR CMAKE_SOURCE_DIR STREQUAL libncl_SOURCE_DIR)
    enable_testing()
    add_subdirectory(tests)
endif()
