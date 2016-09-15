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

#include <future>
#include <string>

namespace ebpf {
class BPFModule;
}

namespace iov {

namespace internal {
struct FileDesc;
}

class IOModule {
 public:
  enum ModuleType {
    NetModule,
  };

 private:
  std::unique_ptr<internal::FileDesc> prog_;
  std::unique_ptr<ebpf::BPFModule> mod_;

 public:
  IOModule();
  ~IOModule();
  std::future<bool> Init(std::string &&text);
  std::future<bool> Load(ModuleType type);
};

}  // namespace iov
