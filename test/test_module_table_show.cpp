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
  std::string t_data, m_data;
  std::ifstream tableFile, metaFile;
  int ret = 0;
  Table table;

  tableFile.open("/var/tmp/table.txt");
  metaFile.open("/var/tmp/meta.txt");
  getline(tableFile, t_data);
  getline(metaFile, m_data);
  tableFile.close();
  metaFile.close();

  FileSystem fs(t_data, m_data);

  REQUIRE((ret = table.ShowTableElements(fs)) == 0);
}
