#include "file_transfer_server.h"
#include "net_utils.h"
#include <iostream>
#include <chrono>
#include <unistd.h>

namespace net {

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

FileTransferServer::FileTransferServer(const std::string& bind_port, size_t recv_buffer_size, bool enable_mptcp) {
    auto server_addr = netutils::parse_sockaddr_in(static_cast<uint16_t>(std::stoul(bind_port)));
    _sock = std::unique_ptr<TCPSocket>(new TCPSocket(recv_buffer_size));

    //set tcp opt 
    netutils::set_mptcp_enable(_sock->fd(), static_cast<int>(enable_mptcp));
    //server should set reuse 
    netutils::set_tcp_reuse_addr(_sock->fd(), 1);
    
    //bind
    _sock->my_bind(server_addr);
}

RecvRes FileTransferServer::listen_and_recv() {
    //listen 1 
    _sock->my_listen();
        
    //accept
    sockaddr_in client_addr;
    int client_fd; 

    auto begin = high_resolution_clock::now();
    std::tie(client_addr, client_fd) = _sock->my_accept();
    //print client info 
    //std::cout << "accept: " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << '\n';
    size_t recvd_data = 0;
    //recv data
    while (true) {
        auto recvd_this_time = _sock->simple_recv(client_fd);
        if (recvd_this_time == 0) {
            break;
        }
        recvd_data += recvd_this_time;
    }
    close(client_fd);
    auto end = high_resolution_clock::now();

    auto krecvd_data = recvd_data >> 10;
    double time_interval = std::chrono::duration_cast<microseconds>(end - begin).count();
    return RecvRes(krecvd_data, time_interval);
}

}
