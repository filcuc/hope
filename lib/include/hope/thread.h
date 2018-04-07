#pragma once

#include <thread>

namespace hope {

class EventLoop;

class Thread {
public:
    ~Thread();

    std::thread::id id() const;

    void start();

    void quit();

    void wait();

    EventLoop* event_loop() { return m_event_loop; }

private:
    std::thread m_thread;
    EventLoop* m_event_loop = nullptr;
};

}
