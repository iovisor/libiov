# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(test_trivial test_trivial.c)
target_link_libraries(test_trivial iov)

add_test(NAME test_trivial COMMAND ./test_trivial)
