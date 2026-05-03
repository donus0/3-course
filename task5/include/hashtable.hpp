#pragma once

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// HashTable с методом цепочек (separate chaining).
// Коллизии разрешаются хранением списка элементов в каждом бакете.
template <class Key, class Value, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
class HashTable {
public:
    using key_type = Key;
    using mapped_type = Value;
    using hasher = Hash;
    using key_equal = KeyEqual;

    HashTable() = default;

    explicit HashTable(std::size_t bucket_count, Hash h = Hash{}, KeyEqual eq = KeyEqual{})
        : hash_(std::move(h)), eq_(std::move(eq)) {
        if (bucket_count == 0) throw std::invalid_argument("HashTable: bucket_count must be > 0");
        buckets_.assign(bucket_count, {});
    }

    explicit HashTable(Hash h) : hash_(std::move(h)) { buckets_.assign(16, {}); }

    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }
    [[nodiscard]] std::size_t bucket_count() const { return buckets_.size(); }

    [[nodiscard]] float load_factor() const {
        return buckets_.empty() ? 0.0f : static_cast<float>(size_) / static_cast<float>(buckets_.size());
    }

    void max_load_factor(float lf) {
        if (!(lf > 0.0f)) throw std::invalid_argument("HashTable: max_load_factor must be > 0");
        max_load_factor_ = lf;
        if (!buckets_.empty()) maybe_rehash_for_insert();
    }
    [[nodiscard]] float max_load_factor() const { return max_load_factor_; }

    void clear() {
        for (auto& b : buckets_) b.clear();
        size_ = 0;
    }

    // Добавление (upsert): если ключ уже есть — обновляем значение.
    void insert(const Key& key, const Value& value) { insert_impl(key, value); }
    void insert(const Key& key, Value&& value) { insert_impl(key, std::move(value)); }
    void insert(Key&& key, const Value& value) { insert_impl(std::move(key), value); }
    void insert(Key&& key, Value&& value) { insert_impl(std::move(key), std::move(value)); }

    [[nodiscard]] Value* find(const Key& key) {
        if (buckets_.empty()) return nullptr;
        auto& b = buckets_[bucket_index(key)];
        for (auto& e : b) {
            if (eq_(e.key, key)) return &e.value;
        }
        return nullptr;
    }

    [[nodiscard]] const Value* find(const Key& key) const {
        if (buckets_.empty()) return nullptr;
        const auto& b = buckets_[bucket_index(key)];
        for (const auto& e : b) {
            if (eq_(e.key, key)) return &e.value;
        }
        return nullptr;
    }

    // Удаление: true если удалили, false если ключа не было.
    bool erase(const Key& key) {
        if (buckets_.empty()) return false;
        auto& b = buckets_[bucket_index(key)];
        for (std::size_t i = 0; i < b.size(); ++i) {
            if (eq_(b[i].key, key)) {
                b[i] = std::move(b.back());
                b.pop_back();
                --size_;
                return true;
            }
        }
        return false;
    }

private:
    struct Entry {
        Key key;
        Value value;
    };

    std::vector<std::vector<Entry>> buckets_;
    Hash hash_{};
    KeyEqual eq_{};
    std::size_t size_ = 0;
    float max_load_factor_ = 1.0f;

    [[nodiscard]] std::size_t bucket_index(const Key& key) const {
        return hash_(key) % buckets_.size();
    }

    void maybe_rehash_for_insert() {
        if (buckets_.empty()) return;
        if (load_factor() <= max_load_factor_) return;
        rehash(buckets_.size() * 2);
    }

    void rehash(std::size_t new_bucket_count) {
        if (new_bucket_count == 0) new_bucket_count = 1;
        std::vector<std::vector<Entry>> new_buckets(new_bucket_count);
        for (auto& b : buckets_) {
            for (auto& e : b) {
                const std::size_t idx = hash_(e.key) % new_bucket_count;
                new_buckets[idx].push_back(std::move(e));
            }
        }
        buckets_ = std::move(new_buckets);
    }

    template <class K, class V>
    void insert_impl(K&& key, V&& value) {
        if (buckets_.empty()) buckets_.assign(16, {});

        // сначала ищем существующий ключ (без изменения size_)
        {
            auto& b = buckets_[hash_(key) % buckets_.size()];
            for (auto& e : b) {
                if (eq_(e.key, key)) {
                    e.value = std::forward<V>(value);
                    return;
                }
            }
        }

        // новый ключ
        ++size_;
        maybe_rehash_for_insert();

        auto& b = buckets_[hash_(key) % buckets_.size()];
        b.push_back(Entry{std::forward<K>(key), std::forward<V>(value)});
    }
};

