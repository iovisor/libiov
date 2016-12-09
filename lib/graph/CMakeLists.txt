# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${LIBBCC_INCLUDE_DIRS})

add_library(iovgraph command.cpp graph.cpp module.cpp filesystem.cpp table.cpp event.cpp)
