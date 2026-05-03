#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "segment_tree.hpp"

struct SumOp {
    int operator()(int a, int b) const { return a + b; }
};

struct MinOp {
    int operator()(int a, int b) const { return std::min(a, b); }
};

int main() {
    // Пример 1: моноид суммы (op = +, id = 0)
    SegmentTree<int, SumOp> st_sum(5, SumOp{}, 0);
    std::vector<int> a = {5, 1, 7, 3, 2};
    for (std::size_t i = 0; i < a.size(); ++i) st_sum.set(i, a[i]);

    std::cout << "sum[0..4] = " << st_sum.fold(0, 4) << "\n"; // 18
    std::cout << "sum[1..3] = " << st_sum.fold(1, 3) << "\n"; // 11
    st_sum.set(2, 10); // a[2]=10
    std::cout << "sum[1..3] after set(2,10) = " << st_sum.fold(1, 3) << "\n"; // 14

    // Пример 2: моноид минимума (op = min, id = +inf)
    SegmentTree<int, MinOp> st_min(5, MinOp{}, std::numeric_limits<int>::max());
    for (std::size_t i = 0; i < a.size(); ++i) st_min.set(i, a[i]);
    std::cout << "min[0..4] = " << st_min.fold(0, 4) << "\n"; // 1
    std::cout << "min[2..4] = " << st_min.fold(2, 4) << "\n"; // 2

    return 0;
}

