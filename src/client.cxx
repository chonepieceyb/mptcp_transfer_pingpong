#include <iostream> 
#include <tuple>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <string> 
#include <vector>
#include <cstdint>
#include "file_transfer_client.h"
#include "errors.h"
#include "common.h"


namespace po = boost::program_options;

int main(int argc, char** argv) {

//./client [-t, --tcp] [-p, --port][-b,send_buffer][-B, recv_buffer] [-M=S,R,SR, --mode] [-a, --address] list_of blocks to send 
//./transfer_pingpong mptcp/tcp server port 
//./transfer_pingpong mptcp/tcp client dest port data_size(KB)


    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help messages")
        ("tcp,t", "use tcp")
        ("send_buffer,b", po::value<std::uint16_t>()->default_value(net::SEND_BUFFER), "send buffer size")
        ("version,V", po::value<int>()->default_value(1), "mptcp version")
        ("port,p", po::value<std::uint16_t>()->default_value(net::DEFAULT_PORT), "server's port")
        ("bind_port,P", po::value<std::uint16_t>()->default_value(0), "client's bind port")

        ("address,a", po::value<std::string>()->default_value("127.0.0.1"), "server's ip address")
        ("bind_address,i", po::value<std::string>()->default_value(""), "bind address to client")

        ("config", "show config info")
        ("verbose,v", "print transfer result to stdout")
        ("blocks", po::value<std::vector<std::uint64_t>>(), "position arguments, blocks to send")
        ;
    po::positional_options_description pd;
    pd.add("blocks", -1);
    
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
        std::cout << "usage: ./client [options] block1(KB) block2..." << '\n'; 
        std::cout << desc << std::endl;;
        return 0;
    }

    std::vector<std::uint64_t> blocks;
    net::ClientConfig config;
    bool verbose = false;
    try {
        config.use_mptcp = true; 
        if (vm.count("tcp")) config.use_mptcp = false;
        config.send_buffer = vm["send_buffer"].as<std::uint16_t>();
        config.port = vm["port"].as<std::uint16_t>();
        config.bind_port = vm["bind_port"].as<std::uint16_t>();
        config.version = vm["version"].as<int>();
        if (vm.count("verbose")) {
            verbose = true;
        }
        if (!vm.count("address")) throw std::runtime_error("address is needed");
        config.address = vm["address"].as<std::string>();
        config.bind_address = vm["bind_address"].as<std::string>();
        if (vm.count("blocks")) {
            blocks = vm["blocks"].as<std::vector<std::uint64_t>>();
        } else {
            throw std::runtime_error("block is need");
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    if (vm.count("config")) {
        //show configs 
        config.show();
        std::cout << "blocks: ";
        for (auto b : blocks) {
            std::cout << b << " ";
        }
        std::cout << '\n';
    }
    
    try {
        net::FileTransferClient client(config);
        for (auto block : blocks) {
            auto res = client.transfer(block);
            if (verbose)
                std::cout << res.str() << "\n";
        }
        return 0;
    } catch (const std::system_error &e) {
        std::cerr << "system_error: " << e.what() << " " << e.code() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return -1;
    }

}
