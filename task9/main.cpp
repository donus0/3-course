#include <iostream>
#include <string>

#include "binary_heap.hpp"
#include "binomial_heap.hpp"

static void print_top_ptr(const char* label, const std::pair<int, std::string>* top) {
    if (top) {
        std::cout << label << " priority=" << top->first << ", value=" << top->second << "\n";
    } else {
        std::cout << label << " <empty>\n";
    }
}

int main() {
    // Пример 1: двоичная max-куча (приоритет int, значение string).
    BinaryHeap<int, std::string> bh;
    bh.insert(3, "low");
    bh.insert(10, "high");
    bh.insert(7, "mid");
    bh.insert(10, "high-dup");

    print_top_ptr("BinaryHeap peek", bh.peek());
    if (auto x = bh.extract_max()) {
        std::cout << "BinaryHeap extract_max -> (" << x->first << ", " << x->second << ")\n";
    }
    print_top_ptr("BinaryHeap peek after extract", bh.peek());
    std::cout << "BinaryHeap size = " << bh.size() << "\n";

    // Пример 2: биномиальная max-куча.
    BinomialHeap<int, std::string> binh;
    binh.insert(5, "five");
    binh.insert(12, "twelve");
    binh.insert(8, "eight");

    if (auto top = binh.peek()) {
        std::cout << "BinomialHeap peek -> (" << top->first << ", " << top->second << ")\n";
    }
    if (auto x = binh.extract_max()) {
        std::cout << "BinomialHeap extract_max -> (" << x->first << ", " << x->second << ")\n";
    }
    std::cout << "BinomialHeap size after extract = " << binh.size() << "\n";

    // Пример 3: слияние двух биномиальных куч (исходные кучи разрушаются).
    BinomialHeap<int, std::string> left;
    left.insert(4, "L1");
    left.insert(9, "L2");

    BinomialHeap<int, std::string> right;
    right.insert(6, "R1");
    right.insert(11, "R2");

    auto merged = BinomialHeap<int, std::string>::merge(std::move(left), std::move(right));
    std::cout << "merged.size = " << merged.size()
              << ", left.empty = " << (left.empty() ? "true" : "false")
              << ", right.empty = " << (right.empty() ? "true" : "false") << "\n";

    while (auto x = merged.extract_max()) {
        std::cout << "merged extract -> (" << x->first << ", " << x->second << ")\n";
    }

    return 0;
}
