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

#include "hope/private/filedescriptor.h"
#include "hope/private/objectdata.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace hope {
namespace detail {

class PollWrapper {
public:
    enum EventType { ReadyRead, ReadyWrite };

    struct ObserverData {
        std::weak_ptr<hope::detail::ObjectData> object;
        hope::detail::ObjectData* object_ptr;
        int file_descriptor;
        EventType event_type;
    };

    static PollWrapper& instance();

    void register_observer(ObserverData data);

    void unregister_observer(ObserverData data);

    struct PollWrapperEvent {
        virtual ~PollWrapperEvent() = default;
    };

private:
    PollWrapper();

    ~PollWrapper();

    void loop();

    void quit();

    std::mutex m_mutex;
    std::vector<std::unique_ptr<PollWrapperEvent>> m_events;
    std::thread m_thread;
    FileDescriptor m_event_fd;
    std::vector<hope::detail::PollWrapper::ObserverData> m_observers;
};

inline bool operator==(const PollWrapper::ObserverData& lhs, const PollWrapper::ObserverData& rhs) {
    return lhs.event_type == rhs.event_type
            && lhs.file_descriptor == rhs.file_descriptor
            && lhs.object_ptr == rhs.object_ptr;
}

}
}
