#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <string_view>

namespace cpphttp {

class Buffer {
public:
    Buffer();
    explicit Buffer(size_t capacity);

    void write(const uint8_t* data, size_t len);
    void write(std::string_view data);

    std::string_view read(size_t len);
    std::string_view peek() const;
    void commitRead(size_t len);

    void retrieveAll();
    size_t size() const { return size_; }
    size_t capacity() const { return buffer_.size(); }
    void ensureWritable(size_t minSize);

    void swap(Buffer& other) noexcept;

private:
    std::vector<uint8_t> buffer_;
    size_t readPos_ = 0;
    size_t writePos_ = 0;
    size_t size_ = 0;
};

inline Buffer::Buffer() : buffer_(1024) {}

inline Buffer::Buffer(size_t capacity) : buffer_(capacity) {}

inline std::string_view Buffer::peek() const {
    return std::string_view(reinterpret_cast<const char*>(buffer_.data() + readPos_), size_);
}

inline void Buffer::retrieveAll() {
    readPos_ = 0;
    writePos_ = 0;
    size_ = 0;
}

inline void Buffer::swap(Buffer& other) noexcept {
    buffer_.swap(other.buffer_);
    std::swap(readPos_, other.readPos_);
    std::swap(writePos_, other.writePos_);
    std::swap(size_, other.size_);
}

}
