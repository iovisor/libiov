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

* Filter() - create an empty Filter, that matches nothing; needed if the user wants to extend this class to enable extra functionality;
* Filter(string filter) - create a Filter to receive a specific set of events from a Bus; the format of the filter string is specific to the implementation and event;
* bool filter(const Event e) - callback invoked when a new Event has been received; it should return _true_ when the Event matches the Filter.

Example:
````C++
/* example of a fictitious filter string to match NetworkPacket events representing IPv4 packets */
Filter f1("event.ethertype == 0x0800");

/* a more complicated way to achieve the same result: filter a generic ethertype provided dynamically */
class EtherTypeFilter : public Filter {
  EtherTypeFilter(uint16_t type) : Filter(), type_(type) { }
  
  bool filter(const Event e) {
    const NetworkPacket n = (NetworkPacket) e;

    return (n.ethertype == type_);
  }
  
  uint16_t type_;
};

EtherTypeFilter f2(0x0800);
````


#### Action
An Action is used to perform some actions when a given Filter is triggered.

* Action() - create an empty Action, that performs no operation; needed if the user needs to extend this class to enable extra functionality;
* Action(functor f) - create an Action that executes the provided code; the functor code uses a set of primitives provided by the system to process events;
* Action(string code) - create an Action that executes the provided code; the string is "compiled" to use the internal implementation provided by the system to process events;
* Status process(Event e) - callback invoked when a new Event has been received; if not overridden, the base implementation provides a system-dependent set of base actions. The implementation might put some restrictions on the complexity of the overriding code, or might execute it in a sandboxed/slower environment.

Example:
````C++
/* Example of a basic action to handle network packets (i.e. fictitious NetworkPacket events).
 *
 * The action decreases the TTL in the IPv4 header of the packet, and sends it to another bus "Bus2"
 * if it is still nonzero; the packet event is dropped otherwise.
 *
 * The same action is performed with slightly different types of syntax.
 */

Action a1("event.ipv4.ttl--;
  if (event.ipv4.ttl == 0)
    return ACTION_DROP(e);
  return ACTION_BUS_SEND(e, \"Bus2\");
");

Action a2([](Event e) {
  NetworkPacket n = (NetworkPacket) e;
  n.ipv4.ttl--;
  if (n.ipv4.ttl == 0) {
    e.drop();
    return;
  }
  Bus b = Bus::get_bus_by_name("Bus2");
  auto s = b.join(this);
  s[1].send(e);
  return;
});

class MyAction : public Action {
  MyAction(SendHandle h) : Action(), handle_(h) { }
  
  Status process(Event e) {
    NetworkPacket n = (NetworkPacket) e;
    n.ipv4.ttl--;
    if (n.ipv4.ttl == 0) {
      e.drop();
      return OK;
    }
    handle_.send(e);
    return OK;
  }

  SendHandle handle_;
};

Bus b = Bus::get_bus_by_name("Bus2");
auto join_status = b.join(some_module);
if (join_status[0] != OK)
  return ERROR;
MyAction a3(join_status[2]);
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
