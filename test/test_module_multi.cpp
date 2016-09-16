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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using namespace iov;

TEST_CASE("test multiple module loading", "[module_multi]") {
  Command cmd;
  for (int i = 0; i < 5; ++i) {
    string text = "int foo(void *ctx) { return 0; }";
    auto mod = unique_ptr<IOModule>(new IOModule());
    REQUIRE(mod->Init(std::move(text), IOModule::NET_FORWARD).get() == true);
    cmd.AddModule("test" + std::to_string(i), std::move(mod));
  }
  for (const auto &mod : cmd.GetModules())
    ;
}
