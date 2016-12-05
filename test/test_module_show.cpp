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

#include <cstdio>
#include <memory>
#include <vector>

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>
#include <libiov.h>
#include "libiov/command.h"
#include "libiov/filesystem.h"
#include "libiov/module.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;
using namespace std;

TEST_CASE("test show module attributes", "[module_show]") {
  std::string t_file, m_file, uuid_str, e_file;
  Table table;
  std::string uuidFile, tableFile, metaFile, eventFile;
  std::ifstream File;
  int fd_table = 0;
  uint32_t key;
  int ret;
  struct descr item;
  metaFile = "/var/tmp/meta.txt";
  eventFile = "/var/tmp/module.txt";
  tableFile = "/var/tmp/table.txt";
  uuidFile = "/var/tmp/uuid.txt";
  File.open(uuidFile);

  getline(File, uuid_str);
  File.close();

  string uuid_test;
  string module_name = "bridge";
  vector<Table> tables;
  vector<Event> events;
  bool scope = false;
  FileSystem *fs;

  auto mod = unique_ptr<IOModule>(new IOModule(module_name));
  REQUIRE(mod->Init("libiov/", NET_FORWARD, uuid_str, eventFile, tableFile,
              metaFile, scope) == true);

  uuid_test = mod->NameToUuid(module_name);
  REQUIRE(uuid_test == uuid_str);

  tables = mod->ShowStates(module_name);
  events = mod->ShowEvents(module_name);

  fs = mod->GetFileSystemHandler();
  fs->Delete("libiov", true);
  std::remove(tableFile.c_str());
  std::remove(metaFile.c_str());
  std::remove(eventFile.c_str());
  std::remove(uuidFile.c_str());
}
