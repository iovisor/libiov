# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")
cmake_minimum_required(VERSION 2.8.7)

project(libiov)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

enable_testing()

include(cmake/GetGitRevisionDescription.cmake)
include(cmake/version.cmake)
find_package(PkgConfig)
find_library(LIBBCC NAMES bcc)
pkg_search_module(LIBBCC REQUIRED libbcc)
message(STATUS "Found libbcc ${LIBBCC_LIBRARIES}")

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(test)
