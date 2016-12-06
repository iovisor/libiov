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
using namespace std;

namespace iov {

IOModule::IOModule() {}
IOModule::IOModule(string module_name) { name = module_name; }
IOModule::~IOModule() {}

void IOModule::GenerateUuid(string &uuid) {
  uuid_t id1;
  char *uuid_str = new char[UUID_LEN];
  uuid_generate((unsigned char *)&id1);
  uuid_unparse(id1, uuid_str);
  uuid = uuid_str;
  prog_uuid[name] = uuid;
  delete[] uuid_str;
}

bool IOModule::Init(string fs_prefix, ModuleType type, string uuid_str,
    string event_fd_path, string table_fd_path, string meta_fd_path,
    bool scope) {
  uuid = uuid_str;
  prog_uuid[name] = uuid_str;
  std::unique_ptr<FileSystem> fs = make_unique<FileSystem>(
      fs_prefix, event_fd_path, table_fd_path, meta_fd_path);
  fs_ = std::move(fs);
  return Load(type);
}

bool IOModule::Init(
    string fs_prefix, string &&text, ModuleType type, bool scope) {
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

  return Load(fs_prefix, type, scope);
}

bool IOModule::Load(ModuleType type) {
  bool ret = true;
  std::ifstream t_file, m_file, e_file;
  string e_line, t_line, m_line;
  string file_name;
  bool scope = false;

  t_file.open(fs_->GetTableFile());
  m_file.open(fs_->GetMetaFile());
  e_file.open(fs_->GetEventFile());

  while (getline(e_file, e_line)) {
    file_name = e_line.substr(e_line.find_last_of("/") + 1);
    std::unique_ptr<Event> ev = make_unique<Event>(file_name);
    Event *ev_tmp = ev.get();
    event[file_name] = std::move(ev);
    ret = ev_tmp->InitEvent(this, type, e_line);
  }
  e_file.close();

  while (getline(t_file, t_line)) {
    getline(m_file, m_line);
    file_name = t_line.substr(t_line.find_last_of("/") + 1);
    std::unique_ptr<Table> tb = make_unique<Table>(file_name, scope);
    Table *tb_tmp = tb.get();
    table[file_name] = std::move(tb);
    file_name = m_line.substr(m_line.find_last_of("/") + 1);
    ret = tb_tmp->InitTable(this, t_line, m_line);
  }
  t_file.close();
  m_file.close();
  return ret;
}

bool IOModule::Load(string fs_prefix, ModuleType type, bool scope) {
  bool ret = true;

  // In this construnctor we can pass some arg. We need to
  // pass the root of the filesystem like libiov/ or something else
  std::unique_ptr<FileSystem> fs = make_unique<FileSystem>(fs_prefix);
  // FileSystem *fs_tmp = fs.get();
  fs_ = std::move(fs);

  num_functions = mod_->num_functions();
  num_tables = mod_->num_tables();

  GenerateUuid(uuid);

  for (size_t i = 0; i < num_functions; i++) {
    // In this construnctor we can pass some arg
    std::unique_ptr<Event> ev = make_unique<Event>(mod_->function_name(i));
    Event *ev_tmp = ev.get();
    event[mod_->function_name(i)] = std::move(ev);
    ret = ev_tmp->InitEvent(this, i, type, scope);
    if (!ret)
      cout << "Error in loading event: " << mod_->function_name(i) << endl;
  }

  for (size_t i = 0; i < num_tables; i++) {
    std::unique_ptr<Table> tb = make_unique<Table>(mod_->table_name(i), scope,
        mod_->table_key_size(i), mod_->table_leaf_size(i));
    Table *tb_tmp = tb.get();
    table[mod_->table_name(i)] = std::move(tb);

    ret = tb_tmp->InitTable(this, i);
    if (!ret)
      cout << "Error in loading table: " << mod_->table_name(i) << endl;
  }
  return ret;
}

ebpf::BPFModule *IOModule::GetBpfModule() const { return mod_.get(); }

FileSystem *IOModule::GetFileSystemHandler() const { return fs_.get(); }

std::vector<Table> IOModule::ShowStates(string module_name) {
  // Lookup the module in the filesystem
  // return the all the table object beloging to the module
  string uuid_str;
  string pathname;
  bool ret;
  vector<Table> tables;
  vector<string> v;
  string p;

  uuid_str = NameToUuid(module_name);
  ret = fs_->MakePathName(p, this, TABLE, "", false);
  if (!ret) {
    cout << "ERROR DETECTED in making pathname" << endl;
    return tables;
  }
  v = fs_->GetFiles(p);
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
  string pathname;
  bool ret;
  vector<Event> events;
  vector<string> v;
  string p;

  uuid_str = NameToUuid(module_name);
  ret = fs_->MakePathName(p, this, EVENT, "", false);
  if (!ret) {
    cout << "ERROR DETECTED in making pathname" << endl;
    return events;
  }

  v = fs_->GetFiles(p);
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

std::string IOModule::GetUuid() { return uuid; }

Table *IOModule::GetTable(std::string name) { return table[name].get(); }

std::map<const std::string, std::unique_ptr<Event>>::iterator
IOModule::GetFirstEvent() {
  return event.begin();
}
std::map<const std::string, std::unique_ptr<Event>>::iterator
IOModule::GetLastEvent() {
  return event.end();
}
std::map<const std::string, std::unique_ptr<Table>>::iterator
IOModule::GetFirstTable() {
  return table.begin();
}
std::map<const std::string, std::unique_ptr<Table>>::iterator
IOModule::GetLastTable() {
  return table.end();
}

}  // namespace iov
