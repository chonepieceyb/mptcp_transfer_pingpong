#include "file_transfer_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream> 
#include <chrono>
#include "net_utils.h"

namespace net {

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

FileTransferClient::FileTransferClient(const std::string& dest_ip, const std:: string& dest_port, size_t send_buffer_size, bool enable_mptcp)
    :_send_buffer_size(send_buffer_size), 
    _mptcp_enable(static_cast<int>(enable_mptcp)) {
    
    _addr = netutils::parse_sockaddr_in(dest_ip, static_cast<uint16_t>(std::stoul(dest_port)));
}

SendRes FileTransferClient::start_transfer(uint64_t kbytes) {
    //transfer
    TCPSocket sock;
    
    //set tcp opts 
    netutils::set_mptcp_enable(sock.fd(), _mptcp_enable);
    int batch = ((kbytes << 10) / _send_buffer_size) + 1;
    uint64_t ksend_data = (batch * _send_buffer_size) >> 10;
    std::string data(_send_buffer_size, 'a');

    auto begin = high_resolution_clock::now();
    sock.my_connect(_addr);
    for (int i = 0; i < batch; i++) {
         sock.my_send(data);
    }
    sock.my_close();
    auto end = high_resolution_clock::now();
    double time_interval = std::chrono::duration_cast<microseconds>(end - begin).count();

    return SendRes(ksend_data, time_interval);
}

}
