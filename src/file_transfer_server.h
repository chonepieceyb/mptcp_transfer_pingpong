#ifndef MPTCP_FILE_TRANSFER_SERVER_H 
#define MPTCP_FILE_TRANSFER_SERVER_H

#include <cstdint>
#include <string> 
#include <memory>
#include <iostream>
#include "tcp_sock.h"
#include "common.h"
#include "net_utils.h"

namespace net {

struct ServerConfig {
    bool use_mptcp;
    TrafficMode mode;
    uint16_t send_buffer;
    uint16_t recv_buffer;
    std::uint16_t port;

    void show() {
        std::cout << "use_mptcp: " << use_mptcp << "\n";
        std::cout << "mode: " << tm_to_string(mode) << "\n";
        std::cout << "send_buffer(Bytes): " << send_buffer << "\n";
        std::cout << "recv_buffer(Bytes): " << recv_buffer << "\n";
        std::cout << "port: " << port << "\n";
    }
};

class FileTransferServer {
public: 
    FileTransferServer(ServerConfig config);

    void listen_and_transfer(std::uint64_t kbytes);  //start transfer kbytes data(fix)

private: 
    std::unique_ptr<TCPSocket> _sock;
    netutils::TrafficFunc _traffic_func;
    ServerConfig _config;
   
};

}
#endif 
