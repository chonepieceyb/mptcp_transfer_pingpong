#ifndef MPTCP_ERROR_H
#define MPTCP_ERROR_H

#include <string.h>
#include <exception>
#include <errno.h>
#include <string>

namespace net {

class LinuxError : public std::exception {
public:
    explicit LinuxError(int err) : _err(err) {}

    virtual ~LinuxError() = default;

    virtual const char* what() const noexcept override {
        return strerror(_err);
    }

    int err() {return _err;}
private:
    int _err;
};

class Exception : public std::exception {
public:
    explicit Exception(const std::string &err) : _err(err) {}

    virtual ~Exception() = default;

    virtual const char* what() const noexcept override {
        return _err.data();
    }

private:
    std::string _err;
};

}
#endif
