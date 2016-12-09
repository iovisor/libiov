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

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>
#include <libiov.h>
#include "libiov/command.h"
#include "libiov/event.h"
#include "libiov/filesystem.h"
#include "libiov/module.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test load and save module fds to filesystem", "[module_load]") {
  ebpf::BPFModule *bpf_mod;
  bool scope = false;
  string fd_path;
  string text =
      "struct packet { u64 "
      "rx_pkt; u64 tx_pkt; }; BPF_TABLE(\"hash\", uint32_t, struct packet, "
      "num_ports, 1); "
      "int one(void *ctx) { return 0; } int two(void *ctx) { "
      "return 0; } int three(void *ctx) { return 0; }";

  std::ofstream uuidFile, moduleFile, tableFile, metaFile;

  moduleFile.open("/var/tmp/module.txt", std::ios::app);
  tableFile.open("/var/tmp/table.txt", std::ios::app);
  metaFile.open("/var/tmp/meta.txt", std::ios::app);
  uuidFile.open("/var/tmp/uuid.txt", std::ios::app);

  std::unique_ptr<FileSystem> fs_ = make_unique<FileSystem>("libiov/");
  FileSystem *fs_tmp = fs_.get();

  auto mod_ = unique_ptr<IOModule>(new IOModule("bridge", fs_tmp));

  fs_tmp->UpdateIOModule("bridge", std::move(mod_));

  IOModule *mod_tmp = fs_tmp->GetIOModule("bridge");
  REQUIRE(mod_tmp->Init(std::move(text), NET_FORWARD, scope) == true);
  bpf_mod = mod_tmp->GetBpfModule();
  size_t num_funcs = bpf_mod->num_functions();
  size_t num_tables = bpf_mod->num_tables();

  string uuid = mod_tmp->GetUuid();
  uuidFile << uuid;

  std::map<const std::string, std::unique_ptr<Event>>::iterator event;
  for (event = mod_tmp->GetFirstEvent(); event != mod_tmp->GetLastEvent();
       ++event) {
    fd_path = event->second->GetFdPath();
    moduleFile << fd_path << std::endl;
  }

  std::map<const std::string, std::unique_ptr<Table>>::iterator table;
  for (table = mod_tmp->GetFirstTable(); table != mod_tmp->GetLastTable();
       ++table) {
    fd_path = table->second->GetTableFdPath();
    tableFile << fd_path << std::endl;
    fd_path = table->second->GetMetaFdPath();
    metaFile << fd_path << std::endl;
  }
}
