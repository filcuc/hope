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

namespace detail { class EventHandlerData; }
namespace test { class ApplicationTestHelper; }

class Application : public EventHandler {
public:
    Application();
    ~Application() override;

    void quit(int exit_code);
    void quit() { quit(0); }

    int exec();

    void on_event(Event *event) final;

private:
    friend class test::ApplicationTestHelper;

    std::shared_ptr<detail::EventHandlerData> m_data;
    EventLoop m_event_loop;
};

}
