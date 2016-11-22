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
#include "libiov/module.h"
#include "libiov/table.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test update local table element", "[module_update_get]") {
  std::string f_table;
  FileSystem fs;
  Table table;
  std::ifstream tableFile;
  int fd_table = 0;
  uint32_t key;
  int ret;
  struct descr item;

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

  getline(tableFile, f_table);
  tableFile.close();

  REQUIRE((fd_table = fs.Open(f_table.c_str())) > 0);

  host.mac = 123;
  host.ifindex = 456;
  host.pad = 0;
  packet.rx_pkt = 25;
  packet.tx_pkt = 30;
  REQUIRE(table.Update(fd_table, &host, &packet, BPF_ANY) == 0);
}
