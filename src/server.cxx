#include <iostream> 
#include <tuple>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <string> 
#include <vector>
#include <cstdint>
#include "net_utils.h"
#include "file_transfer_server.h"
#include "errors.h"
#include "common.h"


namespace po = boost::program_options;

int main(int argc, char** argv) {

//./server [-t, --tcp] [-p, --port][-b,send_buffer][-B, recv_buffer] [-M=S,R,SR, --mode]  block
//./transfer_pingpong mptcp/tcp server port 
//./transfer_pingpong mptcp/tcp client dest port data_size(KB)


    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help messages")
        ("tcp,t", "use tcp")
        ("mode,M", po::value<std::string>()->default_value("C_S"), "client mode [C_S,S_C,BIO], should be the same as server's")
        ("send_buffer,b", po::value<std::uint16_t>()->default_value(net::SEND_BUFFER), "send buffer size")
        ("recv_buffer,B", po::value<std::uint16_t>()->default_value(net::RECV_BUFFER), "recv buffer size")
        ("port,p", po::value<std::uint16_t>()->default_value(net::DEFAULT_PORT), "server's port")
        ("config", "show config info")
        ("block", po::value<std::uint64_t>(), "position argument, block to send")
        ;
    po::positional_options_description pd;
    pd.add("block", 1);
    
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).
            options(desc).positional(pd).run(), vm);
        po::notify(vm);
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        std::cerr << "use -h or --help for help" << std::endl;
        return -1;
    }
    if (vm.count("help")) {
        std::cout << "usage: ./client [options] block(KB)" << '\n'; 
        std::cout << desc << std::endl;;
        return 0;
    }

    std::uint64_t block = 0;
    net::ServerConfig config;
    try {
        config.use_mptcp = true; 
        if (vm.count("tcp")) config.use_mptcp = false;
        config.mode = net::string_to_tm(vm["mode"].as<std::string>());
        config.send_buffer = vm["send_buffer"].as<std::uint16_t>();
        config.recv_buffer = vm["recv_buffer"].as<std::uint16_t>();
        config.port = vm["port"].as<std::uint16_t>();
        
        if (config.mode == net::TrafficMode::S_TO_C) {
            if (vm.count("block")) {
                block = vm["block"].as<std::uint64_t>();
            } else {
                throw net::Exception("block is need");
            }
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    if (vm.count("config")) {
        //show configs 
        config.show();
        std::cout << "block: " << block;
        std::cout << '\n';
    }
    
    try {
        net::FileTransferServer server(config);
        server.listen_and_transfer(block);
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return -1;
    }
}
