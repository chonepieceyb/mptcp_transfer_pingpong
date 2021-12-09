#include "file_transfer_client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream> 
#include "net_utils.h"

namespace net {

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

    std::cout << "start send: " << ((batch * _send_buffer_size) >> 10) << " KB data\n";

    std::string data(_send_buffer_size, 'a');
    for (int i = 0; i < batch; i++) {
         sock.my_send(data);
    }
}

}
