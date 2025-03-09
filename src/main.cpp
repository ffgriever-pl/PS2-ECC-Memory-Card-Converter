#include "MemoryCardManager.h"
#include <iostream>
#include <string>

/**
 * @brief Display program header information
 */
static void printHeader() {
    std::cout << "\nECC <-> Non ECC Memory Card Converter v1.0.1\n";
}

/**
 * @brief Display available command-line options
 */
static void printOptions() {
    std::cout << "Use: convmcecc [options] inputFile outputFile\n"
              << "\nOptions:\n"
              << "    -2e     convert file into ECC format\n"
              << "    -2b     convert file into bin format\n"
              << "    -h      show this info\n";
}

int main(int argc, char* argv[]) {
    enum class Mode {
        Unknown,
        BinaryToEcc,
        EccToBinary
    };

    Mode mode = Mode::Unknown;
    std::string inputFile;
    std::string outputFile;
    
    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-2e") {
            mode = Mode::BinaryToEcc;
        } else if (arg == "-2b") {
            mode = Mode::EccToBinary;
        } else if (arg == "-h") {
            printHeader();
            printOptions();
            return 0;
        } else if (arg[0] == '-') {
            printHeader();
            std::cerr << "Bad argument: " << arg << "\n\n";
            printOptions();
            return 1;
        } else if (inputFile.empty()) {
            inputFile = arg;
        } else if (outputFile.empty()) {
            outputFile = arg;
        }
    }
    
    // Check if we have all required arguments
    if (mode == Mode::Unknown || inputFile.empty() || outputFile.empty()) {
        printHeader();
        if (mode == Mode::Unknown) {
            std::cerr << "Conversion type was not specified\n\n";
        } else {
            std::cerr << "Not enough arguments\n\n";
        }
        printOptions();
        return 1;
    }
    
    // Perform the conversion
    bool success = false;
    try {
        MemoryCardManager::ConversionMode convMode = (mode == Mode::BinaryToEcc) 
            ? MemoryCardManager::ConversionMode::BinaryToEcc 
            : MemoryCardManager::ConversionMode::EccToBinary;
            
        success = MemoryCardManager::convertMemoryCard(inputFile, outputFile, convMode);
    } catch (const std::exception& e) {
        std::cerr << "Error during conversion: " << e.what() << std::endl;
        return 1;
    }
    
    return success ? 0 : 1;
}