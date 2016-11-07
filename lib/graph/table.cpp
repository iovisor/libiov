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

#include <string.h>
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

Table::Table() {}
Table::~Table() {}

// Insert creates a Map of type bpf_map_type, with key size key_size, a value size of
// leaf_size and the maximum amount of entries of max_entries.

int Table::Insert(bpf_map_type map_type, 
                  int key_size, 
                  int leaf_size, 
                  int max_entries) {

     int fd;
     fd = bpf_create_map(map_type,
                    key_size,
                    leaf_size,
                    max_entries);

     return fd;
}

// Update updates the map in fd with the given value in the given key.
int Table::Update(int fd , void *key, void *value, uint64_t flags) {

     int ret;
     ret = bpf_update_elem(fd, key, value, flags);

     return ret;
}

// Lookup looks up for the map value stored in fd with the given key. The value
// is stored in the value pointer.
int Table::Lookup(int fd, void *key, void *value) {
     int ret;
     ret= bpf_lookup_elem(fd, key, value);
     return ret;
}
// Delete deletes the map element with the given key.
int Table::Delete(int fd, void *key) {
     int ret;
     ret = bpf_delete_elem(fd, key);
     return ret;
}

// GetKey stores, in next_key, the next key after the key of the map in fd.
int Table::GetKey(int fd, void *key, void *next_key) {
     int ret;
     ret =  bpf_get_next_key(fd, key, next_key);
     return ret;
}

} //End of namespace iov

