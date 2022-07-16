#include <iostream> 
#include <tuple>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <string> 
#include <vector>
#include <cstdint>
#include "file_msg_client.h"
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
        ("address,a", po::value<std::string>()->default_value("127.0.0.1"), "server's ip address")
        ("config", "show config info")
        ("verbose,v", "print transfer result to stdout")
        ("msg_num", po::value<std::uint32_t>(), "position arguments, msg number per socket")
        
        ("socket_num", po::value<std::uint32_t>(), "position arguments, socket number")
        ;
    po::positional_options_description pd;
    pd.add("msg_num", 1).add("socket_num", 1);
    
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
    net::MsgClientConfig config;
    bool verbose = false;
    std::uint32_t msg_num, socket_num;
    try {
        config.use_mptcp = true; 
        if (vm.count("tcp")) config.use_mptcp = false;
        config.send_buffer = vm["send_buffer"].as<std::uint16_t>();
        config.port = vm["port"].as<std::uint16_t>();
        config.version = vm["version"].as<int>();
        if (vm.count("verbose")) {
            verbose = true;
        }
        if (!vm.count("address")) throw std::runtime_error("address is needed");
        config.address = vm["address"].as<std::string>();
        msg_num = vm["msg_num"].as<std::uint32_t>();
        socket_num = vm["socket_num"].as<std::uint32_t>();
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    if (vm.count("config")) {
        //show configs 
        config.show();
        std::cout << "msg_num: " << msg_num << "\n";
        std::cout << "socket_num: " << socket_num << "\n";
    }
    
    try {
        net::FileMsgClient client(config);
        for (int i = 0; i < socket_num; i++) {
            auto res = client.transfer(msg_num);
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
