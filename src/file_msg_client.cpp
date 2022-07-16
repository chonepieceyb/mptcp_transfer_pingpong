#include "file_msg_client.h"
#include <chrono>
#include "utils.h"

namespace net {

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

FileMsgClient::FileMsgClient(const MsgClientConfig &config) : _config(config) {
}
    
MsgTransferRes FileMsgClient::transfer(std::uint32_t msg_num) {
    //example 
    
    //define mesg 
    std::string msg (1428, 'a');

    std::chrono::time_point<std::chrono::high_resolution_clock>  begin;     
    { 
/*-----------create mptcp socket begin ---------*/
        //create mptcp socket don't modify 
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

/*-----------create mptcp socket end ---------*/
        

        begin = high_resolution_clock::now();
        client_sock->connect(_config.address, _config.port);
    

/*-----------transfer data begin (modify this block )---------*/
        
        for (int i = 0; i < msg_num; i++) {
            client_sock->send(msg.data(), msg.length(), 0);
        }
/*-----------transfer data end (modify this block )---------*/
        
    }
    auto end = high_resolution_clock::now();
    double time_interval = std::chrono::duration_cast<microseconds>(end - begin).count();
    return MsgTransferRes(msg_num, time_interval);
}

}
