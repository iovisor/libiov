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

#include "libiov/event.h"
#include "libiov/filesystem.h"
#include "libiov/internal/types.h"
#include "libiov/table.h"

using std::future;
using std::promise;
using namespace iov::internal;
using namespace std;

namespace iov {

FileSystem::FileSystem() { root_path = RootPath; }
FileSystem::FileSystem(string prefix) { root_path = RootPath + prefix; }
FileSystem::FileSystem(
    string prefix, string e_data, string t_data, string m_data) {
  root_path = RootPath + prefix;
  e_file = e_data;
  m_file = m_data;
  t_file = t_data;
}
FileSystem::~FileSystem() {}

/* Save
 * Api to save the file descriptor of a bpf program, bpf table.
 * pathname will have the location in the filesytem. Check filesytem.h
 * for more info.
 * file_name is the file where the fd will be save
 */

int FileSystem::Save(string p, int fd) {
  int ret = 0;
  ret = bpf_obj_pin(fd, p.c_str());
  return ret;
}

int FileSystem::Open(string file) {
  int ret = 0;
  ret = bpf_obj_get(file.c_str());
  return ret;
}

void FileSystem::ProcessEntry(string directory, vector<string> &files) {
  string dirToOpen = root_path;

  if (directory.compare("libiov") != 0) {
    dirToOpen = root_path + directory;
    // set the new path for the content of the directory
    root_path = dirToOpen + "/";
  }
  auto dir = opendir(dirToOpen.c_str());

  if (NULL == dir) {
    cout << "could not open directory: " << dirToOpen.c_str() << endl;
    return;
  }

  auto entity = readdir(dir);

  while (entity != NULL) {
    ProcessEntity(entity, files);
    entity = readdir(dir);
  }

  // we finished with the directory so remove it from the path
  root_path.resize(root_path.length() - 1 - directory.length());
  closedir(dir);
}

void FileSystem::ProcessEntity(struct dirent *entity, vector<string> &files) {
  // find entity type
  if (entity->d_type == DT_DIR) {
    // it's an direcotry
    // don't process the  '..' and the '.' directories
    if (entity->d_name[0] == '.')
      return;
    // it's an directory so process it
    ProcessEntry(string(entity->d_name), files);
    return;
  }

  if (entity->d_type == DT_REG) {
    // regular file
    ProcessFile(string(entity->d_name), files);
    return;
  }

  cout << "Not a file or directory: " << entity->d_name << endl;
}

void FileSystem::ProcessFile(string file, vector<string> &files) {
  cout << "FILE: " << file << endl;
  files.push_back(file);
}

/* Show
 * api to show the filesystem. Conceptually same as:
 * Ex: ls /sys/fs/bpf/libiov
 * tables/ modules/
 * pathname will have the starting location in the filesytem. Check filesytem.h
 * for more info.
 */

void FileSystem::Show(string pathname, vector<string> &files) {
  ProcessEntry(pathname, files);
}

bool FileSystem::dirExists(string dir_path) {
  struct stat sb;

  if (stat(dir_path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    return true;
  else
    return false;
}

int FileSystem::DeleteFilesInDirectory(string dirpath, bool recursive) {
  if (dirpath.empty())
    return 0;
  auto *folder = opendir(dirpath.c_str());
  if (folder == NULL)
    return errno;

  struct dirent *next_file;
  string filepath;
  int ret_val;

  while ((next_file = readdir(folder)) != NULL) {
    if (next_file->d_name[0] == '.') {
      continue;
    }
    filepath = dirpath + "/" + next_file->d_name;
    // dirExists will check if the "filepath" is a directory
    if (dirExists(filepath)) {
      if (!recursive)
        // if we aren't recursively deleting in subfolders, skip this dir
        continue;

      ret_val = DeleteFilesInDirectory(filepath, recursive);

      if (ret_val != 0) {
        closedir(folder);
        return ret_val;
      }
    }

    ret_val = remove(filepath.c_str());

    if (ret_val != 0 && ret_val != ENOENT) {
      closedir(folder);
      return ret_val;
    }
  }

  closedir(folder);
  return 0;
}

/* Delete
 * api to delete a specific file <file_name>.
 * pathname will have the location in the filesytem. Check filesytem.h
 * for more info.
 */

int FileSystem::Delete(string pathname, bool recursive) {
  pathname = RootPath + pathname;
  DeleteFilesInDirectory(pathname, recursive);
  return 0;
}

bool FileSystem::Replace(string &str, const string &from, const string &to) {
  size_t start_pos = str.find(from);
  if (start_pos == string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

int FileSystem::createPath(
    mode_t mode, const std::string &rootPath, std::string &path) {
  struct stat st;

  for (std::string::iterator iter = path.begin(); iter != path.end();) {
    std::string::iterator newIter = std::find(iter, path.end(), '/');
    std::string newPath = rootPath + std::string(path.begin(), newIter);

    if (stat(newPath.c_str(), &st) != 0) {
      if (mkdir(newPath.c_str(), mode) != 0 && errno != EEXIST) {
        std::cout << "cannot create folder [" << newPath
                  << "] : " << strerror(errno) << std::endl;
        return -1;
      }
    } else if (!S_ISDIR(st.st_mode)) {
      errno = ENOTDIR;
      std::cout << "path [" << newPath << "] not a dir " << std::endl;
      return -1;
    }

    iter = newIter;
    if (newIter != path.end())
      ++iter;
  }
  return 0;
}

bool FileSystem::MakePathName(string &p, IOModule *module, obj_type_t obj_type,
    string name, bool global) {
  string pathname;
  string uuid = module->GetUuid();
  switch (obj_type) {
  case EVENT: {
    pathname.append(ModulePath)
        .append(uuid)
        .append("/")
        .append(ModuleEventPath);
  } break;
  case TABLE: {
    if (global) {
      pathname.append(GlobalTablePath).append(uuid).append("/");
    } else {
      pathname.append(ModulePath).append(uuid).append("/").append(StatePath);
    }
  } break;
  default:
    return false;
  }

  if (createPath(0777, root_path.c_str(), pathname)) {
    cout << "mkdir " << pathname << " failed: " << strerror(errno) << endl;
    return false;
  }
  if (!name.empty()) {
    pathname.append(name).append("/");
    if (createPath(0777, root_path.c_str(), pathname)) {
      cout << "mkdir " << pathname << " failed: " << strerror(errno) << endl;
      return false;
    }
  }

  p = root_path + pathname.c_str();
  return true;
}

vector<string> FileSystem::GetFiles(string p) {
  vector<string> v;

  if (p.empty())
    return v;

  auto *folder = opendir(p.c_str());
  if (folder == NULL) {
    std::cout << "Could not open dir: " << p << std::endl;
    return v;
  }

  struct dirent *next_file;

  while ((next_file = readdir(folder)) != NULL) {
    if (next_file->d_name[0] == '.') {
      continue;
    }
    v.push_back(next_file->d_name);
  }
  return v;
}

string FileSystem::GetTableFile() { return t_file; }
string FileSystem::GetMetaFile() { return m_file; }
string FileSystem::GetEventFile() { return e_file; }
}  // namespace iov
