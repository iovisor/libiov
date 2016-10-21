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

#include <future>
#include <string>
#include <iostream>
#include <uuid/uuid.h>

#include "libiov/table.h"
#include "libiov/event.h"

class Module {

  // Module should be a collection of tables and/or events, where tables
  // collect the states of a module (counters, lookups etc..) and events
  // are the ingress and egress packet, kprobe etc... handlers

  // Random number that uniquily identify a module. Look at filestem.h
  // for filesystem layout
  std::map<std::string, uuid_t> prog_uuid;

  // Human readable name of the module to translate the uuid
  std::string name;

  struct Properties {
    // Tables associated to this module
    std::vector<Table> tables;

    // Event associated to this module
    std::vector<Event> events;
  };

  Module();
  ~Module();

  // Api to retrive uuid from prog_name
  uuid_t *NameToUuid(std::string module_name);

  // Api to list all the name's properties of a module. Like tables and events.
  Properties ShowModule(std::string module_name);

  // Api to display one/all table states for a module
  std::map<std::string, Table> LocalTableStates(std::string table_name);

  // Api to display all events for a module
  std::vector<Event> LocalEvent();

};
