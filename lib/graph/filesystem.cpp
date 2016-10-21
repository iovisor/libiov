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
#include "libiov/prog.h"
#include "libiov/table.h"
#include "libiov/filesystem.h"
#include "libiov/event.h"

using std::future;
using std::promise;
using std::string;
using namespace iov::internal;


FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}

int FileSystem::Save(std::string pathname, std::string file_name, int fd) {
     int ret = 0;
     const char *file = NULL;
     pathname.append(file_name);
     file = pathname.c_str();
     ret = bpf_obj_pin(fd, file);
     return ret;
}

int FileSystem::Open(std::string pathname) {
     int ret = 0;
     const char *file = NULL;
     file = pathname.c_str();
     ret = bpf_obj_get(file);
     return ret;
}


