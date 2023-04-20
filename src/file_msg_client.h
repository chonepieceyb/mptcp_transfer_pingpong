#ifndef MPTCP_FILE_TRANSFER_CLIENT_H 
#define MPTCP_FILE_TRANSFER_CLIENT_H

#include <cstdint>
#include <string>
#include <iostream>
#include "common.h"
#include "in_sock.h"

namespace net {

using namespace sock;

struct MsgClientConfig {
    bool use_mptcp;
    uint16_t send_buffer;
    std::string address;
    std::uint16_t port;
    int version;

    void show() {
        std::cout << "use_mptcp: " << use_mptcp << "\n";
        std::cout << "send_buffer(Bytes): " << send_buffer << "\n";
        std::cout << "port: " << port << "\n";
        std::cout << "ip address: " << address << "\n";
        std::cout << "mptcp version: " << version << "\n";
    }
};

class FileMsgClient {
public: 
    FileMsgClient(const MsgClientConfig &config);

    MsgTransferRes transfer(std::uint32_t msg_num);  //start transfer kbytes data(fix)

private:
    MsgClientConfig _config;
};

}
#endif 
