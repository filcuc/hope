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

#include <hope/global.h>

#include <atomic>
#include <thread>

namespace hope {

class Event;
class Thread;

namespace detail { class ObjectData; }

class Object {
public:
    HOPE_API Object();

    HOPE_API virtual ~Object();

    HOPE_API void move_to_thread(Thread* thread);

    HOPE_API void move_to_thread(std::thread::id thread);

    HOPE_API virtual void on_event(Event* event);

protected:
    HOPE_API Object(bool initialize);

    HOPE_API void initialize();
    HOPE_API void terminate();

    std::atomic<bool> m_initialized;
    std::atomic<bool> m_terminated;
    std::shared_ptr<hope::detail::ObjectData> m_data;
};

}
