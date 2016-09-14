/*
 * Copyright (c) 2016 PLUMgrid, Inc.
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

#include "libiov/command.h"
#include "libiov/graph.h"
#include "libiov/internal/types.h"

using iov::internal::make_unique;
using std::move;
using std::string;
using std::unique_ptr;

namespace iov {

void Command::AddModule(const string &name, unique_ptr<IOModule> mod) {
  modules_[name] = move(mod);
}

unique_ptr<IOModule> Command::TakeModule(const string &name) {
  auto it = modules_.find(name);
  if (it == modules_.end())
    return make_unique<IOModule>();

  auto p = move(it->second);
  modules_.erase(it);
  return p;
}

bool Command::LookupModule(const string &name, IOModule *result) const {
  auto it = modules_.find(name);
  if (it == modules_.end())
    return false;
  result = &*it->second;
  return true;
}

}  // namespace iov
