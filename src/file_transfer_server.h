#ifndef MPTCP_FILE_TRANSFER_SERVER_H 
#define MPTCP_FILE_TRANSFER_SERVER_H

#include <cstdint>
#include <string> 
#include <memory>
#include <iostream>
#include "in_sock.h"

namespace net {

using namespace sock;

struct ServerConfig {
    bool use_mptcp;
    uint16_t recv_buffer;
    std::uint16_t port;
    int version;

    void show() {
        std::cout << "use_mptcp: " << use_mptcp << "\n";
        std::cout << "recv_buffer(Bytes): " << recv_buffer << "\n";
        std::cout << "port: " << port << "\n";
        std::cout << "version: " << version << "\n";
    }
};

class FileTransferServer {
public: 
    FileTransferServer(const ServerConfig &config);

    void listen_and_transfer();  //start transfer kbytes data(fix)

private: 
    std::unique_ptr<TCPSockIn> _sock;
    ServerConfig _config;
};

}
#endif 
