#include <arpa/inet.h>
#include "net_utils.h"
#include "errors.h"


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

}

}
