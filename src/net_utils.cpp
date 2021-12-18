#include "net_utils.h"
#include "errors.h"
#include "tcp_sock.h"
#include <iostream>

namespace net {

namespace netutils {

const static int MPTCP_ENABLED = 42;
const static int SOL_TCP = 6;

sockaddr_in parse_sockaddr_in(const std::string& addr, uint16_t port) {
    sockaddr_in ip_addr;
    int res = inet_aton(addr.data(), &ip_addr.sin_addr);
    if (res == 0) {
        throw Exception("failed to parse sockaddr_in, invalid addr");
    }
    ip_addr.sin_family = AF_INET;
    ip_addr.sin_port = htons(port);
    return ip_addr;
}

sockaddr_in parse_sockaddr_in(uint16_t port) {
    sockaddr_in ip_addr;
    ip_addr.sin_family = AF_INET;
    ip_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    ip_addr.sin_port = htons(port);
    return ip_addr;
}

void my_setsockopt(int sockfd, int level, int optname,
        const void *optval, socklen_t optlen) {
    int res = setsockopt(sockfd, level, optname, optval, optlen);
    if (res == -1) {
        throw LinuxError(errno);
    }
}

void set_mptcp_enable(int fd, int enable) {
    my_setsockopt(fd, SOL_TCP, MPTCP_ENABLED, &enable, sizeof(enable));
}

void set_tcp_reuse_addr(int fd, int reuse) {
    my_setsockopt(fd,SOL_SOCKET,SO_REUSEADDR, &reuse, sizeof(reuse));
}

TrafficFunc build_up_traffic_func() {
    //up traffic send data
    auto up_func = [](TCPSocket &sock, int batch_size, const std::string *batch_data)->std::pair<uint64_t, uint64_t> {
        uint64_t sent = 0;
        for (int i = 0; i < batch_size; i++) {
             sent += sock.tcp_send(*batch_data);
        }
        return std::make_pair(sent, 0);
    };
    return up_func;
}

TrafficFunc build_down_traffic_func() {
    //down traffic recv data
    auto down_func = [](TCPSocket &sock, int batch_size, const std::string *batch_data)->std::pair<uint64_t, uint64_t> {
        uint64_t recv = 0;
        while (true) {
            auto recv_data = sock.tcp_recv();
            auto recv_this_time = recv_data.length();
            if (recv_this_time == 0) {
                break;
            }
            recv += recv_this_time;
        }
        return std::make_pair(0, recv);
    };
    return down_func;
}

TrafficFunc build_bio_traffic_func(bool first_send) {
    //down traffic recv data
    TrafficFunc bio_func;
    if (first_send) {

    bio_func = [](TCPSocket &sock, int batch_size, const std::string *batch_data)->std::pair<uint64_t, uint64_t> {
        uint64_t recv = 0;
        uint64_t sent = 0;
        int index = 0;
        while (true) {
            //ping 
            if (index++ < batch_size) {
                sent += sock.tcp_send(*batch_data);
            } else {
                //end of send 
                break;
            }

            //pong 
            auto recv_data = sock.tcp_recv();
            auto recv_this_time = recv_data.length();
            if (recv_this_time == 0) break;;
            recv += recv_this_time;
        }
        return std::make_pair(sent, recv);
    };

    } else {

    bio_func = [](TCPSocket &sock, int batch_size, const std::string *batch_data)->std::pair<uint64_t, uint64_t> {
        uint64_t recv = 0;
        uint64_t sent = 0;
        while (true) {
            //pong 
            auto recv_data = sock.tcp_recv();
            auto recv_this_time = recv_data.length();
            if (recv_this_time == 0) break;
            recv += recv_this_time;

            //ping
            sent += sock.tcp_send(recv_data);
        }
        return std::make_pair(sent, recv);
    };

    }
    return bio_func;
}

}

}

