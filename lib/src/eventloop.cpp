#include "eventloop.h"

thread_local EventLoop* m_current_event_loop = nullptr;

EventLoop::EventLoop() {
    m_current_event_loop = this;
}

EventLoop::~EventLoop() {
    m_current_event_loop = nullptr;
}
