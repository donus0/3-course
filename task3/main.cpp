#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "bloom_filter.hpp"

int main() {
    // Пример: фильтр на строках, 1024 бита, 3 хеш-функции.
    // Здесь хеши сделаны "по-простому": std::hash + разные соли.
    const std::size_t bits = 1024;
    std::vector<BloomFilter<std::string>::HashFn> hashes;
    hashes.emplace_back([](const std::string& s) { return std::hash<std::string>{}(s); });
    hashes.emplace_back([](const std::string& s) { return std::hash<std::string>{}("salt1|" + s); });
    hashes.emplace_back([](const std::string& s) { return std::hash<std::string>{}("salt2|" + s); });

    BloomFilter<std::string> bf(bits, std::move(hashes));

    bf.add("apple");
    bf.add("banana");
    bf.add("orange");

    for (const auto& q : {"apple", "banana", "grape", "lemon", "orange"}) {
        std::cout << q << ": " << (bf.contains(q) ? "maybe" : "no") << "\n";
    }

    return 0;
}

