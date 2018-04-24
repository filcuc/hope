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
        : m_valid(true)
        , m_id(id)
        , m_event_loop(event_loop)
    {}

    bool is_valid() const {
        return m_valid;
    }

    std::thread::id thread_id() const {
        return m_id;
    }

    EventLoop* event_loop() const {
        return m_event_loop;
    }

private:
    bool m_valid = false;
    std::thread::id m_id;
    EventLoop* m_event_loop = nullptr;
};

class ThreadDataRegistry {
public:
    static ThreadDataRegistry& get_instance() {
        static ThreadDataRegistry instance;
        return instance;
    }

    ThreadData thread_data(const std::thread::id& id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_registry.find(id);
        return it != m_registry.end() ? it->second : ThreadData();
    }

    void set_thread_data(ThreadData data) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_registry.emplace(data.thread_id(), data);
    }

private:
    ThreadDataRegistry() = default;

    std::mutex m_mutex;
    std::unordered_map<std::thread::id, ThreadData> m_registry;
};

}
