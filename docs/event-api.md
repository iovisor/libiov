## Event API

### Classes overview

#### Entities
* Bus
  * Bus::SendHandle
  * Bus::RecvHandle
* IOmodule
* Filter
* Action
* Generator

#### Messages
* Event

### Classes details

In the API below, "Status" is an appropriate type to return a status code and ancillary context (e.g. error string).

#### Bus
A Bus defines a mediator to exchange Events of a given type among Generators and IOmodules.

* Bus(string name, Event::Type type) - create a new Bus by name, handling events of a specific type
* {Status, SendHandle, RecvHandle} join(IOmodule m) - attach an IOmodule to a Bus; since an IOmodule can also both send and receive events to/from a Bus, two handles are returned
* {Status, SendHandle} join(Generator g) - attach an Event Generator to a Bus; an Event Generator can only send events to a Bus
* static Bus get_bus_by_name(string name) - return a Bus given its name, or an error otherwise

Example:
````C++
IOmodule m;
Bus b = Bus::get_bus_by_name("network");
auto join_status = b.join(m);
if (join_status[0] != OK)
  return ERROR;
````


##### Bus::SendHandle
* Status send(Event e) - send an Event to the bus - the request can fail for a number of reasons (e.g., if the sender has not joined the Bus, or if the Event is of the wrong type)
* Status leave() - disconnect the sender from the Bus

Example:
````C++
auto join_status = bus.join(module);
Bus::SendHandle h = join_status[1];

Event e;
Status s = h.send(e);
if (s != OK)
  return ERROR;
````


##### Bus::RecvHandle
* Status subscribe(Filter f, Action a) - subscribe a Filter and Action to this Bus; when an Event that matches the Filter is sent on the Bus, the provided Action is called
* Status leave() - disconnect the receiver from the Bus

Example:
````C++
auto join_status = bus.join(module);
Bus::RecvHandle h = join_status[2];

Filter f(...);
Action a;
Status s = h.subscribe(f, a);
if (s != OK)
  return ERROR;
````

#### IOmodule
An IOmodule attaches to a Bus to receive and possibly send Events.

* IOmodule() - create a new IOmodule

Example:
````C++
class MyModule : public IOmodule {
  MyModule() : IOmodule(), f_(...) { }
  
  Status init() {
    Bus b = Bus::get_bus_by_name("network");
    auto join_status = b.join(this);
    if (join_status[0] != OK)
      return ERROR;

    h_ = join_status[2];
    Status s = h_.subscribe(f_, a_);
    if (s != OK)
      return ERROR;

    return OK;
  }
  
  Filter f_;
  Action a_;
  Bus::RecvHandle h_;
};

MyModule m;
m.init();
````

#### Filter
A Filter allows an IOmodule to capture a subset of the events traveling on a Bus.

* Filter(string filter) - create a Filter to receive a specific set of events from a Bus; the format of the filter string is specific to the implementation and event
* bool filter(Event e) - callback invoked when a new Event has been received; it should return _true_ when the Event matches the Filter

Example:
````C++
class MyFilter : public Filter {
  MyFilter(string s) : Filter(s), f_(s) { }
  
  bool filter(Event e) {
    bool match;
    /* Use string f_ to determine whether Event e matches the filter, or defer to
     * Filter::filter(e), that uses a base implementation, if no specialization needed.
     */
    
    return match;
  }
  
  string f_;
};

MyFilter f("filter string");
````


#### Action
An Action is used to perform some actions when a given Filter is triggered.

* Action() - create an Action
* Status process(Event e) - callback invoked when a new Event has been received

Example:
````C++
class MyAction : public Action {
  MyAction() : Action() { }
  
  Status process(Event e) {
    /* Add code here to perform the requested action.
     * The implementation might define a base implementation for Action::process()
     * that can perform some basic operations, so this method would not be needed.
     */
    return OK;
  }
};

MyAction a;
````

#### Generator
A Generator creates events and sends them over a bus.

* Generator() - create an event generator

Example:
````C++
class MyGen : public Generator {
  MyGen() : Generator() { }
  
  Status init() {
    Bus b = Bus::get_bus_by_name("network");
    auto join_status = b.join(this);
    if (join_status[0] != OK)
      return ERROR;

    h_ = join_status[1];
    /* Setup a thread or notification mechanism to generate events.
     * Use h_ to send events.
     */

    return OK;
  }
  
  Bus::SendHandle h_;
};

MyGen g;
g.init();
````


#### Event
An Event instance carries information about a given system event. The specific details of the Event class depend on the event type.

* Event() - create an event
* Event(Event original) - copy an event
* Status drop() - prevent an event from being delivered further to other IOmodules

Example:
````C++
class MyGen : public Generator {
  MyGen() : Generator() { ... }

  /* Example of a worker method that generates Event instances based on hardware events */
  void worker() {
    while (true) {
      /* wait for a hardware event to happen */
      
      MyHardwardEvent e;
      /* set the properties of the event */
      
      h_.send(e);
    }
  }
  
  Bus::SendHandle h_;
}
````
