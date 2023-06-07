#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
uint64_t spark_hash(std::string str) {
    uint64_t hash = 0x84222325CBF29CE4;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    std::replace(str.begin(), str.end(), '/', '\\');
    size_t semicolonIndex = str.find(';');
    if (semicolonIndex != std::string::npos) {
        str = str.substr(0, semicolonIndex);
    }
    for (char chr : str) {
        hash = static_cast<uint64_t>(chr) ^ (hash << 40) + (hash * 0x1B3);

    }
    return hash;
}

int main() {
    std::string input;
    std::cout << "Enter a string: ";
    std::getline(std::cin, input);
    uint64_t result = spark_hash(input);
    std::cout << "Hash: " << std::hex << std::setw(16) << std::setfill('0') << result << std::endl;
    return 0;
}
