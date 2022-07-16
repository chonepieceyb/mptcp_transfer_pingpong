#ifndef MPTCP_ERROR_H
#define MPTCP_ERROR_H

#include<cstring>
#include<cerrno> 
#include<exception> 
#include<system_error>
#include<string>

namespace errors {

//project based exception
class Exception : public std::exception {
    using std::exception::exception;
};

class KeyboardInterrupt : public Exception {
    using Exception::Exception;
};

inline std::system_error make_system_error(int e = errno) {
    return std::system_error(e,std::system_category());
}

inline std::system_error make_system_error(const char* what_msg, int e = errno) {
    return std::system_error(e,std::system_category(), what_msg);
}

}
#endif
