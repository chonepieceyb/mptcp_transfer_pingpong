#include "tcp_sock.h"

#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

namespace net {

TCPSocket::TCPSocket(size_t rb) : _recv_buffer_size(rb) {
    int fd  = socket(AF_INET,SOCK_STREAM,0);
    if (fd == -1) {
        throw LinuxError(errno);
    }
    _set_fd(fd);
    _recv_buffer = std::unique_ptr<char[]>(new char[_recv_buffer_size]);
}

TCPSocket::TCPSocket(int fd, size_t rb) : _recv_buffer_size(rb) {
    _set_fd(fd);
    _recv_buffer = std::unique_ptr<char[]>(new char[_recv_buffer_size]);
}

TCPSocket::~TCPSocket() {
    try {
        tcp_close();
    } catch (std::exception &e) {
        std::cerr << "failed to destory socket: " << e.what() << '\n';
    }
}

void TCPSocket::reset(int fd) {
    tcp_close();
    _set_fd(fd);
}

void TCPSocket::tcp_close() {
    if (_is_closed) {
        return;
    }
    int res = close(_sockfd);
    if (res == -1) {
        throw LinuxError(errno);
    }
    _is_closed = true;
}

void TCPSocket::tcp_bind(const sockaddr_in &addr) {
    _check();
    const sockaddr *saddr = (const sockaddr*)(&addr);
    int res = bind(_sockfd, saddr, sizeof(sockaddr_in));
    if (res == -1) {
        throw LinuxError(errno);
    }
}

void TCPSocket::tcp_listen(int backlog) {
    _check();
    if (listen(_sockfd, backlog) == -1) {
        throw LinuxError(errno);
    }
}

std::pair<sockaddr_in, int> TCPSocket::tcp_accept() {
    _check();
    sockaddr_in accepted_addr;
    socklen_t len = sizeof(sockaddr_in);
    int fd = accept(_sockfd, (sockaddr*)(&accepted_addr), &len);
    if (fd == -1) {
        throw LinuxError(errno);
    }
    if (len != sizeof(accepted_addr)) {
        throw Exception("listen len != sizeof(acepted_addr)");
    }
    return std::make_pair(accepted_addr, fd);
}

void TCPSocket::tcp_connect(const sockaddr_in &addr) {
    _check();
    const sockaddr *caddr = (const sockaddr*)(&addr);
    int res = connect(_sockfd, caddr, sizeof(sockaddr_in));
    if (res == -1) {
        throw LinuxError(errno);
    }
}

std::size_t TCPSocket::tcp_send(const std::string &data, int flags) {
    _check();
    ssize_t send_size = send(_sockfd, data.data(), data.length(), flags);
    if (send_size == -1) {
        throw LinuxError(errno);
    }
    return static_cast<std::size_t>(send_size);
}

std::string TCPSocket::tcp_recv(int flags) {
    _check();
    auto recvd = _recv(flags);
    return std::string(_recv_buffer.get(), recvd);
}

std::size_t TCPSocket::_recv(int flags) {
    auto recv_size = recv(_sockfd, 
            static_cast<void*>(_recv_buffer.get()), 
            _recv_buffer_size, flags);
    if (recv_size == -1) {
        throw LinuxError(errno);
    }
    return static_cast<std::size_t>(recv_size);
}

}
