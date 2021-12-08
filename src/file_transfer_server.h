#ifndef MPTCP_FILE_TRANSFER_SERVER_H 
#define MPTCP_FILE_TRANSFER_SERVER_H

#include <cstdint>
#include <string> 
#include <memory>
#include "tcp_sock.h"

namespace net {

class FileTransferServer {
public: 
    FileTransferServer(const std::string& bind_port, size_t recv_buffer_size = 2048, bool enable_mptcp = true);

    void listen_and_recv();  //start transfer kbytes data(fix)

private: 
    std::unique_ptr<TCPSocket> _sock;
};

}
#endif 
