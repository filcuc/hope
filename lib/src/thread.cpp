#include "hope/thread.h"

#include "hope/eventloop.h"
#include "hope/threaddata.h"

#include <cassert>
#include <future>

namespace hope {

Thread::Thread() = default;

Thread::~Thread()
{
    quit();
    wait();
}

std::thread::id Thread::id() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_thread.get_id();
}

void Thread::start()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_state == State::Starting || m_state == State::Starting) {
        std::cerr << "Thread already started" << std::endl;
        return;
    }

    if (m_state == State::Stopping) {
        m_cond.wait(lock, [this]{ return m_state == State::Stopped; });
    }

    if (m_thread.joinable())
        m_thread.join();

    assert(m_state == State::Stopped);
    m_state = State::Starting;
    m_cond.notify_all();

    m_thread = std::thread([this] { run(); });
    m_cond.wait(lock, [this]{ return m_state == State::Started; });
    assert(m_state == State::Started);
}

void Thread::quit()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_state == State::Stopping || m_state == State::Stopped)
        return;

    if (m_state == State::Starting) {
        m_cond.wait(lock, [this]{ return m_state == State::Started; });
    }

    assert(m_state == State::Started);
    m_state = State::Stopping;
    m_cond.notify_all();
    m_event_loop->quit();
}

void Thread::wait()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_state != State::Stopped) {
        m_cond.wait(lock, [this]{ return m_state == State::Stopped; });
    }

    if (m_thread.joinable())
        m_thread.join();
}

void Thread::run()
{
    EventLoop event_loop;

    {
        m_mutex.lock();
        assert(m_state == State::Starting);
        m_event_loop = &event_loop;
        ThreadDataRegistry::get_instance().set_thread_data(ThreadData(std::this_thread::get_id(), m_event_loop));
        m_state = State::Started;
        m_cond.notify_all();
        m_mutex.unlock();
    }

    event_loop.exec();

    {
        m_mutex.lock();
        assert(m_state == State::Stopping);
        ThreadDataRegistry::get_instance().set_thread_data(ThreadData(std::this_thread::get_id(), nullptr));
        m_event_loop = nullptr;
        m_state = State::Stopped;
        m_cond.notify_all();
        m_mutex.unlock();
    }
}

}
