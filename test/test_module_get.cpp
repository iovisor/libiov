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

#include <iostream>
#include <fstream>
#include <libiov.h>
#include "libiov/command.h"
#include "libiov/module.h"
#include <uuid/uuid.h>
#include "libiov/filesystem.h"
#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test get local event/table fd to filesystem", "[module_get]") {
  const char *file_path;
  std::string text;
  FileSystem fs;
  string del_module = "rm -r -f ";
  std::ifstream moduleFile;
  std::ifstream deleteFile;

  moduleFile.open("/var/tmp/module.txt");
  deleteFile.open("/var/tmp/delete.txt");
 
  getline(moduleFile,text);
  moduleFile.close();

  file_path = text.c_str();

  REQUIRE(fs.Open(file_path) > 0);

  getline(deleteFile,text);
  deleteFile.close();

  del_module.append(text);
  file_path = del_module.c_str();
  REQUIRE(system(file_path) == 0);
 
}
