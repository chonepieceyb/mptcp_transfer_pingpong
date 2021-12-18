#ifndef MPTCP_FILE_TRANSFER_CLIENT_H 
#define MPTCP_FILE_TRANSFER_CLIENT_H

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include "tcp_sock.h"
#include "common.h"
#include "net_utils.h"

namespace net {

struct ClientConfig {
    bool use_mptcp;
    TrafficMode mode;
    uint16_t send_buffer;
    uint16_t recv_buffer;
    std::string address;
    std::uint16_t port;

    void show() {
        std::cout << "use_mptcp: " << use_mptcp << "\n";
        std::cout << "mode: " << tm_to_string(mode) << "\n";
        std::cout << "send_buffer(Bytes): " << send_buffer << "\n";
        std::cout << "recv_buffer(Bytes): " << recv_buffer << "\n";
        std::cout << "port: " << port << "\n";
        std::cout << "ip address: " << address << "\n";
    }
};

class FileTransferClient {
public: 
    FileTransferClient(ClientConfig config);

    TransferRes transfer(std::uint64_t kbytes);  //start transfer kbytes data(fix)

private:
    sockaddr_in _addr;
    ClientConfig _config;
    netutils::TrafficFunc _traffic_func;
};

}
#endif 
