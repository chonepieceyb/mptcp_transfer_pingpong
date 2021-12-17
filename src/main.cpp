#include <iostream> 
#include <tuple>
#include "net_utils.h"
#include "file_transfer_client.h"
#include "file_transfer_server.h"
#include "errors.h"

using namespace std;
using namespace net;

int main(int argc, char** argv) {

//./transfer_pingpong mptcp/tcp server port 
//./transfer_pingpong mptcp/tcp client dest port data_size(KB)

    if (argc < 3) {
        cout << "usage: \n"\
             << "./transfer_pingpong mptcp/tcp server port\n"\
             << "./transfer_pingpong mptcp/tcp client dest port data_size(KB)"\
             << endl;

        return -1;
    }
    bool mptcp = false;
    if (string(argv[1]) == "mptcp") mptcp = true;

    string mode = argv[2];
    string dest_addr, port;
    size_t data_size;
    
    try {
    if (mode == "server" && argc == 4) {
        port = argv[3];
        FileTransferServer server(port, 60000, mptcp); 
        std::cout << "recvd(KB)\t" << "time(ms)\t" << "rate(KB/s)" << std::endl;
        netutils::TermSignal ts;
        while (ts.ok()) {
            auto res = server.listen_and_recv();
            std::cout << res.str() << "\n";
        }
    } else if (mode == "client" && argc == 6) {
        dest_addr = argv[3];
        port = argv[4];
        data_size = stoul(argv[5]);
        FileTransferClient client(dest_addr, port, 60000, mptcp);
        auto res = client.start_transfer(data_size);
        std::cout << res.str() << "\n";
        return 0;
    } else {
        cout << "no mode support\n" << endl;
        return -1;
    }
    } catch (const LinuxError &e) {
        cerr << e.err() << " " << e.what() << endl;    
    } catch (const std::exception &e) {
        cerr << e.what() << endl;
    }
}
