#include "cpphttp/cpphttp.hpp"
#include <iostream>
#include <thread>
#include <csignal>

volatile std::sig_atomic_t g_running = true;

void signal_handler(int) {
    g_running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    cpphttp::HttpServer server;
    
    server.get("/hello", [](const cpphttp::HttpRequest& req, cpphttp::HttpResponse& res) {
        res.setBody("Hello, World!");
        res.setContentType("text/plain");
        std::cout << "GET /hello from " << req.remoteAddr << ":" << req.remotePort << std::endl;
    });

    server.get("/json", [](const cpphttp::HttpRequest& req, cpphttp::HttpResponse& res) {
        res.setBody(R"({"message":"Hello JSON","status":200})");
        res.setContentType("application/json");
        std::cout << "GET /json from " << req.remoteAddr << std::endl;
    });

    server.post("/echo", [](const cpphttp::HttpRequest& req, cpphttp::HttpResponse& res) {
        res.setBody(req.body);
        res.setContentType("text/plain");
        std::cout << "POST /echo body: " << req.body.substr(0, 100) << std::endl;
    });

    server.get("/async", [](const cpphttp::HttpRequest& req, cpphttp::HttpResponse& res) {
        std::cout << "GET /async processing..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        res.setBody("Async response completed");
    });

    server.get("/params/:id", [](const cpphttp::HttpRequest& req, cpphttp::HttpResponse& res) {
        res.setBody("Parameter id: " + req.path);
        res.setContentType("text/plain");
        std::cout << "GET /params/:id path=" << req.path << std::endl;
    });

    std::cout << "=== HTTP Server Demo ===" << std::endl;
    std::cout << "Starting server on 0.0.0.0:8080..." << std::endl;
    std::cout << "Endpoints:" << std::endl;
    std::cout << "  GET  /hello     - Simple hello world" << std::endl;
    std::cout << "  GET  /json      - JSON response" << std::endl;
    std::cout << "  POST /echo      - Echo POST body" << std::endl;
    std::cout << "  GET  /async     - Delayed response" << std::endl;
    std::cout << "  GET  /params/:id - Path parameter" << std::endl;
    std::cout << "\nPress Ctrl+C to stop\n" << std::endl;

    try {
        server.start("0.0.0.0", 8080);
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
