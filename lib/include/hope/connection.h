#pragma once

#include <cstdint>

namespace hope {

class Connection {
public:
    Connection(std::int64_t id)
        : m_valid(true)
        , m_id(id)
    {}

    std::int64_t id() const {
        return m_id;
    }

    bool valid() const {
        return m_valid;
    }

    bool operator==(const Connection& other) const {
        return m_id == other.m_id;
    }

    bool operator<(const Connection& other) const {
        return m_id < other.m_id;
    }

private:
    const bool m_valid = false;
    const std::int64_t m_id;
};


}
