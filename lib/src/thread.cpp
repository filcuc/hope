#include "thread.h"

#include "eventloop.h"

#include <future>

namespace hope {

Thread::~Thread() {
    quit();
    wait();
}

std::thread::id Thread::id() const
{
    return m_thread.get_id();
}

void Thread::start() {
    std::promise<void> promise;
    std::future<void> future = promise.get_future();
    m_thread = std::thread([&]() mutable {
        EventLoop k;
        m_event_loop = &k;
        promise.set_value();
        k.exec();
        m_event_loop = nullptr;
    });
    future.wait();
}

void Thread::quit() {
    if (m_event_loop)
        m_event_loop->quit();
}

void Thread::wait() {
    if (m_thread.joinable())
        m_thread.join();
}

}
