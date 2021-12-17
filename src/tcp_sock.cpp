#include "tcp_sock.h"
#include "errors.h"
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

namespace net {

TCPSocket::TCPSocket(size_t rb) : _recv_buffer_size(rb) {
    _sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (_sockfd == -1) {
        throw LinuxError(errno);
    }
    _recv_buffer = std::unique_ptr<char[]>(new char[_recv_buffer_size]);
}

TCPSocket::~TCPSocket() {
    my_close();
}

void TCPSocket::my_close() {
    if (_is_closed) return;
    int res = close(_sockfd);
    if (res == -1) {
        LinuxError e(errno);
        std::cerr << "failed to destory socket: " << e.what() << '\n';
    }
    _is_closed = true;
}

void TCPSocket::my_bind(const sockaddr_in &addr) {
    const sockaddr *saddr = (const sockaddr*)(&addr);
    int res = bind(_sockfd, saddr, sizeof(sockaddr_in));
    if (res == -1) {
        throw LinuxError(errno);
    }
}

void TCPSocket::my_listen(int backlog) {
    if (listen(_sockfd, backlog) == -1) {
        throw LinuxError(errno);
    }
}

std::pair<sockaddr_in, int> TCPSocket::my_accept() {
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

void TCPSocket::my_connect(const sockaddr_in &addr) {
    const sockaddr *caddr = (const sockaddr*)(&addr);
    int res = connect(_sockfd, caddr, sizeof(sockaddr_in));
    if (res == -1) {
        throw LinuxError(errno);
    }
}

ssize_t TCPSocket::my_send(const std::string &data, int flags) {
    ssize_t send_size = send(_sockfd, data.data(), data.length(), flags);
    if (send_size == -1) {
        throw LinuxError(errno);
    }
    return send_size;
}

std::string TCPSocket::my_recv(int fd, int flags) {
    auto recvd = _recv(fd, flags);
    return std::string(_recv_buffer.get(), recvd);
}

ssize_t TCPSocket::_recv(int fd, int flags) {
    auto recv_size = recv(fd, 
            static_cast<void*>(_recv_buffer.get()), 
            _recv_buffer_size, flags);
    if (recv_size == -1) {
        throw LinuxError(errno);
    }
    return recv_size;
}

}
