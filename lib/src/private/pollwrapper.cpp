#include "hope/private/pollwrapper.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

namespace hope {
namespace detail {

struct PollWrapperEvent {
    virtual ~PollWrapperEvent() = default;
};

struct RegisterObserverEvent : public PollWrapperEvent {
    std::weak_ptr<hope::detail::ObjectData> object;
    PollWrapper::EventType type;
};

struct UnregisterObserverEvent : public PollWrapperEvent {
    std::weak_ptr<hope::detail::ObjectData> object;
};

struct QuitEvent : public PollWrapperEvent {

};

PollWrapper &PollWrapper::instance() {
    static PollWrapper result;
    return result;
}

void PollWrapper::register_observer(const std::shared_ptr<hope::detail::ObjectData> &object, EventType type) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto ptr = new RegisterObserverEvent();
    ptr->object = object;
    ptr->type = type;
    ::write(m_fifo_fd, &ptr, sizeof(ptr));
}

void PollWrapper::unregister_observer(const std::shared_ptr<hope::detail::ObjectData> &object)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto ptr = new UnregisterObserverEvent();
    ptr->object = object;
    ::write(m_fifo_fd, &ptr, sizeof(ptr));

    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_observers.begin(); it != m_observers.end(); ) {
        if (auto ptr = it->first.lock()) {
            if (ptr == object) {
                it = m_observers.erase(it);
            } else {
                ++it;
            }
        } else {
            it = m_observers.erase(it);
        }
    }
}

PollWrapper::PollWrapper()
    : m_thread([this]{ loop(); })
{}

PollWrapper::~PollWrapper() {
    if (m_thread.joinable()) {
        uint8_t data = 1;
        ::write(m_fifo_fd, &data, sizeof(uint8_t));
        m_thread.join();
    }
}

void PollWrapper::loop() {
    auto fifo_path = "/tmp/hope/poll";
    ::mkfifo(fifo_path, 0666);

    m_fifo_fd = ::open(fifo_path, O_RDWR);
    if (!m_fifo_fd) {
        std::cerr << "Failed to open poll fifo" << std::endl;
        std::terminate();
    }

    m_descriptors.push_back({m_fifo_fd, POLLIN, 0});

    for (;;) {
        const int poll_num = poll(m_descriptors.data(), m_descriptors.size(), -1);
        if (poll_num > 0) {
            decltype (m_observers) observers;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                observers = m_observers;
            }

            for (const pollfd& fd : m_descriptors) {
                if (fd.fd == m_fifo_fd) {
                    return;
                }

                if (fd.revents & POLLIN || fd.revents & POLLPRI) {
                }

                if (fd.revents & POLLOUT) {
                }
            }
        }
    }
}

}
}
