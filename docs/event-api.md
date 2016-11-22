## Event API

### Classes overview

#### Entities
* Bus
* IOmodule
* Filter
* Action
* Generator

#### Messages
* Event

### Classes details

In the API below, "status" is an appropriate type to return a status code and ancillary context (e.g. error string).

#### Bus
A Bus defines a mediator to exchange Events of a given type among Generators and IOmodules.

* Bus(string name, Event::Type type) - create a new Bus by name, handling events of a specific type
* {status, SendHandle, RecvHandle} join(IOmodule m) - attach an IOmodule to a Bus; since an IOmodule can also both send and receive events to/from a Bus, two handles are returned
* {status, SendHandle} join(Generator g) - attach an Event Generator to a Bus; an Event Generator can only send events to a Bus
* static Bus get_bus_by_name(string name) - return a Bus given its name, or an error otherwise

##### Bus::SendHandle
* status send(Event e) - send an Event to the bus - the request can fail for a number of reasons (e.g., if the sender has not joined the Bus, or if the Event is of the wrong type)
* status leave() - disconnect the sender from the Bus

##### Bus::RecvHandle
* Event subscribe(Filter f, Action a) - subscribe a Filter and Action to this Bus; when an Event that matches the Filter is sent on the Bus, the provided Action is called
* status leave() - disconnect the receiver from the Bus

#### IOmodule
An IOmodule attaches to a Bus to receive and possibly send Events.

* IOmodule() - create a new IOmodule


#### Filter
A Filter allows an IOmodule to capture a subset of the events traveling on a Bus.

* Filter(string filter) - create a Filter to receive a specific set of events from a Bus; the format of the filter string is specific to the implementation and event
* bool filter(Event e) - callback invoked when a new Event has been received; it should return _true_ when the Event matches the Filter


#### Action
An Action is used to perform some actions when a given Filter is triggered.

* Action() - create an Action
* status process(Event e) - callback invoked when a new Event has been received


#### Generator
A Generator creates events and sends them over a bus.

* Generator() - create an event generator


#### Event
An Event instance carries information about a given system event. The specific details of the Event class depend on the event type.

* Event() - create an event
