#ifndef MPTCP_FILE_TRANSFER_CLIENT_H 
#define MPTCP_FILE_TRANSFER_CLIENT_H

#include <cstdint>
#include <string>
#include "tcp_sock.h"

namespace net {

class FileTransferClient {
public: 
    FileTransferClient(const std::string& dest_ip, const std::string& dest_port, size_t send_buffer_size = 1400, bool enable_mptcp = true);

    void start_transfer(uint64_t kbytes);  //start transfer kbytes data(fix)

private: 
    sockaddr_in _addr;
    size_t _send_buffer_size;
    int _mptcp_enable;
};

}
#endif 
