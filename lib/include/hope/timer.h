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
#include <hope/object.h>
#include <hope/signal.h>

#include <chrono>

namespace hope {

class Timer final : public Object {
public:
    HOPE_API Timer();

    HOPE_API ~Timer() override;

    HOPE_API std::chrono::milliseconds duration() const;

    HOPE_API void set_duration(std::chrono::milliseconds duration);

    HOPE_API Signal<>& triggered();

    HOPE_API void start();

protected:
    void on_event(Event* event) final;

private:
    std::chrono::milliseconds m_duration;
    Signal<> m_triggered;
};

}
