#include "file_transfer_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream> 
#include <chrono>
#include "net_utils.h"

namespace net {

using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

FileTransferClient::FileTransferClient(const std::string& dest_ip, const std:: string& dest_port, size_t send_buffer_size, bool enable_mptcp)
    :_send_buffer_size(send_buffer_size), 
    _mptcp_enable(static_cast<int>(enable_mptcp)) {
    
    _addr = netutils::parse_sockaddr_in(dest_ip, static_cast<uint16_t>(std::stoul(dest_port)));
}

void FileTransferClient::start_transfer(uint64_t kbytes) {
    //transfer
    TCPSocket sock;
    
    //set tcp opts 
    netutils::set_mptcp_enable(sock.fd(), _mptcp_enable);

    sock.my_connect(_addr);
    int batch = ((kbytes << 10) / _send_buffer_size) + 1;
    uint64_t ksend_data = (batch * _send_buffer_size) >> 10;

    std::cout << "start send: " << ksend_data << " KB data\n";

    std::string data(_send_buffer_size, 'a');

    auto begin = high_resolution_clock::now();
    for (int i = 0; i < batch; i++) {
         sock.my_send(data);
    }
    auto end = high_resolution_clock::now();
    double time_interval = std::chrono::duration_cast<milliseconds>(end - begin).count() + 1e-5;

    std::cout << "send time cost(ms): " << time_interval << " rate(kbyte/s): " << static_cast<double>(1000*ksend_data/time_interval) << std::endl; 
}

}