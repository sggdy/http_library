#include "cpphttp/cpphttp.hpp"
#include <iostream>
#include <thread>
#include <csignal>
#include <atomic>

volatile std::sig_atomic_t g_running = true;

void signal_handler(int) {
    g_running = false;
}

int main() {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    cpphttp::WebSocketServer server;
    
    std::atomic<uint64_t> connectionCount{0};
    
    server.onConnect([&connectionCount](cpphttp::WsSession* session) {
        connectionCount++;
        std::cout << "[+] Client connected: id=" << session->id() 
                  << " addr=" << session->remoteAddr() 
                  << " port=" << session->remotePort() << std::endl;
        std::cout << "    Total connections: " << connectionCount.load() << std::endl;
    });

    server.onMessage([](cpphttp::WsSession* session, const std::string& msg) {
        std::cout << "[msg] id=" << session->id() 
                  << " len=" << msg.size() << " content: " << msg.substr(0, 100) << std::endl;
        session->send("Echo: " + msg);
    });

    server.onBinary([](cpphttp::WsSession* session, const std::vector<uint8_t>& data) {
        std::cout << "[bin] id=" << session->id() 
                  << " len=" << data.size() << std::endl;
        session->send(data.data(), data.size());
    });

    server.onClose([&connectionCount](cpphttp::WsSession* session, uint16_t code) {
        connectionCount--;
        std::cout << "[-] Client disconnected: id=" << session->id() 
                  << " code=" << code 
                  << " remaining: " << connectionCount.load() << std::endl;
    });

    server.onError([](cpphttp::WsSession* session, const cpphttp::Error& err) {
        std::cout << "[!] Error on session " << session->id() 
                  << ": " << err.toString() << std::endl;
    });

    std::cout << "=== WebSocket Server Demo ===" << std::endl;
    std::cout << "Starting WebSocket server on 0.0.0.0:8081..." << std::endl;
    std::cout << "Endpoints:" << std::endl;
    std::cout << "  ws://127.0.0.1:8081 - WebSocket endpoint" << std::endl;
    std::cout << "\nFeatures:" << std::endl;
        std::cout << "    - Echo messages back to client" << std::endl;
    std::cout << "  - Binary data echo" << std::endl;
    std::cout << "  - Connection counting" << std::endl;
    std::cout << "\nPress Ctrl+C to stop\n" << std::endl;

    try {
        server.start(8081);
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
