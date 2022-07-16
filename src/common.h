#ifndef MPTCP_COMMON_H
#define MPTCP_COMMON_H

#include <cstdint>
#include <string>
#include <functional>
#include <sstream> 
#include "errors.h"

namespace net {

constexpr std::uint16_t DEFAULT_PORT = 60000;
constexpr std::uint16_t SEND_BUFFER = 1500;
constexpr std::uint16_t RECV_BUFFER = 1500;


struct TransferRes {
    TransferRes(uint64_t sk, uint64_t rk, double time) : ksend_data(sk), krecv_data(rk), time_micro(time) {
        send_rate = static_cast<double>(1000 * 1000 * ksend_data / (time_micro + 1e-9));
        recv_rate = static_cast<double>(1000 * 1000 * krecv_data / (time_micro + 1e-9));
    }
    uint64_t ksend_data;
    uint64_t krecv_data;
    double time_micro; 
    double send_rate;   //KB/s
    double recv_rate;   //KB/s

    std::string str() {
        if (_str.empty()) {
            std::ostringstream s;
            s << ksend_data << " " << time_micro << " " << send_rate << " ";
            s << krecv_data << " " << time_micro << " " << recv_rate;
            _str = s.str();
        }
        return _str;
    }
    std::string _str;
};

struct MsgTransferRes {
    MsgTransferRes(uint32_t s_msg_num, double time) : send_msg_num(s_msg_num), time_micro(time) {
        send_rate = static_cast<double>(1000 * 1000 * send_msg_num / (time_micro + 1e-9));
    }
    uint64_t send_msg_num;
    double time_micro; 
    double send_rate;   //num-of-msg/s

    std::string str() {
        if (_str.empty()) {
            std::ostringstream s;
            s << send_msg_num << " " << time_micro << " " << send_rate << " ";
            _str = s.str();
        }
        return _str;
    }
    std::string _str;
};

}

#endif 
