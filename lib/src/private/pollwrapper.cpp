#include "hope/private/pollwrapper.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/eventfd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <poll.h>

#include <algorithm>
#include <iostream>
#include <cassert>

namespace {

constexpr const char* FIFO_NAME = "/tmp/hope-socket";

struct RegisterObserverEvent : public hope::detail::PollWrapper::PollWrapperEvent {
    RegisterObserverEvent(hope::detail::PollWrapper::ObserverData data)
        : data(std::move(data))
    {}

    hope::detail::PollWrapper::ObserverData data;
};

struct UnregisterObserverEvent : public hope::detail::PollWrapper::PollWrapperEvent {
    UnregisterObserverEvent(hope::detail::PollWrapper::ObserverData data)
        : data(std::move(data))
    {}

    hope::detail::PollWrapper::ObserverData data;
};

struct QuitEvent : public hope::detail::PollWrapper::PollWrapperEvent {

};

}

namespace hope {
namespace detail {

PollWrapper &PollWrapper::instance() {
    static PollWrapper result;
    return result;
}

PollWrapper::PollWrapper()
    : m_event_fd(eventfd(0,0)) {
    m_thread = std::thread([this]{loop();});
}

PollWrapper::~PollWrapper() {
    if (m_thread.joinable()) {
        quit();
        m_thread.join();
    }
}

void PollWrapper::quit() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.emplace_back(new QuitEvent());
    eventfd_write(m_event_fd.fd(), 1);
}

void PollWrapper::register_observer(ObserverData data) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.emplace_back(new RegisterObserverEvent(std::move(data)));
    eventfd_write(m_event_fd.fd(), 1);
}

void PollWrapper::unregister_observer(ObserverData data) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.emplace_back(new UnregisterObserverEvent(std::move(data)));
    eventfd_write(m_event_fd.fd(), 1);
}

void PollWrapper::loop() {
    std::vector<struct pollfd> poll_list {{m_event_fd.fd(), POLLIN | POLLPRI, 0}};

    while (true) {
        int num_changed = ::poll(poll_list.data(), poll_list.size(), -1);
        if (num_changed <= 0)
            continue;

        for (const pollfd& fd : poll_list) {
            const bool readable = (fd.revents & POLLIN) == POLLIN || (fd.revents & POLLPRI) == POLLPRI;
            const bool writable = (fd.revents & POLLOUT) == POLLOUT;

            if (readable && fd.fd == m_event_fd.fd()) {
                eventfd_t value = 0 ;
                eventfd_read(m_event_fd.fd(), &value);

                decltype (m_events) events;
                m_mutex.lock();
                std::swap(events, m_events);
                m_mutex.unlock();

                for (const std::unique_ptr<PollWrapperEvent>& ev : events) {
                    if (auto quit = dynamic_cast<const QuitEvent*>(ev.get())) {
                        return;
                    }
                    else if (auto reg = dynamic_cast<const RegisterObserverEvent*>(ev.get())) {
                        pollfd fd {};
                        fd.fd = reg->data.file_descriptor;
                        if (reg->data.event_type == PollWrapper::ReadyRead)
                            fd.events = POLLIN | POLLPRI;
                        else if (reg->data.event_type == PollWrapper::ReadyWrite)
                            fd.events = POLLOUT;
                        else
                            continue;
                        poll_list.push_back(fd);
                        m_observers.push_back(reg->data);
                    }
                    else if (auto unreg = dynamic_cast<const UnregisterObserverEvent*>(ev.get())) {
                        {
                            auto it = std::find_if(poll_list.begin(), poll_list.end(), [&](const pollfd& fd){
                                return fd.fd == unreg->data.file_descriptor;
                            });
                            if (it != poll_list.end())
                                poll_list.erase(it);
                        }
                        {
                            auto it = std::find_if(m_observers.begin(), m_observers.end(), [&](const PollWrapper::ObserverData& data){
                                return data.file_descriptor == unreg->data.file_descriptor;
                            });
                            if (it != m_observers.end())
                                m_observers.erase(it);
                        }
                    }
                }
            } else if (readable) {
                auto observer = std::find_if(m_observers.begin(), m_observers.end(), [&](const PollWrapper::ObserverData& data) {
                    return data.file_descriptor == fd.fd;
                });
                if (observer != m_observers.end()) {
                }

            } else if (writable) {
                auto observer = std::find_if(m_observers.begin(), m_observers.end(), [&](const PollWrapper::ObserverData& data) {
                    return data.file_descriptor == fd.fd;
                });
                if (observer != m_observers.end()) {
                }
            }
        }
    }
}

}
}
