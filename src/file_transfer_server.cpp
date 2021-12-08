#include "file_transfer_server.h"
#include "net_utils.h"
#include <iostream>

namespace net {

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

void FileTransferServer::listen_and_recv() {
    //listen 1 
    _sock->my_listen();
        
    //accept
    sockaddr_in client_addr;
    int client_fd; 
    std::tie(client_addr, client_fd) = _sock->my_accept();

    //print client info 
    std::cout << "accept: " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << '\n';
    size_t recvd_data = 0;
    //recv data
    while (true) {
        auto data = _sock->my_recv();
        if (data.empty()) {
            break;
        }
        recvd_data += data.length();
    }
    std::cout << "recvd: " << (recvd_data >> 10) << " KB data\n"; 
}

}
