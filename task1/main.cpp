#include <iostream>
#include <string>
#include <vector>

#include "bst.hpp"

template <class K, class V>
static void print_pairs(const std::vector<std::pair<K, V>>& v) {
    for (const auto& [k, val] : v) std::cout << "(" << k << ", " << val << ") ";
    std::cout << "\n";
}

int main() {
    // Пример: BST<int, std::string> с обычным порядком (std::less)
    BST<int, std::string> t;
    t.insert(5, "five");
    t.insert(2, "two");
    t.insert(8, "eight");
    t.insert(1, "one");
    t.insert(3, "three");
    t.insert(7, "seven");
    t.insert(9, "nine");

    std::cout << "Inorder (sorted by key): ";
    print_pairs(t.to_vector_inorder());

    if (auto* v = t.find(7)) {
        std::cout << "find(7) = " << *v << "\n";
    } else {
        std::cout << "find(7) = <not found>\n";
    }

    std::cout << "erase(2) = " << (t.erase(2) ? "true" : "false") << "\n";
    std::cout << "Inorder after erase(2): ";
    print_pairs(t.to_vector_inorder());

    std::cout << "Preorder: ";
    print_pairs(t.to_vector_preorder());
    std::cout << "Postorder: ";
    print_pairs(t.to_vector_postorder());

    // Пример вариативного отношения порядка: по убыванию ключа
    BST<int, std::string, std::greater<int>> desc;
    desc.insert(10, "ten");
    desc.insert(5, "five");
    desc.insert(20, "twenty");
    std::cout << "Inorder with std::greater (descending): ";
    print_pairs(desc.to_vector_inorder());

    return 0;
}