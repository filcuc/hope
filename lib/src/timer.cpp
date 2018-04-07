#include "timer.h"
#include "event.h"

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

Timer::Timer()
    : m_thread_id(std::this_thread::get_id())
{
    if (m_current_event_loop)
        m_current_event_loop->register_event_handler(this);
}

Timer::~Timer() {
    if (std::this_thread::get_id() != m_thread_id) {
        std::cerr << "Destroying timer from different thread" << std::endl;
    }
}

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
    if (auto event_loop = m_current_event_loop) {
        event_loop->push_event(std::unique_ptr<Event>(new TimerEvent(this)), m_duration);
    } else {
        std::cerr << "Timer started without an event loop" << std::endl;
    }
}

std::thread::id Timer::thread_id() const
{
    return m_thread_id;
}

void Timer::on_event(Event *event) {
    if (auto timer_event = dynamic_cast<TimerEvent*>(event)) {
        if (timer_event->m_timer == this) {
            m_triggered.emit();
        }
    }
}

}
