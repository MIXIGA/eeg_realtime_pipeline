#include "ring_buffer.hpp"
#include "data_packet.hpp"
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <atomic>
#include <cstdio>

// 全局退出标志
std::atomic<bool> g_exit{false};

// 信号处理函数：捕获Ctrl+C，优雅退出
void handle_shutdown(int sig) {
    g_exit = true;
    printf("\n[Main] 收到退出信号，正在关闭流水线...\n");
}

// 线程函数声明
extern void* producer_thread_func(void*);
extern void* consumer_thread_func(void*);

int main() {
    // 注册信号处理
    signal(SIGINT, handle_shutdown);
    signal(SIGTERM, handle_shutdown);

    // 初始化环形缓冲区（20个包的容量，足够应对250Hz数据）
    RingBuffer<DataPacket> ringbuf(20);

    // 创建生产者、消费者线程
    pthread_t producer_tid, consumer_tid;
    pthread_create(&producer_tid, nullptr, producer_thread_func, &ringbuf);
    pthread_create(&consumer_tid, nullptr, consumer_thread_func, &ringbuf);

    printf("[Main] 嵌入式实时脑电数据处理框架启动成功！\n");
    printf("[Main] 采样率: 250Hz, 通道数: 8, 每包样本数: 25\n");
    printf("[Main] 按 Ctrl+C 退出程序\n");

    // 主线程等待退出信号
    while (!g_exit) {
        sleep(1);
    }

    // 等待线程安全退出
    pthread_join(producer_tid, nullptr);
    pthread_join(consumer_tid, nullptr);

    printf("[Main] 程序正常退出\n");
    return 0;
}
