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
#include <linux/bpf.h>
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
using namespace boost::filesystem;

TEST_CASE("test show table", "[module_table_show]") {

  std::string f_table, m_data;
  FileSystem fs;
  std::ifstream tableFile, metaFile;
  int table_fd = 0;
  int meta_fd = 0;
  int ret = 0;
  uint32_t key;
  Table table;
  struct descr item;

  tableFile.open("/var/tmp/table.txt");
  metaFile.open("/var/tmp/meta.txt");
  getline(tableFile, f_table);
  getline(metaFile, m_data);
  tableFile.close();
  metaFile.close();

  REQUIRE((table_fd = fs.Open(f_table.c_str())) > 0);
  REQUIRE((meta_fd = fs.Open(m_data.c_str())) > 0);

  table.fd_meta = meta_fd;
  table.fd_table = table_fd;

  REQUIRE((ret = table.ShowTableElements()) == 0);

}
