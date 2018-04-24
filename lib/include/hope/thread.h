#pragma once

#include <condition_variable>
#include <thread>

namespace hope {

class EventLoop;

class Thread {
public:
    Thread();

    ~Thread();

    std::thread::id id() const;

    void start();

    void quit();

    void wait();

    EventLoop* event_loop() { return m_event_loop; }

private:
    void run();

    enum class State {
        Starting,
        Started,
        Stopping,
        Stopped
    };

    mutable std::mutex m_mutex;
    State m_state = State::Stopped;
    std::condition_variable m_cond;
    std::thread m_thread;
    EventLoop* m_event_loop = nullptr;
};

}
