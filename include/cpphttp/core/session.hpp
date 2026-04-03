#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace cpphttp {

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(uint64_t id, const std::string& remoteAddr, uint16_t remotePort);
    ~Session();

    uint64_t id() const;
    std::string remoteAddr() const;
    uint16_t remotePort() const;

    virtual void send(const std::string& message) = 0;
    virtual void send(const uint8_t* data, size_t len) = 0;
    virtual void close(uint16_t code = 1000) = 0;
    virtual void keepAlive() = 0;

    void addRef();
    void release();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
