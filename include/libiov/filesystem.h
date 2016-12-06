/*
 * Copyright (c) 2016, PLUMgrid, http://plumgrid.com
 *
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

#include <dirent.h>
#include <sys/stat.h>
#include <uuid/uuid.h>
#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <string>
#include <vector>

// Requires the following module to be install
// apt-get install libboost-filesystem-dev
// apt-get install uuid uuid-dev

#include "libiov/types.h"

#define RootPath "/sys/fs/bpf/"
#define LibiovRootPath "/sys/fs/bpf/libiov/"
#define GlobalTablePath "tables/"
#define ModulePath "modules/"

#define LibiovPath "libiov/"
#define ModuleEventPath "events/"
#define StatePath "state/"
#define MetadataPath "metadata/"
#define KeyDesc "_key_desc"
#define LeafDesc "_leaf_desc"
#define UUID_LEN 100

namespace iov {

class IOModule;

enum obj_type_t { EVENT = 1, TABLE, META };

class FileSystem {
  // Root of the filesystem is /sys/fs/bpf/libiov.
  // Ex: ls /sys/fs/bpf/libiov
  // tables/ modules/
  // Tables will have all the global tables, shared or not by IOModules
  // Modules will have all the IOModule.
  // Ex: ls /sys/fs/bpf/libiov/modules
  // <module_1>/ <module_2>/ etc....
  // where <module_id>, unique for each IOModule
  // Ex: ls /sys/fs/bpf/libiov/<module_id>
  // events/ state/ libiov/
  // Events has the handlers (like ingress/egress packet, kprobe, etc...)
  // State is a list of tables local to the IOModule
  // Libiov has the metadata for each table under state/

 private:
  void ProcessEntry(std::string directory, std::vector<std::string> &files);
  void ProcessEntity(struct dirent *entity, std::vector<std::string> &files);
  void ProcessFile(std::string file, std::vector<std::string> &files);
  bool dirExists(std::string dir_path);
  int DeleteFilesInDirectory(std::string dirpath, bool recursive);
  std::string root_path;
  std::string m_file;
  std::string t_file;
  std::string e_file;

 public:
  FileSystem();
  FileSystem(std::string prefix);
  FileSystem(std::string prefix, std::string e_data, std::string t_data,
      std::string m_data);
  ~FileSystem();
  int Save(std::string p, int fd);
  int Open(std::string file);
  void Show(std::string pathname, std::vector<std::string> &files);
  int Delete(std::string pathname, bool recursive);
  bool MakePathName(std::string &p, IOModule *module, obj_type_t obj_type,
      std::string name, bool global);
  bool Replace(
      std::string &str, const std::string &from, const std::string &to);
  std::vector<std::string> GetFiles(std::string p);
  std::string GetTableFile();
  std::string GetMetaFile();
  std::string GetEventFile();
  int createPath(mode_t mode, const std::string &rootPath, std::string &path);
};
}  // namespace iov
