#include "ring_buffer.hpp"
#include "data_packet.hpp"
#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <atomic>

extern std::atomic<bool> g_exit;

// 高精度获取当前时间（微秒）
uint64_t get_current_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

// 生产者线程：模拟250Hz 8通道脑电数据生成
void* producer_thread_func(void* arg) {
    RingBuffer<DataPacket>* ringbuf = static_cast<RingBuffer<DataPacket>*>(arg);
    const int sampling_rate = 250; // Hz
    const int64_t interval_us = 1'000'000 / sampling_rate; // 4000us per sample
    const int64_t packet_interval_us = interval_us * DataPacket::SAMPLES_PER_PACKET; // 100ms per packet

    while (!g_exit) {
        DataPacket packet;
        packet.valid_count = DataPacket::SAMPLES_PER_PACKET;
        uint64_t now = get_current_time_us();

        // 生成25个样本（100ms数据）
        for (size_t i = 0; i < packet.valid_count; ++i) {
            double t = (now + i * interval_us) / 1e6;
            for (int ch = 0; ch < 8; ++ch) {
                // 合成信号：10Hz Alpha波 + 50Hz工频噪声 + 随机噪声
                double alpha = 1000 * sin(2 * M_PI * 10.0 * t);
                double line_noise = 50 * sin(2 * M_PI * 50.0 * t);
                double random_noise = (rand() % 200) - 100;
                packet.samples[i].channels[ch] = static_cast<int32_t>(alpha + line_noise + random_noise);
            }
            packet.samples[i].timestamp_us = now + i * interval_us;
        }

        // 非阻塞推送，缓冲区满则等待
        while (!ringbuf->try_push(packet) && !g_exit) {
            usleep(1000);
        }

        // 精确等待到下一个包的发送时间
        uint64_t next_time = now + packet_interval_us;
        while (get_current_time_us() < next_time && !g_exit) {
            usleep(100);
        }
    }
    return nullptr;
}
