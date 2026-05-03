#include <iostream>
#include <string>

#include "hashtable.hpp"

int main() {
    // Пример: хеш-таблица с цепочками, ключ string -> значение int.
    // Хеш-функцию можно настроить: передать свой функтор/лямбду (через std::function не требуется).
    HashTable<std::string, int> ht;

    ht.insert("apple", 10);
    ht.insert("banana", 20);
    ht.insert("orange", 30);

    // upsert
    ht.insert("banana", 25);

    for (std::string q : {"apple", "banana", "grape", "orange"}) {
        if (auto* v = ht.find(q)) {
            std::cout << q << ": " << *v << "\n";
        } else {
            std::cout << q << ": <not found>\n";
        }
    }

    std::cout << "erase(\"apple\") = " << (ht.erase("apple") ? "true" : "false") << "\n";
    std::cout << "erase(\"apple\") again = " << (ht.erase("apple") ? "true" : "false") << "\n";
    std::cout << "size = " << ht.size() << ", buckets = " << ht.bucket_count()
              << ", load_factor = " << ht.load_factor() << "\n";

    return 0;
}

