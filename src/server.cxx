#include <iostream> 
#include <tuple>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <string> 
#include <vector>
#include <cstdint>
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
        ("recv_buffer,B", po::value<std::uint16_t>()->default_value(net::RECV_BUFFER), "recv buffer size")
        ("version,V", po::value<int>()->default_value(1), "mptcp_version")
        ("port,p", po::value<std::uint16_t>()->default_value(net::DEFAULT_PORT), "server's port")
        ("bind_address,i", po::value<std::string>()->default_value(""), "bind address to server")

        ("config", "show config info")
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
        std::cout << "usage: ./client [options] block(KB)" << '\n'; 
        std::cout << desc << std::endl;;
        return 0;
    }

    net::ServerConfig config;
    try {
        config.use_mptcp = true; 
        if (vm.count("tcp")) config.use_mptcp = false;
        config.recv_buffer = vm["recv_buffer"].as<std::uint16_t>();
        config.port = vm["port"].as<std::uint16_t>();
        config.version = vm["version"].as<int>();
        config.bind_address = vm["bind_address"].as<std::string>();
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    if (vm.count("config")) {
        //show configs 
        config.show();
        std::cout << '\n';
    }
    
    try {
        net::FileTransferServer server(config);
        std::cout << "begin listen \n";
        server.listen_and_transfer();
    } catch (const std::system_error &e) {
        std::cerr << "system_error: " << e.what() << " " << e.code() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return -1;
    }

}
