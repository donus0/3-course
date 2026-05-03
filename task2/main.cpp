#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "btree.hpp"

template <class K, class V>
static void print_pairs(const std::vector<std::pair<K, V>>& v) {
    for (const auto& [k, val] : v) std::cout << "(" << k << ", " << val << ") ";
    std::cout << "\n";
}

int main() {
    // Пример: B-дерево минимальной степени T=3 (до 5 ключей в узле)
    BTree<int, std::string, 3> t;

    for (int x : {10, 20, 5, 6, 12, 30, 7, 17}) {
        t.insert(x, "v" + std::to_string(x));
    }

    std::cout << "Inorder: ";
    print_pairs(t.to_vector_inorder());

    if (auto* v = t.find(12)) std::cout << "find(12) = " << *v << "\n";
    else std::cout << "find(12) = <not found>\n";

    std::cout << "erase(6) = " << (t.erase(6) ? "true" : "false") << "\n";
    std::cout << "erase(13) = " << (t.erase(13) ? "true" : "false") << "\n";
    std::cout << "Inorder after erase: ";
    print_pairs(t.to_vector_inorder());

    // Вариативный порядок: убывание ключей (Compare = std::greater)
    BTree<int, std::string, 2, std::greater<int>> desc;
    desc.insert(1, "one");
    desc.insert(3, "three");
    desc.insert(2, "two");
    std::cout << "Inorder with std::greater: ";
    print_pairs(desc.to_vector_inorder());

    return 0;
}

