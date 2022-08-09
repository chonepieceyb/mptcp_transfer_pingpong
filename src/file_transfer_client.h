#ifndef MPTCP_FILE_TRANSFER_CLIENT_H 
#define MPTCP_FILE_TRANSFER_CLIENT_H

#include <cstdint>
#include <string>
#include <iostream>
#include "common.h"
#include "in_sock.h"

namespace net {

using namespace sock;

struct ClientConfig {
    bool use_mptcp;
    uint16_t send_buffer;
    std::string address;
    std::string bind_address;
    std::uint16_t port;
    int version;

    void show() {
        std::cout << "use_mptcp: " << use_mptcp << "\n";
        std::cout << "send_buffer(Bytes): " << send_buffer << "\n";
        std::cout << "port: " << port << "\n";
        std::cout << "ip address: " << address << "\n";
        std::cout << "bind address: " << bind_address << "\n";
        std::cout << "mptcp version: " << version << "\n";
    }
};

class FileTransferClient {
public: 
    FileTransferClient(const ClientConfig &config);

    TransferRes transfer(std::uint64_t kbytes);  //start transfer kbytes data(fix)

private:
    ClientConfig _config;
};

}
#endif 
