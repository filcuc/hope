#include "hope/timer.h"
#include "hope/event.h"
#include "hope/private/threaddata.h"

namespace hope {

struct TimerEvent final : public Event {
public:
    TimerEvent(Timer* event);

    ~TimerEvent() override;

    Timer* m_timer = nullptr;
};

TimerEvent::TimerEvent(Timer *event)
    : m_timer(event)
{}

TimerEvent::~TimerEvent() {
}

Timer::Timer() = default;

Timer::~Timer() = default;

std::chrono::milliseconds Timer::duration() const {
    return m_duration;
}

void Timer::set_duration(std::chrono::milliseconds duration) {
    m_duration = duration;
}

Signal<void> &Timer::triggered() {
    return m_triggered;
}

void Timer::start() {
    ThreadDataRegistry::get_instance().thread_data(thread_id())->push_event(std::unique_ptr<Event>(new TimerEvent(this)), m_duration);
}

void Timer::on_event(Event *event) {
    if (auto timer_event = dynamic_cast<TimerEvent*>(event)) {
        if (timer_event->m_timer == this) {
            m_triggered.emit();
        }
    }

    Object::on_event(event);
}

}
