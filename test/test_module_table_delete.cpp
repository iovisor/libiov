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
#include "libiov/filesystem.h"
#include "libiov/table.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test delete local table element", "[module_delete_get]") {
  std::string text;
  FileSystem fs;
  std::ifstream tableFile;
  int fd = 0;
  uint32_t key = 0;
  uint32_t value;
  Table table;

  tableFile.open("/var/tmp/table.txt");
 
  getline(tableFile,text);
  tableFile.close();

  REQUIRE((fd = fs.Open(text.c_str())) > 0);
  key = 0;
  REQUIRE(table.Delete(fd, &key) == 0);
  REQUIRE((table.Lookup(fd, &key, &value)) < 0);

}
