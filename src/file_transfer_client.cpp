#include "file_transfer_client.h"
#include <chrono>
#include "utils.h"

namespace net {

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

FileTransferClient::FileTransferClient(const ClientConfig &config) : _config(config) {
}
    
TransferRes FileTransferClient::transfer(std::uint64_t kbytes) {
    //create a new socket transfer kbytes, and close socket, each time a transfer is called , a new socket is created
    int batch = 0;
    std::string batch_data, res_data;

    batch = (kbytes << 10) / _config.send_buffer;
    auto res_size = (kbytes << 10) % _config.send_buffer;
    batch_data = std::string(_config.send_buffer, 'a');
    res_data = std::string(res_size, 'a');
    
    std::chrono::time_point<std::chrono::high_resolution_clock>  begin;     
    uint64_t sent = 0;
    { 
        std::unique_ptr<TCPSockIn> client_sock;
        if (_config.use_mptcp) {
            //mptcp 
            if (_config.version == 0) {
                client_sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new MPTCPSockInV0()));
            } else if(_config.version == 1) {
                client_sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new MPTCPSockInV1()));
            } else {
                throw std::runtime_error("unkonwen mptcp version");
            }       
        } else {
            client_sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new TCPSockIn()));
            if (_config.version == 0) {
                //disable mptcp
                MPTCPSockInV0::set_mptcp_enable(client_sock->view(), 0);
            }
            //tcp
        }   

        //
        begin = high_resolution_clock::now();
        client_sock->connect(_config.address, _config.port);
    
        //transfer data 
        for (int i = 0; i < batch; i++) {
            sent += client_sock->send(batch_data.data(), batch_data.length(), 0);
        }
        if (!res_data.empty()) {
            sent += client_sock->send(res_data.data(), res_data.length(), 0);
        }
    }
    auto end = high_resolution_clock::now();
    double time_interval = std::chrono::duration_cast<microseconds>(end - begin).count();
    return TransferRes(sent >> 10, 0, time_interval);
}

}
