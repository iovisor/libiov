# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(iov iov.cpp)
target_link_libraries(iov iovgraph)

add_subdirectory(graph)
