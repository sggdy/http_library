#include "buffer.hpp"

namespace cpphttp {

void Buffer::write(const uint8_t* data, size_t len) {
    ensureWritable(len);
    std::copy(data, data + len, buffer_.begin() + writePos_);
    writePos_ += len;
    size_ += len;
}

void Buffer::write(std::string_view data) {
    write(reinterpret_cast<const uint8_t*>(data.data()), data.size());
}

std::string_view Buffer::read(size_t len) {
    len = std::min(len, size_);
    auto result = std::string_view(reinterpret_cast<const char*>(buffer_.data() + readPos_), len);
    return result;
}

void Buffer::commitRead(size_t len) {
    len = std::min(len, size_);
    readPos_ += len;
    size_ -= len;
    if (readPos_ >= buffer_.size() / 2) {
        std::copy(buffer_.begin() + readPos_, buffer_.begin() + writePos_, buffer_.begin());
        writePos_ -= readPos_;
        readPos_ = 0;
    }
}

void Buffer::ensureWritable(size_t minSize) {
    if (writePos_ + minSize <= buffer_.size()) return;
    size_t newSize = std::max(buffer_.size() * 2, writePos_ + minSize);
    buffer_.resize(newSize);
}

}
