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
#include "libiov/filesystem.h"
#include "libiov/metadata.h"
#include "libiov/module.h"
#include "libiov/table.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test update local table element", "[module_update_get]") {
  std::string f_table, f_meta;
  FileSystem fs;
  MetaData meta;
  Table table;
  std::ifstream tableFile, metaFile;
  int fd_table, fd_meta, fd_key, fd_leaf = 0;
  uint32_t key;
  int ret;

  struct host_ {
    uint64_t mac;
    int ifindex;
    int pad;
  } host;
  struct packet_ {
    uint64_t rx_pkt;
    uint64_t tx_pkt;
  } packet;

  tableFile.open("/var/tmp/table.txt");
  metaFile.open("/var/tmp/meta.txt");

  getline(tableFile, f_table);
  getline(metaFile, f_meta);
  tableFile.close();
  metaFile.close();

  REQUIRE((fd_table = fs.Open(f_table.c_str())) > 0);
  REQUIRE((fd_meta = fs.Open(f_meta.c_str())) > 0);

  key = 0;
  REQUIRE((ret = table.Lookup(fd_meta, &key, &meta.item)) == 0);

  std::string key_text = f_meta;
  REQUIRE((fs.Replace(key_text, "_metadata", KeyDesc)) == true);
  REQUIRE((fd_key = fs.Open(key_text.c_str())) > 0);

  std::string key_desc(meta.item.key_desc_size, '\0');
  REQUIRE((ret = table.Lookup(fd_key, &key, (void *)key_desc.c_str())) == 0);

  string leaf_text = f_meta;
  REQUIRE((fs.Replace(leaf_text, "_metadata", LeafDesc)) == true);
  REQUIRE((fd_leaf = fs.Open(leaf_text.c_str())) > 0);

  std::string leaf_desc(meta.item.leaf_desc_size, '\0');
  REQUIRE((ret = table.Lookup(fd_leaf, &key, (void *)leaf_desc.c_str())) == 0);

  host.mac = 123;
  host.ifindex = 456;
  host.pad = 0;
  packet.rx_pkt = 25;
  packet.tx_pkt = 30;
  REQUIRE(table.Update(fd_table, &host, &packet, BPF_ANY) == 0);
}
