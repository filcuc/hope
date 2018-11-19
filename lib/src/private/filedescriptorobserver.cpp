#include "hope/private/filedescriptorobserver.h"

#include "hope/private/pollwrapper.h"

namespace hope {
namespace detail {

FileDescriptorObserver::FileDescriptorObserver(FileDescriptor &descriptor, FileDescriptorObserver::EventType type)
    : m_file_descriptor(descriptor)
    , m_event_type(type) {
    PollWrapper::instance().register_observer({m_data, m_data.get(), m_file_descriptor,
                                               m_event_type == ReadyRead ? PollWrapper::ReadyRead
                                                                         : PollWrapper::ReadyWrite });
}

FileDescriptorObserver::~FileDescriptorObserver()
{
    setEnabled(false);
    PollWrapper::instance().unregister_observer({m_data, m_data.get(), m_file_descriptor,
                                                 m_event_type == ReadyRead ? PollWrapper::ReadyRead
                                                                           : PollWrapper::ReadyWrite });
}

bool FileDescriptorObserver::enabled() const {
    return m_enabled;
}

FileDescriptorObserver::EventType FileDescriptorObserver::event_type() const {
    return m_event_type;
}

void FileDescriptorObserver::setEnabled(bool enabled) {
    m_enabled = enabled;
}

hope::Signal<> &FileDescriptorObserver::activated() {
    return m_activated_signal;
}

}
}
