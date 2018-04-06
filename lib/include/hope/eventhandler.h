#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

class Event;
class EventLoop;

extern thread_local EventLoop* m_current_event_loop;

class EventHandler {
public:
    virtual ~EventHandler() = default;

    virtual void on_event(Event& event) = 0;
};


#endif // EVENTHANDLER_H
