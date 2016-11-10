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

#include "libiov/types.h"

class Event {
public:

 // Name of the event
 std::string event_name;

 // File descriptor of the event to store 
 int event_fd;

 // cls_bpf, xdp etc...
 int event_type;

 // handler for ingress or egress
 int direction;

 Event();
 ~Event();

 // Api to display property of the event
 void ShowEvent(std::string event_name);

 // Apis' to Insert/Delete an event
 int Insert(std::string event_name);
 int Delete(std::string event_name);

};

