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

#include <arpa/inet.h>
#include <libiov.h>
#include <memory>
#include <vector>
#include "libiov/command.h"
#include "libiov/filesystem.h"
#include "libiov/module.h"
#include "libiov/table.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test lookup local table element", "[module_table_lookup]") {
  std::string f_table, m_data;
  std::string tableFile, metaFile, eventFile, uuid_str, uuid_test;
  std::ifstream uuidFile;
  bool scope = false;
  std::string module_name = "brigde";
  int ret = 0;
  uint32_t key;
  struct descr item;
  Table *tb;

  tableFile = "/var/tmp/table.txt";
  metaFile = "/var/tmp/meta.txt";
  eventFile = "/var/tmp/module.txt";

  uuidFile.open("/var/tmp/uuid.txt");
  getline(uuidFile, uuid_str);

  std::unique_ptr<FileSystem> fs_ = make_unique<FileSystem>("libiov/");
  FileSystem *fs_tmp = fs_.get();

  auto mod_ = unique_ptr<IOModule>(
      new IOModule(module_name, fs_tmp, eventFile, tableFile, metaFile));
  IOModule *mod_tmp = mod_.get();
  fs_tmp->UpdateIOModule(module_name, std::move(mod_));

  REQUIRE(mod_tmp->Init(NET_FORWARD, uuid_str, scope) == true);
  uuid_test = mod_tmp->NameToUuid(module_name);
  REQUIRE(uuid_test == uuid_str);

  tb = mod_tmp->GetTable("num_ports");

  key = 0;
  REQUIRE(tb->Lookup(META, &key, &item) == 0);

  std::string key_size(item.key_size, 'f');
  std::string next_key_size(item.key_size, '\0');
  std::string packet(item.leaf_size, '\0');
  struct packet_ {
    uint64_t rx_pkt;
    uint64_t tx_pkt;
  } test;

  REQUIRE((ret = tb->GetKey(TABLE, (void *)key_size.c_str(),
               (void *)next_key_size.c_str())) == 0);
  REQUIRE((ret = tb->Lookup(TABLE, (void *)next_key_size.c_str(),
               (void *)packet.c_str())) == 0);

  memcpy(&test, packet.data(), item.leaf_size);
  REQUIRE(test.rx_pkt == 25);
  REQUIRE(test.tx_pkt == 30);
}
