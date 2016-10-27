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

TEST_CASE("test save local event/table fd to filesystem", "[module_pin]") {
  char *uuid_str = NULL;
  int fd;
  FileSystem fs;
  string path = ModulePath;
  string text = "int foo(void *ctx) { return 0; }";
  std::ofstream moduleFile;
  std::ofstream deleteFile;

  moduleFile.open("/var/tmp/module.txt");
  deleteFile.open("/var/tmp/delete.txt");

  auto mod = unique_ptr<IOModule>(new IOModule());
  REQUIRE(mod->Init(std::move(text), IOModule::NET_FORWARD).get() == true);

  uuid_str = new char[100];
  fs.GenerateUuid(uuid_str);

  path.append(uuid_str);
  REQUIRE(mkdir(path.c_str(), (S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH)) == 0);

  deleteFile << path.c_str();

  path.append(ModuleEventPath);
  REQUIRE(mkdir(path.c_str(), (S_IRWXU | S_IXGRP | S_IRGRP | S_IROTH | S_IXOTH)) == 0);

  fd = mod->GetFileDescriptor();

  REQUIRE(fs.Save(path.c_str(), "foo", fd) == 0);

  path.append("foo");

  moduleFile << path.c_str();
  delete[] uuid_str;
  moduleFile.close();
  deleteFile.close();
}
