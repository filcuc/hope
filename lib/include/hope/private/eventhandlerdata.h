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
#include <thread>

namespace hope {

class EventHandler;

namespace detail {

class EventHandlerData {
public:
    std::thread::id m_thread_id;
};

class EventHandlerDataRegistry {
public:
    EventHandlerDataRegistry& instance();

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
    std::map<EventHandler*, std::weak_ptr<EventHandlerData>> m_data;
};

}
}
