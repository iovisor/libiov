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
#include <memory>
#include <vector>
#include <arpa/inet.h>
#include <iostream>     // std::cout, std::endl
#include <iomanip>      // std::setfill, std::set
#include "libiov/internal/types.h"
#include "libiov/module.h"
#include "libiov/table.h"
#include "libiov/filesystem.h"
#include "libiov/metadata.h"
#include "libiov/event.h"

using std::future;
using std::promise;
using std::string;
using std::vector;
using std::unique_ptr;

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

int Table::GetTableElements(std::map<std::string, std::string> &item) {
     //Open Metadata
     //Open Key desc optional for now
     //Open Leaf desc optional for now
     //lookup all the keys and return them along with values
     int ret = 0;
     MetaData meta;
     int key_meta;

     key_meta = 0;
     ret = Lookup(table_desc_fd, &key_meta, &meta.item);
     if (ret != 0) {
         return ret;
     }

     std::string key(meta.item.key_size, 'f');
     std::string next_key(meta.item.key_size, '\0');
     std::string leaf(meta.item.leaf_size, '\0');;
     
     for (;;) {
        ret = GetKey(table_fd, 
                     (void *)key.c_str(), 
                     (void *)next_key.c_str());

        if (ret != 0) {
            // Should return a value for this that is not 0
            // since empty table are ok. Set it to 0 for now.
            // We'll have to think about error handling later.
            ret = 0;
            break;
        }

        ret = Lookup(table_fd, 
                     (void *)next_key.c_str(), 
                     (void *)leaf.c_str());
        if (ret != 0) {
            return ret;
        }
        item[next_key] = leaf;
        // We can fill the data
        key = next_key;
        
     }
     return ret;
}

void Table::DumpItem(std::string item) {
     for (std::string::iterator it = item.begin() ; it != item.end(); ++it) {
          int ch = static_cast<unsigned char>(*it);
          std::cout << std::showbase // show the 0x prefix
                    << std::internal // fill between the prefix and the number
                    << std::setfill('0'); // fill with 0s

          std::cout << std::hex << std::setw(2) << ch << " ";
     }
     std::cout << std::endl;
}

int Table::ShowTableElements() {
     std::map<std::string, std::string> items;
     int ret = 0;
     ret = GetTableElements(items);
     if ( ret != 0 )
        return ret;
     std::map<std::string, std::string>::iterator it = items.begin();
     while (it != items.end()) {
        // Accessing KEY from element pointed by it.
        std::string key = it->first;
        DumpItem(it->first);
        // Accessing VALUE from element pointed by it.
        std::string value = it->second;
        DumpItem(it->second);
        std::cout << std::endl;
        // Increment the Iterator to point to next entry
        it++;
     }
     return ret;
}

void Table::SetTableScope(bool scope) {
     global = scope;
}

bool Table::GetTableScope() {
     return global;
}

} //End of namespace iov

