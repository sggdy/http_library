#include "core/session.hpp"
#include <atomic>

namespace cpphttp {

struct Session::Impl {
    uint64_t id;
    std::string remoteAddr;
    uint16_t remotePort;
    std::atomic<int> refCount{1};

    Impl(uint64_t id, const std::string& addr, uint16_t port)
        : id(id), remoteAddr(addr), remotePort(port) {}
};

Session::Session(uint64_t id, const std::string& remoteAddr, uint16_t remotePort)
    : impl_(std::make_unique<Impl>(id, remoteAddr, remotePort)) {}

Session::~Session() = default;

uint64_t Session::id() const { return impl_->id; }
std::string Session::remoteAddr() const { return impl_->remoteAddr; }
uint16_t Session::remotePort() const { return impl_->remotePort; }

void Session::addRef() { impl_->refCount.fetch_add(1); }
void Session::release() { 
    if (impl_->refCount.fetch_sub(1) == 1) {
        delete this;
    }
}

}
