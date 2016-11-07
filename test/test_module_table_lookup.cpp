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

TEST_CASE("test lookup local table element", "[module_table_lookup]") {
  std::string f_table, m_data;
  FileSystem fs;
  std::ifstream tableFile, metaFile;
  int table_fd = 0;
  int meta_fd = 0;
  int key_fd = 0;
  int leaf_fd = 0;
  int ret = 0;
  uint32_t key;
  uint32_t value;
  Table table;
  MetaData meta;
  std::string key_desc_path;
  std::string leaf_desc_path;

  tableFile.open("/var/tmp/table.txt");
  metaFile.open("/var/tmp/meta.txt"); 
  getline(tableFile,f_table);
  getline(metaFile,m_data);
  tableFile.close();
  metaFile.close();

  REQUIRE((table_fd = fs.Open(f_table.c_str())) > 0);
  REQUIRE((meta_fd = fs.Open(m_data.c_str())) > 0);

  key = 0;
  REQUIRE((ret = table.Lookup(meta_fd, &key, &meta.item)) == 0);

  std::cout << "KEY DESC: " << meta.item.key_desc << std::endl;
  std::cout << "LEAF DESC: " << meta.item.leaf_desc << std::endl;

  std::string key_test(meta.item.key_size, '\0');
  std::string next_key_test(meta.item.key_size, '\0');

  struct packet_ {
    uint64_t rx_pkt;
    uint64_t tx_pkt;
  } packet;

  REQUIRE((ret = table.GetKey(table_fd, (void *)key_test.c_str(), (void *)next_key_test.c_str())) == 0);
  REQUIRE((ret = table.Lookup(table_fd, (void *)next_key_test.c_str(), &packet)) == 0);
  std::cout << "PACKET RX: " << packet.rx_pkt << std::endl;
  std::cout << "PACKET TX: " << packet.tx_pkt << std::endl;
}
