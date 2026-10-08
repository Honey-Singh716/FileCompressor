#ifndef FREQUENCY_TABLE_H
#define FREQUENCY_TABLE_H

#include <array>
#include <cstdint>
#include <string>

class FrequencyTable {
private:
    std::array<uint64_t, 256> frequencies{};
    uint64_t totalBytes = 0;

public:
    void buildFromFile(
        const std::string& fileName
    );

    const std::array<uint64_t, 256>&
    getFrequencies() const;

    uint64_t getTotalBytes() const;
};

#endif