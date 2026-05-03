#include <iostream>
#include <string>

#include "trie.hpp"

int main() {
    // Алфавит настраиваемый: здесь латиница в нижнем регистре + '|'
    Trie<int> t("abcdefghijklmnopqrstuvwxyz|");

    t.insert("a", 1);
    t.insert("ab", 2);
    t.insert("abc", 3);
    t.insert("foo|bar", 42);

    for (std::string q : {"a", "ab", "abc", "abcd", "foo|bar", "foo"}) {
        if (auto* v = t.find(q)) {
            std::cout << "find(\"" << q << "\") = " << *v << "\n";
        } else {
            std::cout << "find(\"" << q << "\") = <not found>\n";
        }
    }

    std::cout << "erase(\"ab\") = " << (t.erase("ab") ? "true" : "false") << "\n";
    std::cout << "find(\"ab\") after erase = " << (t.find("ab") ? "found" : "not found") << "\n";
    std::cout << "find(\"abc\") after erase = " << (t.find("abc") ? "found" : "not found") << "\n";

    return 0;
}

