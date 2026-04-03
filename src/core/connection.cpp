#include "cpphttp/core/connection.hpp"

namespace cpphttp {

struct Connection::Impl {
    boost::asio::ip::tcp::socket socket;
    CloseCallback closeCallback;
    bool closed = false;

    Impl(boost::asio::ip::tcp::socket s) : socket(std::move(s)) {}
};

Connection::Connection(boost::asio::ip::tcp::socket socket)
    : impl_(std::make_unique<Impl>(std::move(socket))) {}

Connection::~Connection() {
    close();
}

void Connection::start() {}

void Connection::close() {
    if (impl_->closed) return;
    impl_->closed = true;
    boost::system::error_code ec;
    impl_->socket.close(ec);
    if (impl_->closeCallback) {
        impl_->closeCallback();
    }
}

void Connection::readSome(size_t maxLen, ReadCallback callback) {
    auto buffer = std::make_shared<std::vector<uint8_t>>(maxLen);
    impl_->socket.async_read_some(
        boost::asio::buffer(*buffer),
        [this, buffer, callback](boost::system::error_code ec, size_t bytes) {
            if (ec) {
                callback(nullptr, 0, Error::fromSystem(ec));
                close();
                return;
            }
            callback(buffer->data(), bytes, Error::success());
        }
    );
}

void Connection::readExact(size_t len, ReadCallback callback) {
    auto buffer = std::make_shared<std::vector<uint8_t>>(len);
    boost::asio::async_read(
        impl_->socket,
        boost::asio::buffer(*buffer),
        [this, buffer, callback](boost::system::error_code ec, size_t bytes) {
            if (ec) {
                callback(nullptr, 0, Error::fromSystem(ec));
                close();
                return;
            }
            callback(buffer->data(), bytes, Error::success());
        }
    );
}

void Connection::write(const uint8_t* data, size_t len, WriteCallback callback) {
    boost::asio::async_write(
        impl_->socket,
        boost::asio::buffer(data, len),
        [this, callback](boost::system::error_code ec, size_t) {
            if (ec) {
                callback(Error::fromSystem(ec));
                close();
                return;
            }
            callback(Error::success());
        }
    );
}

void Connection::write(const std::string& data, WriteCallback callback) {
    write(reinterpret_cast<const uint8_t*>(data.data()), data.size(), std::move(callback));
}

void Connection::setCloseCallback(CloseCallback callback) {
    impl_->closeCallback = std::move(callback);
}

boost::asio::ip::tcp::socket& Connection::socket() {
    return impl_->socket;
}

std::string Connection::remoteAddr() const {
    return impl_->socket.remote_endpoint().address().to_string();
}

uint16_t Connection::remotePort() const {
    return impl_->socket.remote_endpoint().port();
}

}
