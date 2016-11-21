## Event API

### Classes overview

#### Entities
* Bus
* IOmodule
* Filter
* Action
* Generator

#### Messages
* (Event)

### Classes details

In the API below, "status" is an appropriate type to return a status code and ancillary context (e.g. error string).

#### Bus
A Bus defines a platform to exchange Events among Generators and IOmodules.

* Bus(string name) - create a new Bus by name
* {status, SendHandle, RecvHandle} join(IOmodule m) - attach an IOmodule to a Bus; since an IOmodule can also both send and receive events to/from a Bus, two handles are returned
* {status, SendHandle} join(Generator g) - attach an Event Generator to a Bus; an Event Generator can only send events to a Bus
* static Bus get_bus_by_name(string name) - return a Bus given its name, or error

##### Bus::SendHandle
* status send(Event e) - send an Event to the bus - request can fail for a number of reasons (e.g., if the sender has not joined the Bus, or if it is being throttled)
* status leave() - disconnect the sender from the Bus

##### Bus::RecvHandle
* Event recv() - consume next Event from the Bus
* status leave() - disconnect the receiver from the Bus

#### IOmodule
An IOmodule attaches to a Bus to receive and possibly send Events.

* IOmodule() - create a new IOmodule


#### Filter
A Filter allows an IOmodule to capture a subset of the events traveling on a Bus.

* Filter(RecvHandle handle, string filter) - create a Filter to receive a specific set of events from a Bus; the format of the filter string is implementation and event specific
* status dispatch_to(Action a) - when a new Event is received that matches the filter, dispatch it to the provided Action; it can be called multiple times, to dispatch the same event to multiple actions


#### Action
An Action is used to perform some actions when a given Filter is triggered.

* Action() - create an Action
* status process(Event e) - callback invoked when a new Event has been received


#### Generator
A Generator creates events and sends them over a bus.

* Generator() - create an event generator
* status attach(Bus b) - attach the Generator to a Bus
