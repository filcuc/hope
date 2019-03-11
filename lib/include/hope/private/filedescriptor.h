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

#include <algorithm>
#include <unistd.h>

namespace hope {
namespace detail {

struct FileDescriptor {
public:
    FileDescriptor(int fd = -1)
        : m_fd(fd)
    {}

    FileDescriptor(FileDescriptor&& other )
        : m_fd(-1) {
        std::swap(m_fd, other.m_fd);
    }

    ~FileDescriptor() {
        reset();
    }

    FileDescriptor& operator=(FileDescriptor&& other) {
        std::swap(m_fd, other.m_fd);
        return *this;
    }

    FileDescriptor& operator=(const FileDescriptor&) = delete;

    FileDescriptor(const FileDescriptor&) = delete;

    void reset(int fd = -1) {
        if (m_fd != -1) {
            ::close(m_fd);
            m_fd = -1;
        }
        m_fd = fd;
    }

    bool valid() const {
        return m_fd != -1;
    }

    int fd() const {
        return m_fd;
    }

private:
    int m_fd = -1;
};

static bool operator==(int other, const FileDescriptor& descriptor) {
    return other == descriptor.fd();
}

static bool operator!=(int other, const FileDescriptor& descriptor) {
    return other != descriptor.fd();
}

}
}
