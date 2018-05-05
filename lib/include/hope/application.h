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

#include "hope/eventhandler.h"
#include "hope/eventloop.h"

namespace hope {

class Application : public EventHandler {
public:
    Application();

    void quit(int exit_code);
    void quit() { quit(0); }

    int exec();

    void on_event(Event *event) final;

    std::thread::id thread_id() const final;

private:
    std::thread::id m_thread_id;
    EventLoop m_event_loop;
};

}
