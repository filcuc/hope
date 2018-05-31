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

#include <map>
#include <memory>
#include <mutex>
#include <thread>

namespace hope {

class EventHandler;

namespace detail {

class EventHandlerData {
public:
    EventHandlerData(std::thread::id id)
        : m_thread_id(std::move(id))
    {}

    static std::unique_lock<std::mutex> lock(const std::shared_ptr<EventHandlerData>& data) {
        return data ? lock(*data) : std::unique_lock<std::mutex>();
    }

    static std::unique_lock<std::mutex> lock(EventHandlerData& data) {
        return std::unique_lock<std::mutex>(data.m_mutex);
    }

    static std::pair<std::unique_lock<std::mutex>,std::unique_lock<std::mutex>> lock(const std::shared_ptr<EventHandlerData>& first,
                                                                                     const std::shared_ptr<EventHandlerData>& second) {
        if (first && second)
            return lock(*first, *second);
        else if (first)
            return { lock(*first), std::unique_lock<std::mutex>() };
        else if (second)
            return { lock(*second), std::unique_lock<std::mutex>() };
        else
            return { std::unique_lock<std::mutex>(), std::unique_lock<std::mutex>() };
    }

    static std::pair<std::unique_lock<std::mutex>,std::unique_lock<std::mutex>> lock(EventHandlerData& first, EventHandlerData& second) {
        EventHandlerData* first_address = &first;
        EventHandlerData* second_address = &second;
        if (first_address >= second_address)
            std::swap(first_address, second_address);
        std::unique_lock<std::mutex> first_lock = lock(*first_address);
        std::unique_lock<std::mutex> second_lock;
        if (first_address != second_address)
            second_lock = lock(*second_address);
        return {std::move(first_lock), std::move(second_lock)};
    }

    std::thread::id m_thread_id;
    std::mutex m_mutex;
};

class EventHandlerDataRegistry {
public:
    static EventHandlerDataRegistry& instance();

    EventHandlerDataRegistry(const EventHandlerDataRegistry&) = delete;
    EventHandlerDataRegistry(EventHandlerDataRegistry&&) = delete;
    EventHandlerDataRegistry& operator=(const EventHandlerDataRegistry&) = delete;
    EventHandlerDataRegistry& operator=(EventHandlerDataRegistry&&) = delete;

    std::weak_ptr<EventHandlerData> data(EventHandler* handler) {
        auto it = m_data.find(handler);
        return it != m_data.end() ? it->second : std::weak_ptr<EventHandlerData>();
    }

    void register_event_handler_data(EventHandler* handler, const std::shared_ptr<EventHandlerData>& data) {
        m_data.emplace(handler, data);
    }

    void unregister_event_handler_data(EventHandler* handler) {
        m_data.erase(handler);
    }

private:
    EventHandlerDataRegistry() = default;
    std::map<EventHandler*, std::weak_ptr<EventHandlerData>> m_data;
};

}
}
