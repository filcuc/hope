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

#include "hope/connection.h"
#include "hope/event.h"
#include "hope/eventhandler.h"
#include "hope/eventloop.h"
#include "hope/private/threaddata.h"
#include "hope/private/indexsequence.h"
#include "hope/private/queuedinvokationevent.h"

#include <atomic>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>

namespace hope {

template<class ... Args>
class Signal {
    struct SignalHandler {
        SignalHandler(std::function<void(Args...)> callback)
            : m_valid(true)
            , m_receiver_callback(std::move(callback))
        {}

        bool valid() const {
            return m_valid;
        }

        void exec(Args... args) const {
            m_receiver_callback(std::forward<Args>(args)...);
        }

        std::atomic<bool> m_valid;
        std::function<void(Args...)> m_receiver_callback;
    };

public:
    Signal() : m_thread_id(std::this_thread::get_id()) {}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) noexcept = default;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) noexcept = default;

    void emit(Args&&... args) {
        for (const auto& pair : handlers()) {
            if (pair.second->valid())
                pair.second->exec(std::forward<Args>(args)...);
        }
    }

    template<typename Handler>
    Connection connect(Handler handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.emplace(get_next_connection_id(), std::make_shared<SignalHandler>(handler));
        return it.first->first;
    }

    template<typename T, typename std::enable_if<std::is_base_of<EventHandler, T>::value, int>::type = 0>
    Connection connect(T* handler, void(T::*func)(Args...args)) {
        return connect([this, handler, func] (Args...args) {
            if (std::this_thread::get_id() != handler->thread_id()) /* QueuedConnectoin */ {
                auto event = make_queued_invokation_event(handler, func, std::move(args)...);
                ThreadDataRegistry::instance().thread_data(handler->thread_id())->push_event(std::move(event));
            } else {
                (handler->*func)(std::move(args)...);
            }
        });
    }

    void disconnect(Connection c) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.find(c);
        if (it != m_handlers.end()) {
            it->second->m_valid = false;
            m_handlers.erase(it);
        }
    }

private:
    std::map<Connection, std::shared_ptr<SignalHandler>> handlers() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_handlers;
    }
    
    
    Connection get_next_connection_id() {
        return m_next_connection_id++;
    }

    mutable std::mutex m_mutex;
    const std::thread::id m_thread_id;
    std::map<Connection, std::shared_ptr<SignalHandler>> m_handlers;
    int64_t m_next_connection_id = 0;
};

template <>
class Signal<void> {
    struct SignalHandler {
        SignalHandler(std::function<void()> callback)
            : m_valid(true)
            , m_receiver_callback(std::move(callback))
        {}

        bool valid() const {
            return m_valid;
        }

        void exec() const {
            m_receiver_callback();
        }

        std::atomic<bool> m_valid;
        std::function<void()> m_receiver_callback;
    };

public:
    Signal() : m_thread_id(std::this_thread::get_id()) {}
    Signal(const Signal& other) = delete;
    Signal(Signal&& other) = default;
    Signal& operator=(const Signal& other) = delete;
    Signal& operator=(Signal&& other) = default;

    void emit() {
        for (const auto& pair : handlers()) {
            if (pair.second->valid())
                pair.second->exec();
        }
    }

    template<typename Handler>
    Connection connect(Handler handler) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.emplace(get_next_connection_id(), std::make_shared<SignalHandler>(handler));
        return it.first->first;
    }

    template<typename T, typename std::enable_if<std::is_base_of<EventHandler, T>::value, int>::type = 0>
    Connection connect(T* handler, void(T::*func)()) {
        return connect([this, handler, func] {
            if (std::this_thread::get_id() != handler->thread_id()) /* QueuedConnectoin */ {
                auto event = make_queued_invokation_event(handler, func);
                ThreadDataRegistry::instance().thread_data(handler->thread_id())->push_event(std::move(event));
            } else {
                (handler->*func)();
            }
        });
    }

    void disconnect(Connection c) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_handlers.find(c);
        if (it != m_handlers.end()) {
            it->second->m_valid = false;
            m_handlers.erase(it);
        }
    }

private:
    std::map<Connection, std::shared_ptr<SignalHandler>> handlers() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_handlers;
    }
    
    Connection get_next_connection_id() {
        return m_next_connection_id++;
    }

    mutable std::mutex m_mutex;
    const std::thread::id m_thread_id;
    std::map<Connection, std::shared_ptr<SignalHandler>> m_handlers;
    int64_t m_next_connection_id = 0;
};

}

