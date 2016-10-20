/*
 * Copyright (c) 2016, PLUMgrid, http://plumgrid.com
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <future>
#include <string>
#include <iostream>

#include "libiov/types.h"

class Table {

 // Name of the table
 std::string table_name;

 // Key/Value pair to store local properties of a module
 std::map<uint8_t, uint8_t> data;

 // Maximun number of propeties for a table
 int max_size;

 // Define if a table is local or global
 bool global;

 // Flags for table accessibility. RW, Hidden etc..
 uint8_t visibility;

 // File descriptor of the table to store key/value
 int table_fd;

 // File desciptor that keeps the metadata for the table,
 // specifically types for key and value
 int table_desc_fd;


 Table();
 ~Table();

 // Api to display key/value pair and size
 std::map<std::string, std::string> ShowTable(std::string table_name);

 // Apis' to Insert/Update/Delete elements of the table
 int Insert(std::string table_name, std::vector<uint8_t> key, std::vector<uint8_t> value);
 int Update(std::string table_name, std::vector<uint8_t> key, std::vector<uint8_t> value);
 int Delete(std::string table_name, std::vector<uint8_t> key, std::vector<uint8_t> value);

 // Api to reset to default value element of the table (counters etc...)
 int Reset(std::string table_name, std::vector<uint8_t> key, std::vector<uint8_t> value);
};

