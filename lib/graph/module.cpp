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

#include <linux/bpf.h>

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>

#include "libiov/internal/types.h"
#include "libiov/module.h"

using std::future;
using std::promise;
using std::string;
using namespace iov::internal;

namespace iov {

IOModule::IOModule() {}
IOModule::~IOModule() {}

future<bool> IOModule::Init(string &&text, ModuleType type) {
  future<bool> res = std::async(std::launch::async,
      [this](string &&text) -> bool {
        mod_ = make_unique<ebpf::BPFModule>(0);
        if (mod_->load_string(text, nullptr, 0) < 0)
          return false;
        return true;
      },
      std::move(text));
  if (!res.get())
    return res;
  return Load(type);
}

future<bool> IOModule::Load(ModuleType type) {
  return std::async(std::launch::async, [this, type]() -> bool {
    switch (type) {
    case NET_FORWARD:
      prog_.reset(new FileDesc(bpf_prog_load(BPF_PROG_TYPE_SCHED_CLS,
          (const struct bpf_insn *)mod_->function_start(0),
          mod_->function_size(0), mod_->license(), mod_->kern_version(),
          nullptr, 0)));
      if (*prog_ >= 0)
        return true;
    default: {}
    }
    return false;
  });
}

}  // namespace iov
