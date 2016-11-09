/*
 * Copyright (c) 2016, PLUMgrid, http://plumgrid.com
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
#include <linux/bpf.h>

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>

#include "libiov/types.h"

namespace iov {
class IOModule;

class Table {

public:

 // Name of the table
 std::string table_name;

 // Key/Value pair to store local properties of a module
 std::map<std::string, std::string> data;

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

 // Api to gather key/value pair and size
 int GetTableElem(std::map<std::string, std::string> &item);

 // Api to display key/value pair
 void ShowTableElem(std::map<std::string, std::string> item);

 void DumpItem(std::string item);

 // Api to Insert elements of the table
 int Insert(bpf_map_type map_type, int key_size, int leaf_size, int max_entries); 

 // Api to Update an element of the table
 int Update(int fd, void *key, void *value, uint64_t flags);

 // Api to Delete an element of the table
 int Delete(int fd, void *key);

 // Api to  Lookup an element of the table (counters etc...)
 int Lookup(int fd, void *key, void *value);

 // Api to get the next key after the key of the map in fd
 int GetKey(int fd, void *key, void *next_key);
};
} //namespace iov
