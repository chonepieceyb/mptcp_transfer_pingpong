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

    void show() {
        std::cout << "use_mptcp: " << use_mptcp << "\n";
        std::cout << "send_buffer(Bytes): " << send_buffer << "\n";
        std::cout << "port: " << port << "\n";
        std::cout << "ip address: " << address << "\n";
        std::cout << "bind address: " << bind_address << "\n";
        std::cout << "mptcp version: " << version << "\n";
    }
};

void read_blocks(const std::string &file_path, std::vector<std::uint64_t> &blocks, std::vector<std::uint32_t> &intervals) {
    std::ifstream ifile;
    ifile.open(file_path, std::ios::in);
    if (!ifile.is_open()) {
        throw std::runtime_error("fail to open block file");
    }
    uint64_t block;
    uint32_t interval;
    //while(ifile.peek() != std::char_traits<char>::eof()) {
    while(ifile >> block >> interval) {
        blocks.push_back(block);
        intervals.push_back(interval);
    }
    ifile.close();
}

double transfer_block(TCPSockIn *sock, std::uint64_t block_size, char *buf, std::uint16_t buf_size) {
    int batch = (block_size << 10) / buf_size;
    auto res_size = (block_size << 10) % buf_size;
    auto begin = std::chrono::high_resolution_clock::now();
    //transfer data 
    for (int i = 0; i < batch; i++) {
        std::memset(buf, 'b', buf_size);
        sock->send(buf, buf_size, 0);
    }
    if (res_size != 0) {
        std::memset(buf, 'b', res_size);
        sock->send(buf, res_size, 0);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}

void transfer(const ECFExpClientConfig &config, const std::vector<std::uint64_t> &blocks, const std::vector<std::uint32_t> &intervals) {
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
    std::cout << "block_size" << " " << "duration\n"; 
    //transfer data 
    double all_time = 0;
    auto buf = utils::create_buffer(config.send_buffer, true);
    for (int i = 0; i < blocks.size(); i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(intervals[i]));
        auto duration = transfer_block(client_sock.get(), blocks[i], reinterpret_cast<char*>(buf.get()), config.send_buffer);
        std::cout << blocks[i] << " " << duration << "\n";
        all_time += duration;
    }
    std::cout << "all: " << all_time << "\n";
}



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
        ("bind_address,i", po::value<std::string>()->default_value(""), "bind address to client")

        ("config", "show config info")
        ("blocks", po::value<std::vector<std::uint64_t>>()->multitoken(), " blocks to send")
        ("intervals", po::value<std::vector<std::uint32_t>>()->multitoken(), "interval before each block to send")
        ("file,f", po::value<std::string>(), "block's file")
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

        assert(((vm.count("blocks") && vm.count("intervals")) || vm.count("file")) && "must provide both blocks and intervals or provide file");
        if (vm.count("file")) {
            read_blocks(vm["file"].as<std::string>(), blocks, intervals);
        } else {
            blocks = vm["blocks"].as<std::vector<std::uint64_t>>();
            intervals = vm["intervals"].as<std::vector<std::uint32_t>>();       
            assert((blocks.size() == intervals.size()) && "block's size must equal intervals's size");
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    if (vm.count("config")) {
        //show configs 
        config.show();
        std::cout << "interval-block: ";
        for (int i = 0; i < blocks.size(); i++) {
            std::cout << intervals[i] << "-" << "\n";
        }
        std::cout << "\n";
    }
    
    try {
        transfer(config, blocks, intervals);
    } catch (const std::system_error &e) {
        std::cerr << "system_error: " << e.what() << " " << e.code() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return -1;
    }

}
