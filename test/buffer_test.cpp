#include <iostream>
#include "buffer.hpp"

int main() {
    cpphttp::Buffer buf;
    buf.write("hello", 5);
    auto view = buf.read(3);
    std::cout << "read: " << std::string(view.data(), view.size()) << std::endl;
    return 0;
}
