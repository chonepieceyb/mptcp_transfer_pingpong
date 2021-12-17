#ifndef MPTCP_FILE_TRANSFER_SERVER_H 
#define MPTCP_FILE_TRANSFER_SERVER_H

#include <cstdint>
#include <string> 
#include <memory>
#include <sstream> 
#include "tcp_sock.h"

namespace net {

struct RecvRes {
    RecvRes(uint64_t kr, double time) : krecvd_data(kr), time_ms(time) {
        rate = static_cast<double>(1000 *1000* krecvd_data / (time_ms + 1e-6));
    }
    uint64_t krecvd_data;
    double time_ms;
    double rate;   //KB/s

    std::string str() {
        if (_str.empty()) {
            std::ostringstream s;
            s << krecvd_data << " " << time_ms << " " << rate;
            _str = s.str();
        }
        return _str;
    }
    std::string _str;
};

class FileTransferServer {
public: 
    FileTransferServer(const std::string& bind_port, size_t recv_buffer_size = 2048, bool enable_mptcp = true);

    RecvRes listen_and_recv();  //start transfer kbytes data(fix)

private: 
    std::unique_ptr<TCPSocket> _sock;
};

}
#endif 
