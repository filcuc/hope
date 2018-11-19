#include "hope/private/pollwrapper.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <iostream>

namespace hope {
namespace detail {

struct PollWrapperEvent {
    virtual ~PollWrapperEvent() = default;
};

struct RegisterObserverEvent : public PollWrapperEvent {
    RegisterObserverEvent(PollWrapper::ObserverData data)
        : data(std::move(data))
    {}

    PollWrapper::ObserverData data;
};

struct UnregisterObserverEvent : public PollWrapperEvent {
    UnregisterObserverEvent(PollWrapper::ObserverData data)
        : data(std::move(data))
    {}

    PollWrapper::ObserverData data;
};

struct QuitEvent : public PollWrapperEvent {

};

PollWrapper &PollWrapper::instance() {
    static PollWrapper result;
    return result;
}

void PollWrapper::register_observer(ObserverData data) {
    std::lock_guard<std::mutex> lock(m_mutex);
    PollWrapperEvent* ptr = new RegisterObserverEvent(std::move(data));
    ::write(m_fifo_fd, &ptr, sizeof(ptr));
}

void PollWrapper::unregister_observer(ObserverData data) {
    std::lock_guard<std::mutex> lock(m_mutex);
    PollWrapperEvent* ptr = new UnregisterObserverEvent(std::move(data));
    ::write(m_fifo_fd, &ptr, sizeof(ptr));
}

PollWrapper::PollWrapper()
    : m_thread([this]{ loop(); })
{}

PollWrapper::~PollWrapper() {
    if (m_thread.joinable()) {
        quit();
        m_thread.join();
    }
}

void PollWrapper::quit() {
    std::lock_guard<std::mutex> lock(m_mutex);
    PollWrapperEvent* ptr = new QuitEvent();
    ::write(m_fifo_fd, &ptr, sizeof(ptr));
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
            for (const pollfd& fd : m_descriptors) {
                if (fd.fd == m_fifo_fd) {
                    PollWrapperEvent* event;
                    auto read = ::read(m_fifo_fd, &event, sizeof(event));
                    if (read != sizeof(event)) {
                        std::cerr << "Failed to read an event" << std::endl;
                        std::terminate();
                    }
                    if (dynamic_cast<QuitEvent*>(event)) {
                        delete event;
                        return;
                    }
                    else if (auto register_event = dynamic_cast<RegisterObserverEvent*>(event)) {
                        m_observers.emplace_back(std::move(register_event->data));
                        m_descriptors.emplace_back({ register_event->data.file_descriptor,
                                                   });
                        delete event;
                    }
                    else if (auto unregister_event = dynamic_cast<UnregisterObserverEvent*>(event)) {
                        auto it = std::find(m_observers.begin(), m_observers.end(), unregister_event->data);
                        if (it != m_observers.end())
                            m_observers.erase(it);
                        delete event;
                    }
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
