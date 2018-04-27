#pragma once

#include <mutex>
#include <thread>
#include <unordered_map>

namespace hope {

class EventLoop;

class ThreadData {
public:
    ThreadData() = default;

    ThreadData(std::thread::id id, EventLoop* event_loop)
        : m_id(id)
        , m_event_loop(event_loop)
    {}

    std::thread::id thread_id() const {
        return m_id;
    }

    EventLoop* event_loop() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_event_loop;
    }

    void set_event_loop(EventLoop* event_loop) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_event_loop = event_loop;
    }

private:
    mutable std::mutex m_mutex;
    const std::thread::id m_id;
    EventLoop* m_event_loop = nullptr;
};

class ThreadDataRegistry {
public:
    static ThreadDataRegistry& get_instance() {
        static ThreadDataRegistry instance;
        return instance;
    }

    std::shared_ptr<ThreadData> thread_data(const std::thread::id& id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_registry.find(id);
        if (it == m_registry.end()) {
            it = m_registry.emplace(id, std::make_shared<ThreadData>(id, nullptr)).first;
        }
        return it->second;
    }

    std::shared_ptr<ThreadData> current_thread_data() {
        return thread_data(std::this_thread::get_id());
    }

private:
    ThreadDataRegistry() = default;

    mutable std::mutex m_mutex;
    mutable std::unordered_map<std::thread::id, std::shared_ptr<ThreadData>> m_registry;
};

}
