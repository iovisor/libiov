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

#pragma once

#include <map>
#include <memory>
#include <string>

#include "libiov/graph.h"

namespace iov {

class IOModule;

class Command {
 public:
  typedef std::map<std::string, std::unique_ptr<IOModule>> ModuleListType;

 private:
  ModuleListType modules_;

 public:
  Command() = default;
  Command(const Command &) = delete;
  Command &operator=(const Command &) = delete;
  ModuleListType &GetModules() { return modules_; }
  bool LookupModule(const std::string &name, IOModule *result) const;
  const ModuleListType &GetModules() const { return modules_; }
  void AddModule(const std::string &name, std::unique_ptr<IOModule> mod);
  std::unique_ptr<IOModule> TakeModule(const std::string &name);
};

}  // namespace iov
