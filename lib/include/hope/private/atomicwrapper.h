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

#include <atomic>
#include <utility>

namespace hope {
namespace detail {

template<typename T>
class AtomicWrapper {
public:
    AtomicWrapper() = default;

    AtomicWrapper(T&& t)
     : m_atomic(std::move(t))
    {}

    AtomicWrapper(const AtomicWrapper& other)
     : m_atomic(other.m_atomic.load())
    {}

    AtomicWrapper(AtomicWrapper&& other) noexcept = delete;

    AtomicWrapper& operator=(const AtomicWrapper& other) {
        if (&other != this)
            *this = AtomicWrapper(other);
        return *this;
    }

    AtomicWrapper& operator=(AtomicWrapper&& other) noexcept = delete;

    const std::atomic<T>& value() const {
        return m_atomic;
    }

    std::atomic<T>& value() {
        return m_atomic;
    }

private:
    std::atomic<T> m_atomic;
};

} // namespace detail
} // namespace hope
