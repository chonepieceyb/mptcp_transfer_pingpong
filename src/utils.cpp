#include "utils.h" 

#include<cassert>
#include<cstring>

extern "C" {
#include<malloc.h> 
#include<signal.h>
} 

namespace utils {

BufUPtr create_buffer(std::size_t size, bool init = true) {
    assert((size > 0)&&"create_buffer buffer len must > 0");
    void *ptr = std::malloc(size);
    if (ptr == nullptr) {
        throw errors::make_system_error("create_buffer failed");
    }
    if (init) {
        std::memset(ptr, 0, size);
    }
    return BufUPtr(ptr);
}

BufUPtr create_align_buffer(std::size_t alignment, std::size_t size, bool init = true) {
    assert((size > 0)&&"create_align_buffer buffer len must > 0");
    void *ptr = memalign(alignment, size);
    if (ptr == nullptr) {
        throw errors::make_system_error("create_align_buffer failed");
    }
    if (init) {
        std::memset(ptr, 0, size);
    }
    return BufUPtr(ptr);
}

std::pair<bool, std::string> run_cmd(const std::string &cmd) {
    sighandler_t oldsig = signal(SIGCHLD, SIG_DFL);
    auto pipe = popen(cmd.data(), "r");
    char buf[1024];
    std::string output;
    while (std::fgets(buf, sizeof(buf) / sizeof(char), pipe) != nullptr) {
        output += buf;
    }

    auto status = pclose(pipe);
    auto ret = false;
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        ret = true;
    }
    signal(SIGCHLD, oldsig);

    return std::make_pair(ret, output);
}

}

