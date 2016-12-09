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

#include <unistd.h>
#include <memory>

enum ModuleType {
  NET_FORWARD,
  NET_POLICY,
};

enum obj_type_t { MODULE = 1, EVENT, TABLE, META };

namespace iov {

namespace internal {

// A wrapper class to automatically clean up file descriptors.
struct FileDesc {
  FileDesc(int fd) : fd(fd) {}
  FileDesc(std::nullptr_t) : fd(-1) {}
  operator int() { return fd; }
  bool operator==(const FileDesc &other) const { return fd == other.fd; }
  int fd;
};

// A helper struct to implement RAII cleanup for FileDesc
struct FileDescDeleter {
  void operator()(FileDesc *fd) {
    if (fd)
      ::close(*fd);
  }
};

}  // namespace internal

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args &&... params) {
  return std::unique_ptr<T>(new T(std::forward<Args>(params)...));
}

typedef std::unique_ptr<internal::FileDesc, internal::FileDescDeleter> FileDescPtr;

}  // namespace iov
