#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include "cpphttp/error.hpp"

namespace cpphttp {

class Connection : public std::enable_shared_from_this<Connection> {
public:
    using ReadCallback = std::function<void(const uint8_t*, size_t, Error)>;
    using WriteCallback = std::function<void(Error)>;
    using CloseCallback = std::function<void()>;

    Connection(boost::asio::ip::tcp::socket socket);
    ~Connection();

    void start();
    void close();

    void readSome(size_t maxLen, ReadCallback callback);
    void readExact(size_t len, ReadCallback callback);
    void write(const uint8_t* data, size_t len, WriteCallback callback);
    void write(const std::string& data, WriteCallback callback);

    void setCloseCallback(CloseCallback callback);
    boost::asio::ip::tcp::socket& socket();

    std::string remoteAddr() const;
    uint16_t remotePort() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
