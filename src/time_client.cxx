#include <iostream> 
#include <tuple>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <string> 
#include <vector>
#include <chrono>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <thread>
#include <memory>
#include <fstream>
#include "in_sock.h"
#include "errors.h"
#include "common.h"
#include "utils.h"

using namespace sock;

struct ECFExpClientConfig {
    bool use_mptcp;
    uint16_t send_buffer;
    std::string address;
    std::string bind_address;
    std::uint16_t port;
    int version;
    int time; 

    void show() {
        std::cout << "use_mptcp: " << use_mptcp << "\n";
        std::cout << "send_buffer(Bytes): " << send_buffer << "\n";
        std::cout << "port: " << port << "\n";
        std::cout << "ip address: " << address << "\n";
        std::cout << "bind address: " << bind_address << "\n";
        std::cout << "mptcp version: " << version << "\n";
        std::cout << "time transfer: " << time << "\n";
    }
};



auto transfer_time(TCPSockIn *sock, int time, char *buf, std::uint16_t buf_size) -> std::pair<std::uint64_t, double> {
    std::uint64_t sent = 0;
    utils::TermSignal::regist();

    auto begin = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point end;
    while (true) {
        try {
            end = std::chrono::high_resolution_clock::now();
            if (time > 0 && std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() >= time) {
                //transfer finish
                break; 
            }
            std::memset(buf, 'b', buf_size);
            sock->send(buf, buf_size, 0);
            sent += buf_size;
        } catch (const errors::KeyboardInterrupt &e) {
            break;
        }
        
    }
    end = std::chrono::high_resolution_clock::now();
    return std::make_pair(sent, std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
}

void transfer(const ECFExpClientConfig &config) {
    std::unique_ptr<TCPSockIn> client_sock;
    if (config.use_mptcp) {
        //mptcp 
        if (config.version == 0) {
            client_sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new MPTCPSockInV0()));
        } else if(config.version == 1) {
            client_sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new MPTCPSockInV1()));
        } else {
            throw std::runtime_error("unkonwen mptcp version");
        }       
    } else {
        client_sock = std::unique_ptr<TCPSockIn>(dynamic_cast<TCPSockIn*>(new TCPSockIn()));
        if (config.version == 0) {
            //disable mptcp
            MPTCPSockInV0::set_mptcp_enable(client_sock->view(), 0);
        }
            //tcp
    }   
    
    client_sock->bind(config.bind_address, 0);

    //
    client_sock->connect(config.address, config.port);
    std::cout << "sent" << " " << "duration(us)\n"; 
    //transfer data 
    auto buf = utils::create_buffer(config.send_buffer, true);
    auto res_pair = transfer_time(client_sock.get(), config.time, reinterpret_cast<char*>(buf.get()), config.send_buffer);
    std::cout << res_pair.first << " " << res_pair.second << "\n";
}



namespace po = boost::program_options;

int main(int argc, char** argv) {

//./client [-t, --tcp] [-p, --port][-b,send_buffer][-B, recv_buffer] [-M=S,R,SR, --mode] [-a, --address] list_of blocks to send 
//./transfer_pingpong mptcp/tcp server port 
//./transfer_pingpong mptcp/tcp client dest port data_size(KB)
    //boost 

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help messages")
        ("tcp,t", "use tcp")
        ("send_buffer,b", po::value<std::uint16_t>()->default_value(net::SEND_BUFFER), "send buffer size")
        ("version,V", po::value<int>()->default_value(1), "mptcp version")
        ("port,p", po::value<std::uint16_t>()->default_value(net::DEFAULT_PORT), "server's port")
        ("address,a", po::value<std::string>()->default_value("127.0.0.1"), "server's ip address")
        ("bind_address,i", po::value<std::string>()->default_value(""), "bind address to client")

        ("config", "show config info")
        ("time,T", po::value<int>()->default_value(1000), "time to be transfer, -1 means unlimited")
        ;
       
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).
            options(desc).run(), vm);
        po::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        std::cerr << "use -h or --help for help" << std::endl;
        return -1;
    }
    if (vm.count("help")) {
        std::cout << "usage: ./client [options] block1(KB) block2..." << '\n'; 
        std::cout << desc << std::endl;;
        return 0;
    }

    std::vector<std::uint64_t> blocks;
    std::vector<std::uint32_t> intervals;
    ECFExpClientConfig config;
    try {
        config.use_mptcp = true; 
        if (vm.count("tcp")) config.use_mptcp = false;
        config.send_buffer = vm["send_buffer"].as<std::uint16_t>();
        config.port = vm["port"].as<std::uint16_t>();
        config.version = vm["version"].as<int>();
        config.address = vm["address"].as<std::string>();
        config.bind_address = vm["bind_address"].as<std::string>();
        config.time = vm["time"].as<int>();
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    if (vm.count("config")) {
        //show configs 
        config.show();
    }
    
    try {
        transfer(config);
    } catch (const std::system_error &e) {
        std::cerr << "system_error: " << e.what() << " " << e.code() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return -1;
    }

}
