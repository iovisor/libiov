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

#include <memory>
#include <vector>

#include <libiov.h>
#include "libiov/command.h"
#include "libiov/module.h"
#include "libiov/filesystem.h"
#include "libiov/metadata.h"
#include "libiov/table.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test lookup local metadata table element", "[module_table_lookup_metadata]") {
  FileSystem fs;
  MetaData meta;
  Table table;
  std::string meta_path;
  std::ifstream metaFile;
  int meta_fd = 0;
  int key_desc_fd, leaf_desc_fd;
  int ret = 0;
  uint32_t key;

  metaFile.open("/var/tmp/meta.txt");
 
  getline(metaFile,meta_path);
  metaFile.close();

  std::cout << "META PATH: " << meta_path << std::endl;
  REQUIRE((meta_fd = fs.Open(meta_path.c_str())) > 0);

  key = 0;
  REQUIRE((ret = table.Lookup(meta_fd, &key, &meta.item)) == 0);

  std::string key_desc_path = meta_path;
  REQUIRE((fs.Replace(key_desc_path, "_metadata", KeyDesc)) == true);

  REQUIRE((key_desc_fd = fs.Open(key_desc_path.c_str())) > 0);

  std::string key_desc(meta.item.key_desc_size, '\0');
  REQUIRE((ret = table.Lookup(key_desc_fd, &key, (void *)key_desc.c_str())) == 0);

  string leaf_desc_path = meta_path;
  REQUIRE((fs.Replace(leaf_desc_path, "_metadata", LeafDesc)) == true);;
  REQUIRE((leaf_desc_fd = fs.Open(leaf_desc_path.c_str())) > 0);

  std::string leaf_desc(meta.item.leaf_desc_size, '\0');
  REQUIRE((ret = table.Lookup(leaf_desc_fd, &key, (void *)leaf_desc.c_str())) == 0);
  
}
