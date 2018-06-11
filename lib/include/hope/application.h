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

#include <hope/object.h>
#include <hope/eventloop.h>
#include <hope/global.h>

namespace hope {

namespace detail { class ObjectData; }
namespace test { class ApplicationTestHelper; }

class Application : public Object {
public:
	HOPE_API Application();
	HOPE_API ~Application() override;

	HOPE_API void quit(int exit_code);
	HOPE_API void quit() { quit(0); }

	HOPE_API int exec();

private:
    friend class test::ApplicationTestHelper;

    EventLoop m_event_loop;
};

}
