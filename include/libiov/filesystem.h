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
#include <future>
#include <string>
#include <iostream>
#include <fstream>
#include <uuid/uuid.h>

#include "libiov/types.h"

#define LibiovRootPath    "/sys/fs/bpf/libiov/"
#define GlobalTablePath   "/sys/fs/bpf/libiov/tables/"
#define ModulePath        "/sys/fs/bpf/libiov/modules/"

#define ModuleEventPath   "/events/"
#define StatePath         "/state/"  
#define MetadataPath      "/libiov/"


namespace iov {
  class IOModule;

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

  void ProcessEntry(std::string directory, std::vector<std::string> &files);
  void ProcessEntity(struct dirent* entity, std::vector<std::string> &files);
  void ProcessFile(std::string file, std::vector<std::string> &files);
  bool dirExists(std::string dir_path);
  int DeleteFilesInDirectory(std::string dirpath, bool recursive);
  std::string root_path; 
 
 public:
  FileSystem();
  ~FileSystem();
  int Save(std::string pathname, std::string file_name, int fd);
  int Open(std::string pathname);
  void Show(std::string pathname, std::vector<std::string> &files);
  int Delete(std::string pathname, bool recursive);
  void GenerateUuid(char *uuid_str);
};
} // namespace iov
