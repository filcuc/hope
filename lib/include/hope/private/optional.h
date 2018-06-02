/*
    Copyright (C) 2018 Filippo Cucchetto.
    Contact: https://github.com/filcuc/hope

    This file is part of the Hope library.

    The Hope library is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License.

    The Hope library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the Hope library.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

namespace hope {
namespace detail {

/// Naive optional class until we decide to c++14 standard
/// This works only for `T` that are default constructable and copiable
template<class T>
class Optional {
public:
    Optional() = default;

    Optional(T&& data)
        : m_ok(true)
        , m_data(std::move(data))
    {}

    Optional(const T& data)
        : m_ok(true)
        , m_data(data)
    {}

    template<typename ...Args>
    Optional(Args&&... args)
        : m_ok(true)
        , m_data(std::forward<Args...>(args...))
    {}

    Optional(const Optional&) = default;
    Optional(Optional&&) noexcept = default;
    Optional& operator=(const Optional&) = default;
    Optional& operator=(Optional&&) noexcept = default;

    Optional& operator=(const T& data) {
        m_ok = true;
        m_data = data;
        return *this;
    }

    Optional& operator=(T&& data) {
        m_ok = true;
        m_data = std::move(data);
        return *this;
    }

    void reset() {
        m_ok = false;
    }

    T& data() {
        return m_data;
    }

    const T& data() const {
        return m_data;
    }

    bool is_some() const {
        return m_ok;
    }

    bool is_none() const {
        return !m_ok;
    }

    operator T() {
        return m_data;
    }

    operator bool() {
        return m_ok;
    }

    T* operator->() {
        return m_ok ? &m_data : nullptr;
    }

    const T* operator->() const {
        return m_ok ? &m_data : nullptr;
    }

    bool operator==(const Optional& other) const {
        return m_ok == other.m_ok && m_data == other.m_data;
    }

    bool operator!=(const Optional& other) const {
        return !operator==(other);
    }

private:
    bool m_ok = false;
    T m_data;
};

}
}
