# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(test_sanity test_sanity.cpp)
target_link_libraries(test_sanity iov)

add_test(NAME test_sanity COMMAND ./test_sanity)
