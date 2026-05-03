#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

// Count-Min Sketch: вероятностная структура для оценки частоты (количества вхождений).
// Гарантии (при неотрицательных добавлениях):
// - недооценки нет (estimate(x) >= true_count(x))
// - возможна переоценка из-за коллизий; ответ = min по строкам
template <class Key>
class CountMinSketch {
public:
    using HashFn = std::function<std::size_t(const Key&)>;
    using counter_type = std::uint32_t;

    CountMinSketch() = default;

    // row_state_bits: размер состояния ОДНОЙ строки матрицы (в битах).
    // Реализация хранит счётчики по 32 бита, поэтому фактическая ширина строки:
    // width = ceil(row_state_bits / 32).
    CountMinSketch(std::size_t row_state_bits, std::vector<HashFn> hashes)
        : row_state_bits_(row_state_bits), hashes_(std::move(hashes)) {
        if (row_state_bits_ == 0) throw std::invalid_argument("CountMinSketch: row_state_bits must be > 0");
        if (hashes_.empty()) throw std::invalid_argument("CountMinSketch: at least one hash function is required");

        width_ = (row_state_bits_ + 31) / 32;
        if (width_ == 0) width_ = 1;

        table_.assign(hashes_.size(), std::vector<counter_type>(width_, 0));
    }

    [[nodiscard]] std::size_t depth() const { return hashes_.size(); }
    [[nodiscard]] std::size_t width() const { return width_; }
    [[nodiscard]] std::size_t row_state_bits() const { return row_state_bits_; }

    void clear() {
        ensure_ready();
        for (auto& row : table_) std::fill(row.begin(), row.end(), 0);
    }

    // Добавить count экземпляров key (count >= 1).
    void add(const Key& key, std::uint64_t count = 1) {
        ensure_ready();
        if (count == 0) return;

        for (std::size_t r = 0; r < hashes_.size(); ++r) {
            const std::size_t c = hashes_[r](key) % width_;
            const std::uint64_t cur = table_[r][c];
            const std::uint64_t next = cur + count;
            table_[r][c] = static_cast<counter_type>(
                std::min<std::uint64_t>(next, std::numeric_limits<counter_type>::max()));
        }
    }

    // Оценка числа вхождений key: min по строкам.
    [[nodiscard]] std::uint32_t estimate(const Key& key) const {
        ensure_ready();
        std::uint32_t ans = std::numeric_limits<std::uint32_t>::max();
        for (std::size_t r = 0; r < hashes_.size(); ++r) {
            const std::size_t c = hashes_[r](key) % width_;
            ans = std::min<std::uint32_t>(ans, table_[r][c]);
        }
        return ans;
    }

private:
    std::size_t row_state_bits_ = 0;
    std::size_t width_ = 0;
    std::vector<HashFn> hashes_;
    std::vector<std::vector<counter_type>> table_;

    void ensure_ready() const {
        if (row_state_bits_ == 0 || width_ == 0 || hashes_.empty() || table_.empty()) {
            throw std::logic_error("CountMinSketch: sketch is not configured (call CountMinSketch(bits, hashes))");
        }
    }
};

