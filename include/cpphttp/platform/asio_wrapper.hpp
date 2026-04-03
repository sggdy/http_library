#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <functional>

namespace cpphttp {

class AsioWrapper {
public:
    AsioWrapper();
    ~AsioWrapper();

    boost::asio::io_context& context();
    void run();
    void stop();
    std::future<void> runAsync();

    boost::asio::ip::tcp::socket createSocket();
    boost::asio::ip::tcp::acceptor createAcceptor(const boost::asio::ip::tcp::endpoint& endpoint);

    void post(std::function<void()> handler);
    void dispatch(std::function<void()> handler);

    template<typename Duration, typename Callback>
    void runAfter(const Duration& duration, Callback&& callback) {
        auto timer = std::make_shared<boost::asio::steady_timer>(context());
        timer->expires_after(duration);
        timer->async_wait([timer, cb = std::forward<Callback>(callback)](const boost::system::error_code& ec) {
            if (!ec) cb();
        });
    }

    AsioWrapper(const AsioWrapper&) = delete;
    AsioWrapper& operator=(const AsioWrapper&) = delete;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
