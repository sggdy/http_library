#pragma once

#include <memory>
#include <vector>
#include <mutex>
#include <functional>

namespace cpphttp {

template<typename T>
class ObjectPool {
public:
    using Factory = std::function<std::unique_ptr<T>()>;

    explicit ObjectPool(Factory factory, size_t initialSize = 0)
        : factory_(std::move(factory)) {
        available_.reserve(initialSize);
        for (size_t i = 0; i < initialSize; ++i) {
            available_.push_back(factory_());
        }
    }

    std::unique_ptr<T> acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (available_.empty()) {
            return factory_();
        }
        auto obj = std::move(available_.back());
        available_.pop_back();
        return obj;
    }

    void release(std::unique_ptr<T> obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        available_.push_back(std::move(obj));
    }

    size_t available() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return available_.size();
    }

private:
    Factory factory_;
    std::vector<std::unique_ptr<T>> available_;
    mutable std::mutex mutex_;
};

template<typename T>
class SharedObjectPool {
public:
    using Factory = std::function<std::shared_ptr<T>()>;

    explicit SharedObjectPool(Factory factory, size_t initialSize = 0)
        : factory_(std::move(factory)) {
        available_.reserve(initialSize);
        for (size_t i = 0; i < initialSize; ++i) {
            available_.push_back(factory_());
        }
    }

    std::shared_ptr<T> acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (available_.empty()) {
            return factory_();
        }
        auto obj = available_.back();
        available_.pop_back();
        return obj;
    }

    void release(std::shared_ptr<T> obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        available_.push_back(std::move(obj));
    }

private:
    Factory factory_;
    std::vector<std::shared_ptr<T>> available_;
    mutable std::mutex mutex_;
};

}
