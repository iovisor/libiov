# Copyright (c) PLUMgrid, Inc.
# Licensed under the Apache License, Version 2.0 (the "License")
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(test_sanity test_sanity.cpp)
target_link_libraries(test_sanity iov)
add_executable(test_module_load test_module_load.cpp)
target_link_libraries(test_module_load iov)
add_executable(test_module_table_update test_module_table_update.cpp)
target_link_libraries(test_module_table_update iov)
add_executable(test_module_table_lookup test_module_table_lookup.cpp)
target_link_libraries(test_module_table_lookup iov)
add_executable(test_module_table_show test_module_table_show.cpp)
target_link_libraries(test_module_table_show iov)
add_executable(test_module_show test_module_show.cpp)
target_link_libraries(test_module_show iov)



add_test(NAME style-check COMMAND ${CMAKE_SOURCE_DIR}/scripts/style-check.sh)
#set_tests_properties(style-check PROPERTIES PASS_REGULAR_EXPRESSION ".*")

add_test(NAME test_sanity COMMAND ./test_sanity)
add_test(NAME test_module_load COMMAND sudo ./test_module_load)
add_test(NAME test_module_table_update COMMAND sudo ./test_module_table_update)
add_test(NAME test_module_table_lookup COMMAND sudo ./test_module_table_lookup)
add_test(NAME test_module_table_show COMMAND sudo ./test_module_table_show)
add_test(NAME test_module_show COMMAND sudo ./test_module_show)
