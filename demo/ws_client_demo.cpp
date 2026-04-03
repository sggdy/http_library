#include "cpphttp/cpphttp.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

int main() {
    std::cout << "=== WebSocket Client Demo ===" << std::endl;

    cpphttp::WebSocketClient client;

    std::atomic<bool> connected{false};
    std::atomic<bool> done{false};

    client.onConnect([&connected]() {
        std::cout << "[+] Connected to server!" << std::endl;
        connected = true;
    });

    client.onMessage([](const std::string& msg) {
        std::cout << "[msg] Received: " << msg.substr(0, 100) << std::endl;
    });

    client.onBinary([](const std::vector<uint8_t>& data) {
        std::cout << "[bin] Received: " << data.size() << " bytes" << std::endl;
    });

    client.onClose([&done](uint16_t code) {
        std::cout << "[-] Connection closed: code=" << code << std::endl;
        done = true;
    });

    client.onError([](const cpphttp::Error& err) {
        std::cout << "[!] Error: " << err.toString() << std::endl;
    });

    std::cout << "\nConnecting to ws://127.0.0.1:8081..." << std::endl;
    
    try {
        client.connect("ws://127.0.0.1:8081");
    } catch (const std::exception& e) {
        std::cerr << "Connect exception: " << e.what() << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (!connected.load()) {
        std::cout << "Failed to connect. Is the server running?" << std::endl;
        return 1;
    }

    std::cout << "\n[1] Send text messages..." << std::endl;
    client.send("Hello, WebSocket!");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    client.send("Second message");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    client.send("Third message");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "\n[2] Send binary data..." << std::endl;
    uint8_t binaryData[] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x42, 0x69, 0x6e, 0x61, 0x72, 0x79};
    client.send(binaryData, sizeof(binaryData));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "\n[3] Rapid messages test..." << std::endl;
    for (int i = 0; i < 10; ++i) {
        client.send("Message #" + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "\n[4] Waiting for responses..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "\n[5] Close connection..." << std::endl;
    client.close(1000);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "\n=== Demo Complete ===" << std::endl;
    return 0;
}
