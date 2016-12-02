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

#include <linux/bpf.h>
#include <future>
#include <iostream>
#include <string>

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>

#include "libiov/module.h"
#include "libiov/types.h"

namespace iov {
class IOModule;

struct descr {
  size_t key_size;
  size_t leaf_size;
};

class Table {
 private:
  boost::filesystem::path path_table_fd;
  boost::filesystem::path path_meta_fd;
  int fd_table;
  int fd_meta;
  FileDescPtr tableprog_;
  FileDescPtr metaprog_;
  // Name of the table
  std::string table_name;
  // Maximun number of propeties for a table
  int max_size;
  // Define if a table is local or global
  bool global;
  size_t key_size;
  size_t leaf_size;
  // Key/Value pair
  std::map<std::string, std::string> data;
  // Flags for table accessibility. RW, Hidden etc..
  uint8_t visibility;

 public:
  Table();
  Table(const std::string name, bool scope, size_t key_size, size_t leaf_size);
  ~Table();

  bool InitTable(IOModule *module, size_t index);

  // Api to set the table scope (local or global)
  void SetTableScope(bool scope);

  // Api to return the table scope (local or global)
  bool GetTableScope();

  // Api to gather key/value pair and size
  int GetTableElements(std::map<std::string, std::string> &item);

  // Api to display key/value pair
  int ShowTableElements(FileSystem fs);

  void DumpItem(std::string item);

  int Insert(
      bpf_map_type map_type, int key_size, int leaf_size, int max_entries);

  // Api to Update an element of the table
  int Update(FileSystem *fs, void *key, void *value, uint64_t flags);

  // Api to Delete an element of the table
  int Delete(int fd, void *key);

  // Api to  Lookup an element of the table (counters etc...)
  int Lookup(int fd, void *key, void *value);

  // Api to get the next key after the key of the map in fd
  int GetKey(int fd, void *key, void *next_key);
};
}  // namespace iov
