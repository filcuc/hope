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

    operator T() {
        return m_data;
    }

    operator bool() {
        return m_ok;
    }

private:
    bool m_ok = false;
    T m_data;
};

}
}
