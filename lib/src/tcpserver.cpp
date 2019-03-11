#include "hope/tcpserver.h"

#include "hope/signal.h"
#include "hope/private/filedescriptor.h"
#include "hope/private/filedescriptorobserver.h"
#include "hope/private/objectdata.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <functional>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <memory>

namespace hope {

class TcpServer::TcpServerImpl {
public:
    uint16_t bind_port;
    std::unique_ptr<detail::FileDescriptorObserver> observer;
    detail::FileDescriptor socket_fd;
};

TcpServer::TcpServer()
    : m_impl(new TcpServer::TcpServerImpl())
{}

TcpServer::~TcpServer() {
    delete m_impl;
}

bool TcpServer::listen(uint16_t bind_port) {
    m_impl->socket_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (!m_impl->socket_fd.valid()) {
        return false;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(bind_port);

    if (::bind(m_impl->socket_fd.fd(), reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
        return false;
    }

    if (::listen(m_impl->socket_fd.fd(), 5) < 0) {
        return false;
    }

    m_impl->observer.reset(new detail::FileDescriptorObserver(m_impl->socket_fd, detail::FileDescriptorObserver::ReadyRead));
    m_impl->observer->activated().connect(this, &TcpServer::on_client_connected);
    m_impl->observer->setEnabled(true);

    return true;
}

void TcpServer::close()
{

}

void TcpServer::on_client_connected()
{
    std::cout << "A client connected" << std::endl;
}

}
