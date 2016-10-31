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
#include "libiov/table.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test table loading and saving", "[module_table_pin]") {
  char *uuid_str = NULL;
  int fd;
  FileSystem fs;
  Table table;
  string path = ModulePath;
  std::ofstream tableFile;
  string text = "BPF_TABLE(\"hash\", u32, u32, num_ports, 1);";

  tableFile.open("/var/tmp/table.txt");

  fd = table.Insert(BPF_MAP_TYPE_HASH, sizeof(uint32_t), sizeof(uint32_t), 1);

  uuid_str = new char[100];
  fs.GenerateUuid(uuid_str);

  path.append(uuid_str);
  REQUIRE(mkdir(path.c_str(), (S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH)) == 0);

  path.append(StatePath);
  REQUIRE(mkdir(path.c_str(), (S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH)) == 0);

  REQUIRE(fs.Save(path.c_str(), "num_ports", fd) == 0);

  path.append("num_ports");

  tableFile << path.c_str();
  delete[] uuid_str;
  tableFile.close();

}
