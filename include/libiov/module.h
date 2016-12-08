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

#include <uuid/uuid.h>
#include <uuid/uuid.h>
#include <future>
#include <string>

#include "libiov/event.h"
#include "libiov/filesystem.h"
#include "libiov/table.h"
#include "libiov/types.h"

namespace ebpf {
class BPFModule;
}

namespace iov {

class Table;
class Event;

class IOModule {
  // IOModule should be a collection of tables and/or events, where tables
  // collect the states of a module (counters, lookups etc..) and events
  // are the ingress and egress packet, kprobe etc... handlers

 private:
  std::unique_ptr<ebpf::BPFModule> mod_;
  std::unique_ptr<FileSystem> fs_;
  std::map<const std::string, std::unique_ptr<Event>> event;
  std::map<const std::string, std::unique_ptr<Table>> table;
  std::string uuid;
  size_t num_functions;
  size_t num_tables;
  std::string name;

 public:
  IOModule();
  IOModule(std::string module_name);
  ~IOModule();

  // Random number that uniquily identify a module. Look at filestem.h
  // for filesystem layout
  std::map<std::string, std::string> prog_uuid;

  void GenerateUuid(std::string &uuid_str);
  bool Load(ModuleType type);
  bool Load(std::string fs_prefix, ModuleType type, bool scope);
  bool Init(
      std::string fs_prefix, std::string &&text, ModuleType type, bool scope);
  bool Init(std::string fs_prefix, ModuleType type, std::string uuid_str,
      std::string even_fd_path, std::string table_pd_path,
      std::string meta_fd_path, bool scope);
  ebpf::BPFModule *GetBpfModule() const;
  FileSystem *GetFileSystemHandler() const;

  // Api to retrive uuid from prog_name
  std::string NameToUuid(std::string module_name);

  // Api to display one/all table states for a module
  std::vector<Table> ShowStates(std::string module_name);

  // Api to display all events for a module
  std::vector<Event> ShowEvents(std::string module_name);
  std::string GetUuid();
  Table *GetTable(std::string name);
  std::map<const std::string, std::unique_ptr<Event>>::iterator GetFirstEvent();
  std::map<const std::string, std::unique_ptr<Event>>::iterator GetLastEvent();
  std::map<const std::string, std::unique_ptr<Table>>::iterator GetFirstTable();
  std::map<const std::string, std::unique_ptr<Table>>::iterator GetLastTable();
};

}  // namespace iov
