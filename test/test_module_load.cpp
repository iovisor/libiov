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
#include "libiov/event.h"
#include "libiov/filesystem.h"
#include "libiov/metadata.h"
#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test load and save module fds' (states and events) to filesystem", "[module_load]") {
  char *uuid_str = NULL;
  int fd, meta_fd;
  FileSystem fs;
  Event *event_list;
  Table *table;
  MetaData *meta;
  ebpf::BPFModule *bpf_mod;
  string pathname;
  string text = "struct interfaces{ u32 rx_pkt; u32 tx_pkt; }; struct packet { u64 rx_pkt; u64 tx_pkt; }; BPF_TABLE(\"hash\", uint32_t, struct packet, num_ports, 1); BPF_TABLE(\"hash\", uint32_t, struct interfaces, ifindex, 1); int one(void *ctx) { return 0; } int two(void *ctx) { return 0; } int three(void *ctx) { return 0; }";
  std::ofstream uuidFile, moduleFile, tableFile, metaFile;

  moduleFile.open("/var/tmp/module.txt");
  tableFile.open("/var/tmp/table.txt");
  metaFile.open("/var/tmp/meta.txt");
  uuidFile.open("/var/tmp/uuid.txt");

  auto mod = unique_ptr<IOModule>(new IOModule());
  REQUIRE(mod->Init(std::move(text)).get() == true);

  bpf_mod = mod->GetBpfModule();
  size_t num_funcs = bpf_mod->num_functions();
  size_t num_tables = bpf_mod->num_tables();

  uuid_str = new char[UUID_LEN];

  std::cout << "NUMBER OF FUNCTIONS: " << num_funcs << std::endl;

  event_list = new Event[num_funcs];
  memset(uuid_str, 0, UUID_LEN);
  fs.GenerateUuid(uuid_str);
  uuidFile << uuid_str;

  for (size_t i = 0; i < num_funcs; i++) {
       pathname.clear();
       REQUIRE(event_list[i].Load(mod.get(), 0, Event::NET_FORWARD) == true);
       REQUIRE(fs.MakePathName(pathname,
               uuid_str,
               EVENT,
               bpf_mod->function_name(i),
               true) == 0);

       fd = event_list[i].GetFileDescriptor();

       REQUIRE(fs.Save(pathname.c_str(), bpf_mod->function_name(i), fd) == 0);

       pathname.append(bpf_mod->function_name(i));

       moduleFile << pathname.c_str() << std::endl;
  }

  table = new Table[num_tables];
  meta = new MetaData[num_tables];

  std::cout << "NUMBER OF TABLES: " << num_tables << std::endl;
  string meta_file_name;
  string table_file_name;
  string key_leaf_path;
  uint32_t key;

  for (size_t i = 0; i < num_tables; i++) {
       pathname.clear();
       std::cout << "TABLE TYPE: " << bpf_mod->table_type(i) << std::endl;
       fd = table[i].Insert((bpf_map_type) (bpf_mod->table_type(i)), bpf_mod->table_key_size(i),
                    bpf_mod->table_leaf_size(i), 1);

       REQUIRE(fs.MakePathName(pathname,
               uuid_str,
               TABLE,
               bpf_mod->table_name(i),
               false) == 0);
       REQUIRE(fs.Save(pathname.c_str(), bpf_mod->table_name(i), fd) == 0);

       table_file_name = pathname;
       table_file_name.append(bpf_mod->table_name(i));
       tableFile << table_file_name.c_str() << std::endl;

       meta[i].Update(bpf_mod);

       meta_fd = table[i].Insert(BPF_MAP_TYPE_HASH, sizeof(uint32_t), sizeof(struct descr), 1);

       meta_file_name = bpf_mod->table_name(i);
       meta_file_name.append("_metadata");

       REQUIRE(fs.Save(pathname.c_str(), meta_file_name.c_str(), fd) == 0);

       key_leaf_path = pathname;
       key_leaf_path.append(meta_file_name);

       metaFile << key_leaf_path.c_str() << std::endl;
       key = 0;
       REQUIRE(table[i].Update(fd, &key, &meta[i].item, BPF_ANY) == 0);

       table[i].UpdateAttributes(bpf_mod, i, false, 0, fd, meta_fd);
  }

  delete [] uuid_str;
  delete [] event_list;
  delete [] table;
  delete [] meta;
  moduleFile.close();
  tableFile.close();
  metaFile.close();
  uuidFile.close();
}
