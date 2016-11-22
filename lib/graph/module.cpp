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
#include <memory>

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>

#include "libiov/event.h"
#include "libiov/filesystem.h"
#include "libiov/internal/types.h"
#include "libiov/module.h"
#include "libiov/table.h"

using std::future;
using std::promise;
using std::string;
using namespace iov::internal;
using namespace boost::filesystem;
using namespace std;

namespace iov {

IOModule::IOModule() {}
IOModule::IOModule(string module_name) {name=module_name;}
IOModule::~IOModule() {}

bool IOModule::Init(string &&text, ModuleType type, bool scope) {
  future<bool> res = std::async(std::launch::async,
      [this](string &&text) -> bool {
        mod_ = make_unique<ebpf::BPFModule>(0);
        if (mod_->load_string(text, nullptr, 0) < 0)
          return false;
        return true;
      },
      std::move(text));
  
  bool ret = res.get();
  if (!ret)
    return false;

  return Load(type, scope);
}

bool IOModule::Load(ModuleType type, bool scope) {

  bool ret = true;
  FileSystem fs;
  path pevent;
  path pmeta;
  path ptable;

  num_functions = mod_->num_functions();
  num_tables = mod_->num_tables();

  fs.GenerateUuid(uuid);

  for (size_t i = 0; i < num_functions; i++) {
    if (!fs.MakePathName(pevent, uuid, EVENT,
            mod_->function_name(i), scope)) {
      std::cout << "Create dir for event failed" << std::endl;
      return false;
    }

     pevent+= mod_->function_name(i);

    // In this construnctor we can pass some arg
    std::unique_ptr<Event> ev = make_unique<Event>(mod_->function_name(i), pevent);
    Event *ev_tmp = ev.get();
    event[mod_->function_name(i)] = std::move(ev);

    ret = ev_tmp->Load(this, i, type);
    if (!ret)
      cout << "Error in loading event: " << mod_->function_name(i) << endl;
  }

  for (size_t i = 0; i < num_tables; i++) {
    // In this construnctor we can pass some arg
    if (!fs.MakePathName(ptable, uuid, TABLE,
            mod_->table_name(i), scope)) {
      std::cout << "Create dir for table failed" << std::endl;
      return false;
    }

    ptable += mod_->table_name(i);
    pmeta = ptable;
    pmeta += "_metadata";

    std::unique_ptr<Table> tb = make_unique<Table>(ptable, pmeta, mod_->table_name(i), scope, mod_->table_key_size(i), mod_->table_leaf_size(i));
    Table *tb_tmp = tb.get();
    table[mod_->table_name(i)] = std::move(tb);

    ret = tb_tmp->Load(this, i);
    if (!ret)
      cout << "Error in loading table: " << mod_->table_name(i) << endl;
  }

  return ret;
}

ebpf::BPFModule *IOModule::GetBpfModule() const { return mod_.get(); }

std::vector<Table> IOModule::ShowStates(string module_name) {
  // Lookup the module in the filesystem
  // return the all the table object beloging to the module
  string uuid_str;
  FileSystem fs;
  string pathname;
  bool ret;
  vector<Table> tables;
  vector<string> v;
  path p;

  uuid_str = NameToUuid(module_name);
  ret = fs.MakePathName(p, uuid_str, TABLE, "", false);
  if (!ret) {
    cout << "ERROR DETECTED in making pathname" << endl;
    return tables;
  }
  v = fs.GetFiles(p);
  cout << "TABLES:" << endl;
  for (vector<string>::const_iterator it(v.begin()), it_end(v.end());
       it != it_end; ++it) {
    cout << "  " << *it << endl;
  }

  return tables;
}

vector<Event> IOModule::ShowEvents(string module_name) {
  // Lookup the module in the filesystem
  // return the all the event object beloging to the module
  string uuid_str;
  FileSystem fs;
  string pathname;
  bool ret;
  vector<Event> events;
  vector<string> v;
  path p;

  uuid_str = NameToUuid(module_name);
  ret = fs.MakePathName(p, uuid_str, EVENT, "", false);
  if (!ret) {
    cout << "ERROR DETECTED in making pathname" << endl;
    return events;
  }

  v = fs.GetFiles(p);

  cout << "EVENTS:" << endl;
  for (vector<string>::const_iterator it(v.begin()), it_end(v.end());
       it != it_end; ++it) {
    cout << "  " << *it << endl;
  }

  return events;
}

string IOModule::NameToUuid(string module_name) {
  map<string, string>::const_iterator pos = prog_uuid.find(module_name);
  string uuid_str;
  if (pos == prog_uuid.end()) {
    uuid_str.empty();
  } else {
    uuid_str = pos->second;
  }
  return uuid_str;
}

void IOModule::InsertTable(Table table) { tables.push_back(&table); }

void IOModule::InsertEvent(Event event) { events.push_back(&event); }

}  // namespace iov
