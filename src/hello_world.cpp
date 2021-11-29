#include <iostream>

#include "hello_world_config.h"

using namespace std;

int main() {
    cout << "hello world" << endl;
    cout << "version: " << hello_world_VERSION_MAJOR << "." << hello_world_VERSION_MINOR << endl;
    cout << "test c++11" << stod("11234") << endl;
}
