/*
 * Copyright (c) 2016, PLUMgrid, http://plumgrid.com
 *
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

#include <future>
#include <iostream>
#include <string>

#include "libiov/filesystem.h"
#include "libiov/module.h"
#include "libiov/types.h"

namespace ebpf {
class BPFModule;
}

namespace iov {
class IOModule;
class FileSystem;

class Event {
 private:
  FileDescPtr prog_;
  std::string event_name;
  int event_type;
  int direction;
  std::string fd_path;

 public:
  Event();
  Event(std::string name);
  ~Event();

  bool InitEvent(IOModule *module, ModuleType type, std::string file);
  bool InitEvent(IOModule *module, size_t index, ModuleType type, bool scope);
  // Api to load the event in kernel

  // Api to return the file descriptor
  int GetFileDescriptor();
  std::string GetFdPath();
};

}  // namespace iov
