/*
    Copyright (C) 2018 Filippo Cucchetto.
    Contact: https://github.com/filcuc/hope

    This file is part of the Hope library.

    The Hope library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    The Hope library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Hope library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "hope/thread.h"

#include "hope/object.h"
#include "hope/eventloop.h"
#include "hope/private/threaddata.h"

#include <cassert>
#include <future>

using namespace hope;

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
        m_cond.wait(lock, [this] { return m_state == State::Stopped; });
    }

    if (m_thread.joinable())
        m_thread.join();

    assert(m_state == State::Stopped);
    m_state = State::Starting;
    m_cond.notify_all();

    m_thread = std::thread([this] { exec(); });
    m_cond.wait(lock, [this] { return m_state == State::Started; });
    assert(m_state == State::Started);
}

void Thread::quit()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_state == State::Stopping || m_state == State::Stopped)
        return;

    if (m_state == State::Starting) {
        m_cond.wait(lock, [this] { return m_state == State::Started; });
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
        m_cond.wait(lock, [this] { return m_state == State::Stopped; });
    }

    if (m_thread.joinable())
        m_thread.join();
}

void Thread::move_to_thread(std::unique_ptr<Object> obj)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_state == State::Stopping || m_state == State::Stopped) {
        std::cerr << "Trying to move and object to a thread that has not been started" << std::endl;
    }

    if (m_state == State::Starting) {
        m_cond.wait(lock, [this] { return m_state == State::Started; });
    }

    obj->move_to_thread(m_thread.get_id());
    assert(m_state == State::Started);
    m_children.push_back(std::move(obj));
}

void Thread::exec()
{
    {
        m_mutex.lock();
        assert(m_state == State::Starting);
        m_event_loop.reset(new EventLoop());
        m_state = State::Started;
        m_cond.notify_all();
        m_mutex.unlock();
    }

    m_event_loop->exec();

    {
        m_mutex.lock();
        assert(m_state == State::Stopping);
        m_children.clear();
        ThreadDataRegistry::instance().current_thread_data()->set_event_loop(nullptr);
        m_event_loop.reset();
        m_state = State::Stopped;
        m_cond.notify_all();
        m_mutex.unlock();
    }
}
