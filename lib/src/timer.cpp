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

#include "hope/timer.h"
#include "hope/event.h"
#include "hope/private/threaddata.h"
#include "hope/private/eventhandlerdata.h"

using namespace hope;

struct TimerEvent final : public Event {
public:
    TimerEvent(Timer* event);

    ~TimerEvent() override;

    Timer* m_timer = nullptr;
};

TimerEvent::TimerEvent(Timer *event)
    : m_timer(event)
{}

TimerEvent::~TimerEvent() = default;

Timer::Timer()
    : m_duration(std::chrono::milliseconds(0))
    , m_triggered(*this)
{}

Timer::~Timer() = default;

std::chrono::milliseconds Timer::duration() const {
    return m_duration;
}

void Timer::set_duration(std::chrono::milliseconds duration) {
    m_duration = duration;
}

Signal<>& Timer::triggered() {
    return m_triggered;
}

void Timer::start() {
    auto lock = detail::EventHandlerData::lock(m_data);
    ThreadDataRegistry::instance().thread_data(m_data->m_thread_id)->push_event(std::unique_ptr<Event>(new TimerEvent(this)), m_duration);
}

void Timer::on_event(Event *event) {
    if (auto timer_event = dynamic_cast<TimerEvent*>(event)) {
        if (timer_event->m_timer == this) {
            m_triggered.emit();
        }
    }

    Object::on_event(event);
}
