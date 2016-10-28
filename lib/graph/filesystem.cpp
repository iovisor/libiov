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

FileSystem::FileSystem() {
root_path = LibiovRootPath;
}
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

void FileSystem::ProcessEntry(std::string directory,
                              std::vector<std::string> &files)
{
    std::string dirToOpen = root_path;

    if (directory.compare("libiov") != 0 ) {
       dirToOpen = root_path + directory;
       //set the new path for the content of the directory
       root_path = dirToOpen + "/";
    }
    auto dir = opendir(dirToOpen.c_str());

    if(NULL == dir)
    {
        std::cout << "could not open directory: " << dirToOpen.c_str() << std::endl;
        return;
    }

    auto entity = readdir(dir);

    while(entity != NULL)
    {
        ProcessEntity(entity, files);
        entity = readdir(dir);
    }

    //we finished with the directory so remove it from the path
    root_path.resize(root_path.length() - 1 - directory.length());
    closedir(dir);
}

void FileSystem::ProcessEntity(struct dirent* entity,
                               std::vector<std::string> &files)
{
    //find entity type
    if (entity->d_type == DT_DIR)
    {//it's an direcotry
        //don't process the  '..' and the '.' directories
        if(entity->d_name[0] == '.')
        {
            return;
        }
        //it's an directory so process it
        ProcessEntry(std::string(entity->d_name), files);
        return;
    }

    if (entity->d_type == DT_REG)
    {//regular file
        ProcessFile(std::string(entity->d_name), files);
        return;
    }

    std::cout << "Not a file or directory: " << entity->d_name << std::endl;
}

void FileSystem::ProcessFile(std::string file,
                             std::vector<std::string> &files)
{
    files.push_back(file);
}

/* Show
 * api to show the filesystem. Conceptually same as:
 * Ex: ls /sys/fs/bpf/libiov
 * tables/ modules/
 * pathname will have the starting location in the filesytem. Check filesytem.h
 * for more info.
 */

void FileSystem::Show(std::string pathname, std::vector<std::string> &files) {
     ProcessEntry(pathname, files);
}

bool FileSystem::dirExists(std::string dir_path) {
    struct stat sb;

    if (stat(dir_path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    else
        return false;
}

int FileSystem::DeleteFilesInDirectory(std::string dirpath, bool recursive) {

    if (dirpath.empty())
        return 0;

    auto *folder = opendir(dirpath.c_str());
    if (folder == NULL)
        return errno;

    struct dirent *next_file;
    std::string filepath;
    int ret_val;

    while ( (next_file = readdir(folder)) != NULL )
    {
        if (next_file->d_name[0] == '.')
        {
           continue;
        }
        filepath = dirpath + "/" + next_file->d_name;
        //dirExists will check if the "filepath" is a directory
        if (dirExists(filepath))
        {
            if (!recursive)
                //if we aren't recursively deleting in subfolders, skip this dir
                 continue;

            ret_val = DeleteFilesInDirectory(filepath, recursive);

            if (ret_val != 0)
            {
                closedir(folder);
                return ret_val;
            }
        }

        ret_val = remove(filepath.c_str());

        if (ret_val != 0 && ret_val != ENOENT)
        {
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

int FileSystem::Delete(std::string pathname, bool recursive) {
    pathname = root_path + pathname;
    DeleteFilesInDirectory(pathname, recursive);
    return 0;
}

void FileSystem::GenerateUuid(char *uuid_str) {
     uuid_t id1;
     uuid_generate( (unsigned char *)&id1 );
     uuid_unparse(id1, uuid_str);
}
} // namespace iov
