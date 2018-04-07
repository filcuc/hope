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
    std::mutex mutex;
    mutex.lock();
    m_thread = std::thread([&]{
        EventLoop k;
        m_event_loop = &k;
        mutex.unlock();
        k.exec();
    });
    mutex.lock();
}

void Thread::quit() {
    if (m_event_loop && m_event_loop->is_running())
        m_event_loop->quit();
}

void Thread::wait() {
    if (m_thread.joinable())
        m_thread.join();
}

}
