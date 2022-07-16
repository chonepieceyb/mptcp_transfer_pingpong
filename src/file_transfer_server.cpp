#include "file_transfer_server.h"
#include <chrono>
#include "utils.h"
#include <iostream>

extern "C" {

#include <unistd.h>

}

namespace net {

using namespace sock;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;

FileTransferServer::FileTransferServer(const ServerConfig &config):_config(config) {
    if (_config.use_mptcp) {
        //mptcp 
        if (_config.version == 0) {
            _sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new MPTCPSockInV0()));
        } else if(_config.version == 1) {
            _sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new MPTCPSockInV1()));
        } else {
            throw std::runtime_error("unkonwen mptcp version");
        }
    } else {
        _sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new TCPSockIn()));
        if (_config.version == 0) {
            //disable mptcp
            MPTCPSockInV0::set_mptcp_enable(_sock->view(), 0);
        }
        //tcp
    }
    
    if (_sock == nullptr) {
        throw std::runtime_error("file transfer server create socket failed");
    }
    std::cout << "mptcp sock create success\n";
    //server should set reuse 
    _sock->set_reuse_addr(1);
    
    //bind
    _sock->bind("", _config.port);
}

void FileTransferServer::listen_and_transfer() {
    auto recv_buf = utils::create_buffer(_config.recv_buffer, true);
    _sock->listen(3);    
    utils::TermSignal::regist();
    while (true) {
        try {
            SockAddrIn client_addr;
            auto client_sock = Socket<SockAddrIn>(_sock->accept(client_addr));

            // new connection
            uint64_t recv = 0;
            while (true) {
                auto recv_this_time = client_sock.recv(reinterpret_cast<char*>(recv_buf.get()), _config.recv_buffer, 0);
                if (recv_this_time == 0) {
                    break;
                }
                recv += recv_this_time;
            }       
            //std::cout << "recv : " << recv << "\n"; 
        } catch (const errors::KeyboardInterrupt &e) {
            break;
        } catch (std::system_error &e) {
            std::cout << "system error: " << e.what() << " " << e.code()<< std::endl;
        }
    }

}

}
