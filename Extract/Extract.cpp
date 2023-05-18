#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <map>
#include <chrono>
#include "Extract.h"

void extract_pak(const std::string& inpath, const std::string& outpath = "") {
    auto read_int = [](std::ifstream& file, size_t bytes) {
        uint64_t value = 0;
        file.read(reinterpret_cast<char*>(&value), bytes);
        return value;
    };

    std::ifstream file(inpath, std::ios::binary);
    if (!file) {
        std::cout << "Failed to open input file!" << std::endl;
        return;
    }

    const std::string signature("Spark Pack (C)2004 Spark Unlimited, Inc. Author Jim Schuler.");
    std::string file_signature(signature.size(), ' ');
    file.read(&file_signature[0], signature.size());

    if (file_signature != signature) {
        std::cout << "Not a valid Spark Pack file!" << std::endl;
        return;
    }

    file.seekg(0x78);
    uint32_t version = read_int(file, 0x4);
    if (version != 0x1) {
        std::cout << "Unsupported Spark Pack version!" << std::endl;
        return;
    }

    std::string output_dir = outpath.empty() ? std::filesystem::path(inpath).parent_path().string() : std::filesystem::absolute(outpath).string();

    uint32_t num_files = read_int(file, 0x4);
    for (uint32_t idx = 0; idx < num_files; ++idx) {
        file.seekg(0x80 + idx * 0x10);
        uint32_t size = read_int(file, 0x4);
        uint32_t offset = read_int(file, 0x4);
        uint64_t filename_hash = read_int(file, 0x8);

        std::string filename = HASH_DICT.count(filename_hash) ? HASH_DICT.at(filename_hash) : std::filesystem::path("__hashed").append(std::to_string(filename_hash)).string();

        std::string path = std::filesystem::path(output_dir).append(filename).string();
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());

        std::ofstream out(path, std::ios::binary);
        if (!out) {
            std::cout << "Failed to create output file: " << path << std::endl;
            continue;
        }

        std::cout << "Writing " << path << std::endl;
        file.seekg(offset);
        std::vector<char> buffer(size);
        file.read(buffer.data(), size);
        out.write(buffer.data(), size);
        out.close();
    }
    file.close();
}

int main() {
    std::string input_path;
    std::cout << "Enter the input file path: ";
    std::getline(std::cin, input_path);

    std::string output_path;
    std::cout << "Enter the output directory path (leave empty for the same directory): ";
    std::getline(std::cin, output_path);
    auto start = std::chrono::high_resolution_clock::now();
    extract_pak(input_path, output_path);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken to extract files:" << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "us" << std::endl;
    system("pause");
    return 0;
}
