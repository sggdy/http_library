#include "platform/asio_wrapper.hpp"

namespace cpphttp {

struct AsioWrapper::Impl {
    boost::asio::io_context ctx;
    std::vector<std::thread> threads;
};

AsioWrapper::AsioWrapper() : impl_(std::make_unique<Impl>()) {}

AsioWrapper::~AsioWrapper() {
    stop();
}

boost::asio::io_context& AsioWrapper::context() {
    return impl_->ctx;
}

void AsioWrapper::run() {
    impl_->ctx.run();
}

void AsioWrapper::stop() {
    impl_->ctx.stop();
    for (auto& t : impl_->threads) {
        if (t.joinable()) t.join();
    }
    impl_->threads.clear();
}

std::future<void> AsioWrapper::runAsync() {
    return std::async(std::launch::async, [this] { run(); });
}

boost::asio::ip::tcp::socket AsioWrapper::createSocket() {
    return boost::asio::ip::tcp::socket(impl_->ctx);
}

boost::asio::ip::tcp::acceptor AsioWrapper::createAcceptor(const boost::asio::ip::tcp::endpoint& endpoint) {
    return boost::asio::ip::tcp::acceptor(impl_->ctx, endpoint);
}

void AsioWrapper::post(std::function<void()> handler) {
    boost::asio::post(impl_->ctx, handler);
}

void AsioWrapper::dispatch(std::function<void()> handler) {
    boost::asio::dispatch(impl_->ctx, handler);
}

}
