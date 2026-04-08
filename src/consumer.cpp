#include "ring_buffer.hpp"
#include "data_packet.hpp"
#include <pthread.h>
#include <sched.h>
#include <cstdio>
#include <atomic>

extern std::atomic<bool> g_exit;

// 消费者线程：读取数据并打印日志
void* consumer_thread_func(void* arg) {
    RingBuffer<DataPacket>* ringbuf = static_cast<RingBuffer<DataPacket>*>(arg);

    while (!g_exit) {
        DataPacket packet;
        if (ringbuf->try_pop(packet)) {
            // 打印数据包信息（后续可替换为滤波/FFT处理）
            printf("[Consumer] 收到数据包，有效样本数: %zu, 首帧时间戳: %lu us\n",
                   packet.valid_count, packet.samples[0].timestamp_us);
        } else {
            // 缓冲区空，让出CPU
            sched_yield();
        }
    }
    return nullptr;
}
