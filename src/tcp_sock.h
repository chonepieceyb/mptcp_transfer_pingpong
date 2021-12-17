#ifndef MPTCP_TCP_SOCK_H
#define MPTCP_TCP_SOCK_H

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <cstdint>
#include <memory>

namespace net {

class TCPSocket {
public:
    explicit TCPSocket(size_t rb = 2048); 
    
    virtual ~TCPSocket();

    void my_close();

    int fd() const {return _sockfd;}

    void my_bind(const sockaddr_in &addr);
    
    void my_listen(int backlog = 1);

    std::pair<sockaddr_in, int> my_accept();

    void my_connect(const sockaddr_in &addr);

    ssize_t my_send(const std::string &data, int flags = 0);  

    std::string my_recv(int fd, int flags = 0);

    ssize_t simple_recv(int fd, int flags = 0) {
        // just recv don't make copy
        return _recv(fd, flags);
    }

protected:
    size_t _recv_buffer_size;
    int _sockfd;
    std::unique_ptr<char[]> _recv_buffer;
    bool _is_closed {false};

    ssize_t _recv(int fd, int flags = 0);
};

}
#endif 
