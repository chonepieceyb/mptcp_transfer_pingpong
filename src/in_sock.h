#ifndef CHONEPIECEYB_IN_SOCK_H
#define CHONEPIECEYB_IN_SOCK_H

#include <string>
#include <cstdint>
#include <memory>
#include <ostream>

#include "sock.h"
#include "errors.h"
#include "utils.h"

extern "C" {
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
}

namespace sock {

template<>
class SockAddr<sockaddr_in> : public sockaddr_in {
public:        
    using domain_type = sockaddr_in; 
    const static int domain_value = AF_INET;

    SockAddr() {
        std::memset(this, 0, sizeof(sockaddr_in));
        sin_family = domain_value;
    }

    SockAddr(const std::string& addr, std::uint16_t port) {
        sin_family = domain_value;
        sin_port = htons(port);
        
        if (addr.empty()) {
            sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            if (0 ==  inet_aton(addr.data(), &sin_addr)) {
                throw std::runtime_error(std::string("create sockaddr_in failed, invalid addr ") + addr);
            }
        }
    }

    SockAddr(std::uint16_t port) {
        sin_family = domain_value;
        sin_port = htons(port);
        sin_addr.s_addr = htonl(INADDR_ANY);
    }

    SockAddr &operator=(const SockAddr &other) {
        sin_port = other.sin_port;
        std::memcpy(&sin_addr, &other.sin_addr, sizeof(sin_addr));
        return *this;
    }
    
    friend std::ostream& operator<<(std::ostream &out, const SockAddr &sock) {
        out << inet_ntoa(sock.sin_addr) << ':' << ntohs(sock.sin_port) ;
        return out;
    }

};

using SockAddrIn = SockAddr<sockaddr_in>;

//IPV4 socket 

class SockIn : public Socket<SockAddrIn> {
public:
    using Socket<SockAddrIn>::Socket;
};

//tcp socket based on ipv4
class TCPSockIn : public SockIn {
public:
    explicit TCPSockIn(int protocol = 0) : SockIn(SOCK_STREAM, protocol) {}

    void set_reuse_addr(int reuse) {
        this->setsockopt(SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    }
};

//mptcp v1 based on ipv4
//#define IPPROTO_MPTCP=262 /include/uapi/linux/in.h
#ifndef IPPROTO_MPTCP 
#define IPPROTO_MPTCP 262
#endif 
class MPTCPSockInV1 : public TCPSockIn {
public:
    const static int protocol = IPPROTO_MPTCP;
    
    MPTCPSockInV1() : TCPSockIn(protocol) {};
};

class MPTCPSockInV0 : public TCPSockIn {
public:
    const static int MPTCP_ENABLED = 42;
    const static int SOL_TCP = 6;

    static void set_mptcp_enable(const view_type *sock, int enable) {
        if (sock == nullptr) {
            throw std::runtime_error("set mptcp enable sock is nullptr");
        }
        sock->setsockopt(SOL_TCP, MPTCP_ENABLED, &enable, sizeof(enable));
    }

    MPTCPSockInV0() : TCPSockIn(0) {
        set_mptcp_enable(1);
    };

    void set_mptcp_enable(int enable) {
        set_mptcp_enable(dynamic_cast<const view_type*>(this), enable);
    }
};

}
#endif 
