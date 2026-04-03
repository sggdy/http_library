#include "cpphttp/cpphttp.hpp"
#include <iostream>
#include <future>
#include <chrono>

void printResponse(const cpphttp::HttpResponse& resp, const std::string& operation) {
    std::cout << "\n--- " << operation << " ---" << std::endl;
    std::cout << "Status: " << resp.statusCode << " " << resp.statusMessage << std::endl;
    std::cout << "Body: " << resp.body.substr(0, std::min(resp.body.size(), size_t(200))) << std::endl;
}

int main() {
    std::cout << "=== HTTP Client Demo ===" << std::endl;

    cpphttp::HttpClient client("http://127.0.0.1:8080");

    std::cout << "\n[1] Sync GET /hello" << std::endl;
    {
        auto resp = client.get("/hello");
        printResponse(resp, "Sync GET /hello");
    }

    std::cout << "\n[2] Async GET /json - launching concurrent requests..." << std::endl;
    {
        auto f1 = client.getAsync("/json");
        auto f2 = client.getAsync("/hello");
        auto f3 = client.getAsync("/json");
        
        auto resp1 = f1.get();
        auto resp2 = f2.get();
        auto resp3 = f3.get();
        
        printResponse(resp1, "Async GET /json #1");
        printResponse(resp2, "Async GET /hello #2");
        printResponse(resp3, "Async GET /json #3");
    }

    std::cout << "\n[3] Sync POST /echo" << std::endl;
    {
        auto resp = client.post("/echo", "Hello from POST request!");
        printResponse(resp, "Sync POST /echo");
    }

    std::cout << "\n[4] Async POST /echo with multiple concurrent requests" << std::endl;
    {
        std::vector<std::future<cpphttp::HttpResponse>> futures;
        for (int i = 0; i < 5; ++i) {
            futures.push_back(client.postAsync("/echo", "Request #" + std::to_string(i)));
        }
        
        for (int i = 0; i < 5; ++i) {
            auto resp = futures[i].get();
            printResponse(resp, "Async POST /echo #" + std::to_string(i));
        }
    }

    std::cout << "\n[5] Sync GET /async (delayed response)" << std::endl;
    {
        auto start = std::chrono::steady_clock::now();
        auto resp = client.get("/async");
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        printResponse(resp, "Sync GET /async");
        std::cout << "Time elapsed: " << ms << "ms" << std::endl;
    }

    std::cout << "\n[6] Concurrent performance test" << std::endl;
    {
        const int count = 100;
        auto start = std::chrono::steady_clock::now();
        
        std::vector<std::future<cpphttp::HttpResponse>> futures;
        for (int i = 0; i < count; ++i) {
            futures.push_back(client.getAsync("/hello"));
        }
        
        int success = 0;
        for (auto& f : futures) {
            auto resp = f.get();
            if (resp.statusCode == 200) success++;
        }
        
        auto end = std::chrono::steady_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        std::cout << "Completed " << success << "/" << count << " requests in " << ms << "ms" << std::endl;
        std::cout << "Average: " << (ms * 1000.0 / count) << "ms per request" << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;
    return 0;
}
