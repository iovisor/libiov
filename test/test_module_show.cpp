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
  std::string uuid_str;
  std::string uuidFile, tableFile, metaFile, eventFile;
  std::ifstream File;
  metaFile = "/var/tmp/meta.txt";
  eventFile = "/var/tmp/module.txt";
  tableFile = "/var/tmp/table.txt";
  uuidFile = "/var/tmp/uuid.txt";
  File.open(uuidFile);

  getline(File, uuid_str);

  string uuid_test;
  string module_name = "bridge";
  vector<Table> tables;
  vector<Event> events;
  bool scope = false;

  std::unique_ptr<FileSystem> fs_ = make_unique<FileSystem>("libiov/");
  FileSystem *fs_tmp = fs_.get();

  auto mod_ = unique_ptr<IOModule>(
      new IOModule(module_name, fs_tmp, eventFile, tableFile, metaFile));
  IOModule *mod_tmp = mod_.get();
  fs_tmp->UpdateIOModule(module_name, std::move(mod_));

  REQUIRE(mod_tmp->Init(NET_FORWARD, uuid_str, scope) == true);

  uuid_test = mod_tmp->NameToUuid(module_name);
  REQUIRE(uuid_test == uuid_str);

  tables = mod_tmp->ShowStates(module_name);
  events = mod_tmp->ShowEvents(module_name);

  fs_tmp->Delete("libiov", true);
  std::remove(tableFile.c_str());
  std::remove(metaFile.c_str());
  std::remove(eventFile.c_str());
  std::remove(uuidFile.c_str());
}
