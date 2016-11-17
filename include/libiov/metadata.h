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

#pragma once

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>
#include <linux/bpf.h>
#include <cstring>
#include <future>
#include <iostream>
#include <string>

#include "libiov/types.h"

namespace iov {
class IOModule;

struct descr {
  size_t key_size;
  size_t leaf_size;
  uint32_t key_desc_size;
  uint32_t leaf_desc_size;
};

class MetaData {
 public:
  struct descr item;

  MetaData();
  ~MetaData();
  void Update(ebpf::BPFModule *bpf_mod);
};
}  // namespace iov
