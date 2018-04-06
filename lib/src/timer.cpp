#include "timer.h"
#include "event.h"

namespace hope {

struct TimerEvent final : public Event {
public:
    TimerEvent(Timer* event);

    ~TimerEvent();

    Timer* m_timer = nullptr;
};

TimerEvent::TimerEvent(Timer *event)
    : m_timer(event)
{}

TimerEvent::~TimerEvent() {

}

Timer::Timer() {
    if (m_current_event_loop)
        m_current_event_loop->register_event_handler(this);
}

Timer::~Timer() = default;

void Timer::set_duration(std::chrono::milliseconds duration) {
    m_duration = duration;
}

void Timer::start() {
    if (auto event_loop = m_current_event_loop) {
        event_loop->push_event(std::unique_ptr<Event>(new TimerEvent(this)), m_duration);
    }
}

void Timer::on_event(Event *event) {
    if (auto timer_event = dynamic_cast<TimerEvent*>(event)) {
        if (timer_event->m_timer == this) {
            if (m_handler)
                m_handler();
        }
    }
}

}
