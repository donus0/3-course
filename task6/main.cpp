#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "count_min_sketch.hpp"

int main() {
    // Пример: Count-Min Sketch на строках.
    // Конфиг:
    // - row_state_bits = 4096 бит на строку (в реализации это 4096/32 = 128 счётчиков на строку)
    // - 3 хеш-функции (depth = 3)
    const std::size_t row_state_bits = 4096;
    std::vector<CountMinSketch<std::string>::HashFn> hashes;
    hashes.emplace_back([](const std::string& s) { return std::hash<std::string>{}(s); });
    hashes.emplace_back([](const std::string& s) { return std::hash<std::string>{}("salt1|" + s); });
    hashes.emplace_back([](const std::string& s) { return std::hash<std::string>{}("salt2|" + s); });

    CountMinSketch<std::string> cms(row_state_bits, std::move(hashes));

    cms.add("apple", 5);
    cms.add("banana", 2);
    cms.add("apple", 3);
    cms.add("orange", 1);

    for (std::string q : {"apple", "banana", "orange", "grape"}) {
        std::cout << q << ": estimate=" << cms.estimate(q) << "\n";
    }

    return 0;
}

