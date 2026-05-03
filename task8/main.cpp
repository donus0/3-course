#include <iostream>
#include <limits>

#include "fenwick_tree.hpp"

struct SumOp {
    int operator()(int a, int b) const { return a + b; }
};

struct NegInv {
    int operator()(int x) const { return -x; }
};

int main() {
    // Пример: Fenwick для суммы (группа по сложению).
    FenwickTree<int, SumOp, NegInv> ft(5, SumOp{}, NegInv{}, 0);

    ft.set(0, 5);
    ft.set(1, 1);
    ft.set(2, 7);
    ft.set(3, 3);
    ft.set(4, 2);

    std::cout << "sum[0..4] = " << ft.fold(0, 4) << "\n"; // 18
    std::cout << "sum[1..3] = " << ft.fold(1, 3) << "\n"; // 11
    ft.set(2, 10);
    std::cout << "sum[1..3] after set(2,10) = " << ft.fold(1, 3) << "\n"; // 14
    std::cout << "prefix[0..2] = " << ft.prefix(2) << "\n"; // 16

    return 0;
}

