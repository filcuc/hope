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

#include <hope/global.h>

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
