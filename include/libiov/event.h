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

#include "libiov/module.h"
#include "libiov/types.h"

namespace ebpf {
class BPFModule;
}

namespace iov {
class IOModule;

class Event {
 public:
  FileDescPtr prog_;

  // Name of the event
  std::string event_name;

  // cls_bpf, xdp etc...
  int event_type;

  // handler for ingress or egress
  int direction;
  Event();
  Event(std::string name, boost::filesystem::path fd);
  ~Event();

  boost::filesystem::path fd_path;
  // Api to load the event in kernel
  bool Load(IOModule *module, size_t index, ModuleType type);

  // Api to return the file descriptor
  int GetFileDescriptor();
};

}  // namespace iov
