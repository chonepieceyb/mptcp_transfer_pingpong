#ifndef MPTCP_FILE_TRANSFER_CLIENT_H 
#define MPTCP_FILE_TRANSFER_CLIENT_H

#include <cstdint>
#include <string>
#include <sstream>
#include "tcp_sock.h"

namespace net {

struct SendRes {
    SendRes(uint64_t ks, double time) : ksended_data(ks), time_ms(time) {
        rate = static_cast<double>(1000 *1000 * ksended_data / (time_ms + 1e-6));
    }
    uint64_t ksended_data;
    double time_ms;  //micro seconds
    double rate;   //KB/s

    std::string str() {
        if (_str.empty()) {
            std::ostringstream s;
            s << ksended_data << " " << time_ms << " " << rate;
            _str = s.str();
        }
        return _str;
    }
    std::string _str;
};

class FileTransferClient {
public: 
    FileTransferClient(const std::string& dest_ip, const std::string& dest_port, size_t send_buffer_size = 1400, bool enable_mptcp = true);

    SendRes start_transfer(uint64_t kbytes);  //start transfer kbytes data(fix)

private: 
    sockaddr_in _addr;
    size_t _send_buffer_size;
    int _mptcp_enable;
};

}
#endif 
