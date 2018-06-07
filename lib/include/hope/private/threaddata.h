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

#pragma once

#include <hope/eventloop.h>

#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace hope {

class EventLoop;

class ThreadData {
public:
    ThreadData() = default;

    ThreadData(std::thread::id id, EventLoop* event_loop)
        : m_id(id)
        , m_event_loop(event_loop)
    {}

    std::thread::id thread_id() const {
        return m_id;
    }

    void set_event_loop(EventLoop* event_loop) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_event_loop = event_loop;
    }

    void register_event_handler(EventHandler* handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_event_loop)
            m_event_loop->register_event_handler(handler);
        else
            std::cerr << "No event event loop when registering handler " << handler << std::endl;
    }

    void unregister_event_handler(EventHandler* handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_event_loop)
            m_event_loop->unregister_event_handler(handler);
        else
            std::cerr << "No event event loop when unregistering handler " << handler << std::endl;
    }

    template<typename ...T>
    void push_event(T... args) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_event_loop)
            m_event_loop->push_event(std::forward<T>(args)...);
        else
            std::cerr << "No event event loop when pushing event" << std::endl;
    }

private:
    mutable std::mutex m_mutex;
    const std::thread::id m_id;
    EventLoop* m_event_loop = nullptr;
};

class ThreadDataRegistry {
public:
    static ThreadDataRegistry& instance();

    std::shared_ptr<ThreadData> thread_data(const std::thread::id& id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_registry.find(id);
        if (it == m_registry.end()) {
            it = m_registry.emplace(id, std::make_shared<ThreadData>(id, nullptr)).first;
        }
        return it->second;
    }

    std::shared_ptr<ThreadData> current_thread_data() {
        return thread_data(std::this_thread::get_id());
    }

private:
    ThreadDataRegistry() = default;

    mutable std::mutex m_mutex;
    mutable std::unordered_map<std::thread::id, std::shared_ptr<ThreadData>> m_registry;
};

}
