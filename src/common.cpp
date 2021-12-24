#include "common.h"

namespace net {
 
TrafficMode string_to_tm(const std::string &tm) {
    if (tm == "C_S") {
        return TrafficMode::C_TO_S;
    } else if (tm == "S_C") {
        return TrafficMode::S_TO_C;
    } else if (tm == "BIO") {
        return TrafficMode::BIO;
    } else {
        throw Exception("unkonwen traffic mode: " + tm);
    }
}

std::string tm_to_string(TrafficMode tm) {
    switch(tm) {
        case TrafficMode::C_TO_S:
            return "C_TO_S";
        
        case TrafficMode::S_TO_C:
            return "S_TO_C";

        case TrafficMode::BIO:
            return "BIO";

        default:
            throw Exception("failed to parse traffic mode to string");
    }
}

}
