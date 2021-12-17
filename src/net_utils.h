#ifndef UTILS_NET_UTILS_H
#define UTILS_NET_UTILS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdint>
#include <csignal> 
#include <string>

namespace net {

namespace netutils {

sockaddr_in parse_sockaddr_in(const std::string& addr, uint16_t port);

sockaddr_in parse_sockaddr_in(uint16_t port);

void set_mptcp_enable(int fd, int enable);

void set_tcp_reuse_addr(int fd, int reuse);

class TermSignal {
public:
    TermSignal() {
        _keeprunning = true;
    }
    
    bool ok() {
        return _keeprunning;
    }
private:
    void _sig_handler(int sig) {
        if (sig == SIGINT) {
            _keeprunning = false;
        }
    }
    volatile bool _keeprunning;
};

}

}
#endif 
