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

#include <bcc/bpf_common.h>
#include <bcc/bpf_module.h>
#include <bcc/libbpf.h>

#include "libiov/internal/types.h"
#include "libiov/table.h"
#include "libiov/module.h"
#include "libiov/filesystem.h"
#include "libiov/event.h"

using std::future;
using std::promise;
using std::string;
using namespace iov::internal;
using namespace boost::filesystem;
using namespace std;

namespace iov {

IOModule::IOModule() {}
IOModule::~IOModule() {}

future<bool> IOModule::Init(string &&text) {
  future<bool> res = std::async(std::launch::async,
      [this](string &&text) -> bool {
        mod_ = make_unique<ebpf::BPFModule>(0);
        if (mod_->load_string(text, nullptr, 0) < 0) 
          return false;
        return true;
      },
      std::move(text));
  return res;
}

ebpf::BPFModule *IOModule::GetBpfModule() const {
  return mod_.get();
}

std::vector<Table> IOModule::ShowStates(string module_name) {
  //Lookup the module in the filesystem
  //return the all the table object beloging to the module
  string uuid_str;
  FileSystem fs;
  string pathname;
  int ret;
  vector<Table> tables;
  vector<string> v;


  uuid_str = NameToUuid(module_name);
  ret = fs.MakePathName(pathname,
                        uuid_str,
                        TABLE,
                        "",
                        false);
  if (ret) {
     cout << "ERROR DETECTED in making pathname" << endl;
     return tables;
  }
  v = fs.GetFiles(pathname);
  cout << "TABLES:" << endl;
  for (vector<string>::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
  {
    cout << "  " << *it << endl;
  }

  return tables;
}

vector<Event> IOModule::ShowEvents(string module_name) {
  //Lookup the module in the filesystem
  //return the all the event object beloging to the module
  string uuid_str;
  FileSystem fs;
  string pathname;
  int ret;
  vector<Event> events;
  vector<string> v;

  uuid_str = NameToUuid(module_name);
  ret = fs.MakePathName(pathname,
                        uuid_str,
                        EVENT,
                        "",
                        false);
  if (ret) {
     cout << "ERROR DETECTED in making pathname" << endl;
     return events;
  }

  v = fs.GetFiles(pathname);

  cout << "EVENTS:" << endl;
  for (vector<string>::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
  {
    cout << "  " << *it << endl;
  }

  return events;

}

string IOModule::NameToUuid(string module_name) {
  map<string, string>::const_iterator pos = prog_uuid.find(module_name);
  string uuid;
  if (pos == prog_uuid.end()) {
    uuid.empty();
  } else {
    uuid = pos->second;
  }
  return uuid;   
}

}  // namespace iov
