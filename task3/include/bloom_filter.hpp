#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// Фильтр Блума: вероятностная структура для проверки принадлежности множеству.
// Ложные срабатывания возможны, ложных отрицаний (при корректном использовании) нет.
template <class Key>
class BloomFilter {
public:
    using HashFn = std::function<std::size_t(const Key&)>;

    BloomFilter() = default;

    // bits - размер состояния в битах
    // hashes - набор хеш-функций (k >= 1)
    BloomFilter(std::size_t bits, std::vector<HashFn> hashes) : bit_count_(bits), hashes_(std::move(hashes)) {
        if (bit_count_ == 0) throw std::invalid_argument("BloomFilter: bit size must be > 0");
        if (hashes_.empty()) throw std::invalid_argument("BloomFilter: at least one hash function is required");
        words_.assign((bit_count_ + 63) / 64, 0ULL);
    }

    [[nodiscard]] std::size_t bit_count() const { return bit_count_; }
    [[nodiscard]] std::size_t hash_count() const { return hashes_.size(); }

    void clear() {
        for (auto& w : words_) w = 0ULL;
    }

    void add(const Key& key) {
        ensure_ready();
        for (const auto& h : hashes_) set_bit(h(key) % bit_count_);
    }

    [[nodiscard]] bool contains(const Key& key) const {
        ensure_ready();
        for (const auto& h : hashes_) {
            if (!get_bit(h(key) % bit_count_)) return false;
        }
        return true;
    }

private:
    std::size_t bit_count_ = 0;
    std::vector<std::uint64_t> words_;
    std::vector<HashFn> hashes_;

    void ensure_ready() const {
        if (bit_count_ == 0 || words_.empty() || hashes_.empty()) {
            throw std::logic_error("BloomFilter: filter is not configured (call BloomFilter(bits, hashes))");
        }
    }

    void set_bit(std::size_t idx) {
        const std::size_t w = idx / 64;
        const std::size_t b = idx % 64;
        words_[w] |= (1ULL << b);
    }

    [[nodiscard]] bool get_bit(std::size_t idx) const {
        const std::size_t w = idx / 64;
        const std::size_t b = idx % 64;
        return (words_[w] >> b) & 1ULL;
    }
};

