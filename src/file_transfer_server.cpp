#include "file_transfer_server.h"
#include <iostream>
#include <chrono>

namespace net {

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

FileTransferServer::FileTransferServer(ServerConfig config):_config(std::move(config)) {
    auto server_addr = netutils::parse_sockaddr_in(_config.port);
    _sock = std::unique_ptr<TCPSocket>(new TCPSocket(1));

    //set tcp opt 
    netutils::set_mptcp_enable(_sock->fd(), static_cast<int>(_config.use_mptcp));
    //server should set reuse 
    netutils::set_tcp_reuse_addr(_sock->fd(), 1);
    
    switch(_config.mode) {
    case TrafficMode::C_TO_S:
        _traffic_func = netutils::build_down_traffic_func();
        break;

    case TrafficMode::S_TO_C:
        _traffic_func = netutils::build_up_traffic_func();
        break;

    case TrafficMode::BIO:
        _traffic_func = netutils::build_bio_traffic_func(false);
        break;

    default:
        throw Exception("unknowen traffic mode");
    }

    //bind
    _sock->tcp_bind(server_addr);
}

void FileTransferServer::listen_and_transfer(std::uint64_t kbytes) {
    //listen 1 
    int batch = 0;
    std::string batch_data;

    if (_config.mode == TrafficMode::S_TO_C) {
        batch = ((kbytes << 10) / _config.send_buffer) + 1;
        batch_data = std::string(_config.send_buffer, 'a');
    }

    std::uint64_t sent = 0, recv = 0;
    TCPSocket client_sock(-1, _config.recv_buffer);

    netutils::TermSignal ts;
    while (ts.ok()) {
        _sock->tcp_listen();
        
        //accept
        sockaddr_in client_addr;
        int client_fd; 
        std::tie(client_addr, client_fd) = _sock->tcp_accept();
        client_sock.reset(client_fd);  //wapper
        //print client info 
        //std::cout << "accept: " << inet_ntoa(client_addr.sin_addr) << ':' << ntohs(client_addr.sin_port) << '\n';
        std::tie(sent, recv) = _traffic_func(client_sock, batch, &batch_data);   
        client_sock.tcp_close();
    }
}

}
