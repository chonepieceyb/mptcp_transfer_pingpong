#include <iostream> 
#include <tuple>
#include "file_transfer_client.h"
#include "file_transfer_server.h"

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
    
    if (mode == "server" && argc == 4) {
        port = argv[3];
        FileTransferServer server(port, 1400, mptcp); 
        while (true) {
            server.listen_and_recv();
        }
    } else if (mode == "client" && argc == 6) {
        dest_addr = argv[3];
        port = argv[4];
        data_size = stoul(argv[5]);
        FileTransferClient client(dest_addr, port, 1400, mptcp);
        client.start_transfer(data_size);
        return 0;
    } else {
        cout << "no mode support\n" << endl;
        return -1;
    }
}
