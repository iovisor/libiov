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

#include "libiov/event.h"
#include <arpa/inet.h>
#include <string.h>
#include <iomanip>   // std::setfill, std::set
#include <iostream>  // std::cout, std::endl
#include <memory>
#include <vector>
#include "libiov/filesystem.h"
#include "libiov/internal/types.h"
#include "libiov/metadata.h"
#include "libiov/table.h"

using std::future;
using std::promise;
using std::string;
using std::vector;
using std::unique_ptr;

using namespace iov::internal;

namespace iov {

Event::Event() {}
Event::~Event() {}

bool Event::Load(IOModule *module, size_t index, ModuleType type) {
  ebpf::BPFModule *bpf_mod = module->GetBpfModule();
  switch (type) {
  case NET_FORWARD:
    prog_.reset(new FileDesc(bpf_prog_load(BPF_PROG_TYPE_SCHED_CLS,
        (const struct bpf_insn *)bpf_mod->function_start(index),
        bpf_mod->function_size(index), bpf_mod->license(),
        bpf_mod->kern_version(), nullptr, 0)));
    if (*prog_ >= 0)
      return true;
  default: {}
  }
  return false;
}

int Event::GetFileDescriptor() {
  FileDesc *fd = prog_.get();
  return *fd;
}

}  // End of namespace iov
