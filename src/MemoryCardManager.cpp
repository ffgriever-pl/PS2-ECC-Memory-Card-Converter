#include "MemoryCardManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <stdexcept>

void MemoryCardManager::calcChecksum(std::span<const std::uint8_t> data, std::span<std::uint8_t> checksum) {
    if (data.size() < BLOCK_SIZE || checksum.size() < CHECKSUM_SIZE) {
        throw std::invalid_argument("Buffer sizes too small for checksum calculation");
    }
    
    int first = 0, second = 0, third = 0;
    
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        std::uint8_t tableItem = XORTABLE[data[i]];
        first ^= tableItem;
        
        if (tableItem & 0x80) {
            second ^= ~i;
            third ^= i;
        }
    }
    
    checksum[0] = static_cast<std::uint8_t>(~first & 0x77);
    checksum[1] = static_cast<std::uint8_t>(~second & 0x7F);
    checksum[2] = static_cast<std::uint8_t>(~third & 0x7F);
}

bool MemoryCardManager::convertMemoryCard(const std::string& inputFile, const std::string& outputFile, 
                                          ConversionMode mode) {
    if (mode == ConversionMode::BinaryToEcc) {
        return convertBinToEcc(inputFile, outputFile);
    } else {
        return convertEccToBin(inputFile, outputFile);
    }
}

bool MemoryCardManager::convertBinToEcc(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile, std::ios::binary);
    if (!input) {
        std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
        return false;
    }
    
    std::ofstream output(outputFile, std::ios::binary);
    if (!output) {
        std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
        return false;
    }
    
    std::vector<std::uint8_t> binBuffer(BINARY_BLOCK_SIZE);
    std::vector<std::uint8_t> eccBuffer(ECC_BLOCK_SIZE, 0);
    
    while (input.read(reinterpret_cast<char*>(binBuffer.data()), binBuffer.size())) {
        // Copy binary data to the ECC buffer
        std::copy(binBuffer.begin(), binBuffer.end(), eccBuffer.begin());
        
        // Calculate checksums for each 128-byte block
        for (size_t i = 0; i < BINARY_BLOCK_SIZE; i += BLOCK_SIZE) {
            calcChecksum(
                std::span(binBuffer).subspan(i, BLOCK_SIZE),
                std::span(eccBuffer).subspan(BINARY_BLOCK_SIZE + (i/BLOCK_SIZE) * CHECKSUM_SIZE, CHECKSUM_SIZE)
            );
        }
        
        output.write(reinterpret_cast<char*>(eccBuffer.data()), eccBuffer.size());
    }
    
    return true;
}

bool MemoryCardManager::convertEccToBin(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile, std::ios::binary);
    if (!input) {
        std::cerr << "Error: Cannot open input file: " << inputFile << std::endl;
        return false;
    }
    
    std::ofstream output(outputFile, std::ios::binary);
    if (!output) {
        std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
        return false;
    }
    
    std::vector<std::uint8_t> eccBuffer(ECC_BLOCK_SIZE);
    
    while (input.read(reinterpret_cast<char*>(eccBuffer.data()), eccBuffer.size())) {
        // Write only the first 512 bytes (without checksums)
        output.write(reinterpret_cast<char*>(eccBuffer.data()), BINARY_BLOCK_SIZE);
    }
    
    return true;
}