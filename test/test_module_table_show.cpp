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

#include <linux/bpf.h>
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

TEST_CASE("test show table", "[module_table_show]") {
  char *uuid_str = NULL;
  int fd, ret;
  uint32_t key;
  FileSystem fs;
  Table table;
  IOModule module;
  MetaData meta;
  ebpf::BPFModule *bpf_mod;
  string path = ModulePath;
  string text = "struct packet { u64 rx_pkt; u64 tx_pkt; }; BPF_TABLE(\"hash\", uint32_t, struct packet, interfaces, 20);";

  struct packet_ {
    uint64_t rx_pkt;
    uint64_t tx_pkt;
  } packet;

  module.Init(std::move(text), IOModule::NET_POLICY);

  bpf_mod = module.GetBpfModule();
   
  fd = table.Insert(BPF_MAP_TYPE_HASH, bpf_mod->table_key_size(0), 
                    bpf_mod->table_leaf_size(0), 20);

  table.table_fd = fd;

  uuid_str = new char[100];
  fs.GenerateUuid(uuid_str);

  path.append(uuid_str);
  fs.CreateDir(path);

  path.append(StatePath);
  fs.CreateDir(path);

  REQUIRE(fs.Save(path.c_str(), bpf_mod->table_name(0), fd) == 0);

  string metadata = path; 
  metadata.append(MetadataPath); 
  fs.CreateDir(metadata);

  path.append(bpf_mod->table_name(0));

  string table_key = bpf_mod->table_key_desc(0);
  string table_leaf = bpf_mod->table_leaf_desc(0);

  meta.Update(bpf_mod);

  fd = table.Insert(BPF_MAP_TYPE_HASH, sizeof(uint32_t), sizeof(struct descr), 1);
  table.table_desc_fd = fd;

  string file_name = bpf_mod->table_name(0);
  file_name.append("_metadata"); 
  REQUIRE(fs.Save(metadata.c_str(), file_name.c_str(), fd) == 0);

  key = 0;
  REQUIRE(table.Update(fd, &key, &meta.item, BPF_ANY) == 0);

  key = 0;
  packet.rx_pkt = 20;
  packet.tx_pkt = 21;
  REQUIRE(table.Update(table.table_fd, &key, &packet, BPF_ANY) == 0);

  key = 1;
  packet.rx_pkt = 26;
  packet.tx_pkt = 27;
  REQUIRE(table.Update(table.table_fd, &key, &packet, BPF_ANY) == 0);
  key = 2;
  packet.rx_pkt = 45;
  packet.tx_pkt = 58;
  REQUIRE(table.Update(table.table_fd, &key, &packet, BPF_ANY) == 0);

  REQUIRE((ret = table.ShowTableElements()) == 0);

  fs.Delete("modules", true); 
  delete[] uuid_str;
}
