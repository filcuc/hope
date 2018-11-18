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

#include "hope/object.h"
#include "hope/signal.h"
#include "hope/private/filedescriptor.h"

namespace hope {
namespace detail {

class FileDescriptorObserver : public hope::Object {
public:
    enum EventType { ReadyRead, ReadyWrite };

    FileDescriptorObserver(FileDescriptor& descriptor, EventType type);

    ~FileDescriptorObserver();

    bool enabled() const;

    EventType event_type() const;

    void setEnabled(bool enabled);

    hope::Signal<>& activated();

private:
    FileDescriptor& m_file_descriptor;
    EventType m_event_type = ReadyRead;
    bool m_enabled = false;
    hope::Signal<> m_activated_signal;
};

}
}
