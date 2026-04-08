#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <vector>
#include <atomic>
#include <algorithm>

template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity)
        : capacity_(capacity), buffer_(capacity) {}

    bool try_push(const T& item) {
        size_t h = head_.load(std::memory_order_relaxed);
        size_t t = tail_.load(std::memory_order_acquire);
        size_t next_h = (h + 1) % capacity_;
        if (next_h == t) return false;
        buffer_[h] = item;
        head_.store(next_h, std::memory_order_release);
        return true;
    }

    bool try_pop(T& item) {
        size_t t = tail_.load(std::memory_order_relaxed);
        size_t h = head_.load(std::memory_order_acquire);
        if (t == h) return false;
        item = buffer_[t];
        tail_.store((t + 1) % capacity_, std::memory_order_release);
        return true;
    }

    size_t size() const {
        size_t h = head_.load(std::memory_order_relaxed);
        size_t t = tail_.load(std::memory_order_relaxed);
        return (h >= t) ? (h - t) : (capacity_ - t + h);
    }

    size_t capacity() const { return capacity_; }

private:
    std::vector<T> buffer_;
    const size_t capacity_;
    std::atomic<size_t> head_{0};
    std::atomic<size_t> tail_{0};
};

#endif
