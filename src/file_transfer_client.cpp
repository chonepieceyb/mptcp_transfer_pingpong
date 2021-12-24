#include "file_transfer_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream> 
#include <chrono>

namespace net {

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

FileTransferClient::FileTransferClient(ClientConfig config) : _config(std::move(config)) {
    _addr = netutils::parse_sockaddr_in(_config.address, _config.port);
    switch(_config.mode) {
    case TrafficMode::C_TO_S:
        _traffic_func = netutils::build_up_traffic_func();
        break;

    case TrafficMode::S_TO_C:
        _traffic_func = netutils::build_down_traffic_func();
        break;

    case TrafficMode::BIO:
        _traffic_func = netutils::build_bio_traffic_func(true);
        break;

    default:
        throw Exception("unknowen traffic mode");
    }
}

TransferRes FileTransferClient::transfer(std::uint64_t kbytes) {
    int batch = 0;
    std::string batch_data, res_data;
    std::uint64_t sent = 0, recv = 0;

    if (_config.mode != TrafficMode::S_TO_C) {
        batch = (kbytes << 10) / _config.send_buffer;
        auto res_size = (kbytes << 10) % _config.send_buffer;
        batch_data = std::string(_config.send_buffer, 'a');
        res_data = std::string(res_size, 'a');
    }

    //transfer
    TCPSocket sock(_config.recv_buffer);
    
    //set tcp opts 
    netutils::set_mptcp_enable(sock.fd(), _config.use_mptcp);

    auto begin = high_resolution_clock::now();
    sock.tcp_connect(_addr);
    std::tie(sent, recv) = _traffic_func(sock, batch, &batch_data, &res_data);   
    sock.tcp_close();
    auto end = high_resolution_clock::now();
    double time_interval = std::chrono::duration_cast<microseconds>(end - begin).count();
    return TransferRes(sent >> 10, recv >> 10, time_interval);
}

}
