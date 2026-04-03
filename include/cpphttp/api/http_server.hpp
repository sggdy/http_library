#pragma once

#include "cpphttp/api/http_types.hpp"
#include "cpphttp/api/ws_server.hpp"
#include <boost/asio.hpp>
#include <memory>

namespace cpphttp {

class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    HttpServer& route(HttpMethod method, const std::string& path, Handler handler);
    HttpServer& get(const std::string& path, Handler handler);
    HttpServer& post(const std::string& path, Handler handler);
    HttpServer& put(const std::string& path, Handler handler);
    HttpServer& delete_(const std::string& path, Handler handler);

    HttpServer& setNumThreads(size_t n);
    HttpServer& setMaxConnections(size_t max);
    HttpServer& setRequestTimeout(std::chrono::seconds timeout);
    HttpServer& setConfig(const Config& config);

    HttpServer& onWsConnect(std::function<void(WsSession*)> callback);

    void start(const std::string& host, uint16_t port);
    void stop();
    std::future<void> startAsync(const std::string& host, uint16_t port);
    std::future<void> stopAsync();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer(HttpServer&&) noexcept;
    HttpServer& operator=(HttpServer&&) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    void handleConnection(boost::asio::ip::tcp::socket socket);
};

}
