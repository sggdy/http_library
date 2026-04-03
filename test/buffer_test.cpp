#include <iostream>
#include "cpphttp/buffer.hpp"

int main() {
    cpphttp::Buffer buf;
    buf.write(reinterpret_cast<const uint8_t*>("hello"), 5);
    auto view = buf.read(3);
    std::cout << "read: " << std::string(view.data(), view.size()) << std::endl;
    return 0;
}
