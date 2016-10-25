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
#include <string.h>

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

namespace iov {

FileSystem::FileSystem() {}
FileSystem::~FileSystem() {}

/* Save
 * Api to save the file descriptor of a bpf program, bpf table.
 * pathname will have the location in the filesytem. Check filesytem.h
 * for more info.
 * file_name is the file where the fd will be save
 */

int FileSystem::Save(std::string pathname, std::string file_name, int fd) {
     int ret = 0;
     const char *file = NULL;
     pathname.append(file_name);
     file = pathname.c_str();
     ret = bpf_obj_pin(fd, file);
     return ret;
}

/* Open
 * api to retrive the file descriptor of a bpf program, bpf table.
 * pathname will have the location in the filesytem. Check filesytem.h
 * for more info.
 */

int FileSystem::Open(std::string pathname) {
     int ret = 0;
     const char *file = NULL;
     file = pathname.c_str();
     ret = bpf_obj_get(file);
     return ret;
}

/* Show
 * api to show the filesystem. Conceptually same as:
 * Ex: ls /sys/fs/bpf/libiov
 * tables/ modules/
 * pathname will have the starting location in the filesytem. Check filesytem.h
 * for more info.
 */

std::vector<std::string> FileSystem::Show(std::string pathname) {
     std::vector<std::string> ret;
     return ret;
}

/* Delete
 * api to delete a specific file <file_name>.
 * pathname will have the location in the filesytem. Check filesytem.h
 * for more info.
 */

bool FileSystem::Delete(std::string pathname, std::string file_name) {
     bool ret = false;
     return ret;
}

void FileSystem::GenerateUuid(char *uuid_str) {
     uuid_t id1;
     uuid_generate( (unsigned char *)&id1 );
     uuid_unparse(id1, uuid_str);
}
} // namespace iov
