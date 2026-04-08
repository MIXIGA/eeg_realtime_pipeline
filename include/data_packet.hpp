#ifndef DATA_PACKET_HPP
#define DATA_PACKET_HPP

#include <cstdint>
#include <array>

struct EEGSample {
    int32_t channels[8];
    uint64_t timestamp_us;
};

struct DataPacket {
    static constexpr size_t SAMPLES_PER_PACKET = 25;
    std::array<EEGSample, SAMPLES_PER_PACKET> samples;
    size_t valid_count = 0;
};

#endif
