# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(test_sanity test_sanity.cpp)
target_link_libraries(test_sanity iov)
add_executable(test_module_multi test_module_multi.cpp)
target_link_libraries(test_module_multi iov)
add_executable(test_module_save test_module_save.cpp)
target_link_libraries(test_module_save iov)

add_test(NAME style-check COMMAND ${CMAKE_SOURCE_DIR}/scripts/style-check.sh)
#set_tests_properties(style-check PROPERTIES PASS_REGULAR_EXPRESSION ".*")

add_test(NAME test_sanity COMMAND ./test_sanity)
add_test(NAME test_module_multi COMMAND sudo ./test_module_multi)
add_test(NAME test_module_save COMMAND sudo ./test_module_save)
