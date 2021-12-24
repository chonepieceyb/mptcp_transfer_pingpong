#ifndef MPTCP_TCP_SOCK_H
#define MPTCP_TCP_SOCK_H

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstdint>
#include <memory>
#include "errors.h"

namespace net {

//目前这个类不支持多线程, 不是线程安全
class TCPSocket {
public:
    explicit TCPSocket(size_t rb); 

    //in this case work as a wrapped 
    TCPSocket(int fd, size_t rb); 
    
    virtual ~TCPSocket();

    void reset(int fd);

    void tcp_close();

    int fd() const {return _sockfd;}

    void tcp_bind(const sockaddr_in &addr);
    
    void tcp_listen(int backlog = 1);

    std::pair<sockaddr_in, int> tcp_accept();

    void tcp_connect(const sockaddr_in &addr);

    std::size_t tcp_send(const std::string &data, int flags = 0);  

    std::string tcp_recv(int flags = 0);

    std::size_t tcp_simple_recv(int flags = 0) {
        // just recv don't make copy
        _check();
        return _recv(flags);
    }

protected:
    size_t _recv_buffer_size;
    int _sockfd;
    std::unique_ptr<char[]> _recv_buffer;
    bool _is_closed {false};
    
    void _check() {
        if (_is_closed) {
            throw Exception("sock has been closed, not allowed");
        }
    }

    void _set_fd(int fd) {
        _sockfd = fd; 
        if (_sockfd < 0) {
            //invalid fd
            _is_closed = true;
        } else {
            _is_closed = false;
        }
    }

    std::size_t _recv(int flags = 0);
};

}
#endif 
