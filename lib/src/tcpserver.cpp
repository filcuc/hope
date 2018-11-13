#include "hope/tcpserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <poll.h>

#include <cstdio>
#include <functional>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>


namespace {

struct FileDescriptor {
public:
    FileDescriptor(int fd = -1)
        : m_fd(fd)
    {}

    ~FileDescriptor() {
        reset();
    }

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

    operator int() const {
        return m_fd;
    }

    operator bool() const {
        return valid();
    }

    bool operator==(int other) const {
        return m_fd == other;
    }

    bool operator!=(int other) const {
        return m_fd != other;
    }

    bool operator==(const FileDescriptor& other) const {
        return m_fd == other.m_fd;
    }

    bool operator!=(const FileDescriptor& other) const {
        return m_fd != other.m_fd;
    }

private:
    int m_fd = -1;
};

class PollWrapper {
public:
    static PollWrapper& instance() {
        static PollWrapper result;
        return result;
    }

private:
    PollWrapper()
        : m_thread([this]{ loop(); })
    {}

    ~PollWrapper() {
        if (m_thread.joinable()) {
            uint8_t data = 1;
            ::write(m_fifo_fd, &data, sizeof(uint8_t));
            m_thread.join();
        }
    }

    void loop() {
        auto fifo_path = "/tmp/hope/poll";
        ::mkfifo(fifo_path, 0666);

        m_fifo_fd = ::open(fifo_path, O_RDWR);
        if (!m_fifo_fd) {
            std::cerr << "Failed to open poll fifo" << std::endl;
            std::terminate();
        }

        m_descriptors.push_back({m_fifo_fd, POLLIN, 0});

        for (;;) {
            const int poll_num = poll(m_descriptors.data(), m_descriptors.size(), -1);
            if (poll_num > 0) {
                for (const pollfd& fd : m_descriptors) {
                    if (fd.fd == m_fifo_fd) {
                        return;
                    }

                    if (fd.revents & POLLIN || fd.revents & POLLPRI) {
                        if (m_ready_read_callback)
                            m_ready_read_callback(fd.fd);
                    }

                    if (fd.revents & POLLOUT) {
                        if (m_ready_write_callback) {
                            m_ready_write_callback(fd.fd);
                        }
                    }
                }
            }
        }
    }

    FileDescriptor m_fifo_fd;
    std::vector<pollfd> m_descriptors;
    std::function<void(int)> m_ready_read_callback;
    std::function<void(int)> m_ready_write_callback;
    std::thread m_thread;
};

}


namespace hope {

class TcpServer::TcpServerImpl {
public:
    uint16_t bind_port;
};

TcpServer::TcpServer()
    : m_impl(new TcpServer::TcpServerImpl())
{}

TcpServer::~TcpServer()
{
    delete m_impl;
}

bool TcpServer::listen(uint16_t bind_port)
{
    FileDescriptor socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!socket_fd) {
        return false;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(bind_port);

    if (::bind(socket_fd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
        return false;
    }

    if (::listen(socket_fd, 5) < 0) {
        return false;
    }

    return false;
}

void TcpServer::close()
{

}

}
