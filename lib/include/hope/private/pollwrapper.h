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

#include <poll.h>

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace hope {
namespace detail {

class PollWrapper {
public:
    enum EventType { ReadyRead, ReadyWrite };

    static PollWrapper& instance();

    void register_observer(const std::shared_ptr<hope::detail::ObjectData>& object, EventType type);
    void unregister_observer(const std::shared_ptr<hope::detail::ObjectData>& object);

private:
    PollWrapper();

    ~PollWrapper();

    void loop();

    std::mutex m_mutex;
    FileDescriptor m_fifo_fd;
    std::vector<pollfd> m_descriptors;
    std::thread m_thread;
    std::vector<std::pair<std::weak_ptr<hope::detail::ObjectData>, EventType>> m_observers;
};

}
}
