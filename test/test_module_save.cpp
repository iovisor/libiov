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

TEST_CASE("test save local module/table to filesystem", "[module_save]") {
  char *uuid_str = NULL;
  const char *file_path;
  int fd;
  FileSystem fs;
  string path = ModulePath;
  string text = "BPF_TABLE(\"hash\", u32, u32, num_ports, 1); int foo(void *ctx) { return 0; } int boo(void *ctx) { return 0; }";
  string del_module = "rm -r -f ";

  auto mod = unique_ptr<IOModule>(new IOModule());
  REQUIRE(mod->Init(std::move(text), IOModule::NET_FORWARD).get() == true);

  uuid_str = new char[100];
  fs.GenerateUuid(uuid_str);

  path.append(uuid_str);
  file_path = path.c_str();
  REQUIRE(mkdir(file_path, S_IRWXU) == 0);

  path.append(ModuleEventPath);
  file_path = path.c_str();
  REQUIRE(mkdir(file_path, S_IRWXU) == 0);

  fd = mod->GetFileDescriptor();

  REQUIRE(fs.Save(file_path, "foo", fd) == 0);

  del_module.append(ModulePath);
  del_module.append(uuid_str);
  file_path = del_module.c_str();
  REQUIRE(system(file_path) == 0);
 
  delete[] uuid_str;
}
