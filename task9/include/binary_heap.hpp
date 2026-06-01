#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

// Двоичная max-куча: родитель не меньше детей по приоритету (Compare = std::less<Priority>).
template <class Priority, class Value, class Compare = std::less<Priority>>
class BinaryHeap {
public:
    using priority_type = Priority;
    using value_type = Value;

    BinaryHeap() = default; // создание пустой кучи
    explicit BinaryHeap(Compare comp) : comp_(std::move(comp)) {}

    BinaryHeap(const BinaryHeap&) = delete;
    BinaryHeap& operator=(const BinaryHeap&) = delete;

    BinaryHeap(BinaryHeap&& other) noexcept
        : data_(std::move(other.data_)), comp_(std::move(other.comp_)), size_(other.size_) {
        other.size_ = 0;
    }

    BinaryHeap& operator=(BinaryHeap&& other) noexcept {
        if (this == &other) return *this;
        data_ = std::move(other.data_);
        comp_ = std::move(other.comp_);
        size_ = other.size_;
        other.size_ = 0;
        return *this;
    }

    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    void clear() {
        data_.clear();
        size_ = 0;
    }

    // Добавление значения по его приоритету.
    void insert(const Priority& priority, const Value& value) {
        data_.push_back({priority, value});
        sift_up(size_++);
    }

    void insert(const Priority& priority, Value&& value) {
        data_.push_back({priority, std::move(value)});
        sift_up(size_++);
    }

    void insert(Priority&& priority, const Value& value) {
        data_.push_back({std::move(priority), value});
        sift_up(size_++);
    }

    void insert(Priority&& priority, Value&& value) {
        data_.push_back({std::move(priority), std::move(value)});
        sift_up(size_++);
    }

    // Поиск значения с наибольшим приоритетом (без удаления).
    [[nodiscard]] const std::pair<Priority, Value>* peek() const {
        if (empty()) return nullptr;
        return &data_.front();
    }

    // Удаление значения с наибольшим приоритетом.
    [[nodiscard]] std::optional<std::pair<Priority, Value>> extract_max() {
        if (empty()) return std::nullopt;
        auto top = std::move(data_.front());
        data_.front() = std::move(data_.back());
        data_.pop_back();
        --size_;
        if (!empty()) sift_down(0);
        return top;
    }

private:
    std::vector<std::pair<Priority, Value>> data_;
    Compare comp_{};
    std::size_t size_ = 0;

    [[nodiscard]] bool higher(const Priority& a, const Priority& b) const {
        return !comp_(a, b); // a не хуже b
    }

    void sift_up(std::size_t i) {
        while (i > 0) {
            const std::size_t p = (i - 1) / 2;
            if (!higher(data_[i].first, data_[p].first)) break;
            std::swap(data_[i], data_[p]);
            i = p;
        }
    }

    void sift_down(std::size_t i) {
        while (true) {
            const std::size_t left = 2 * i + 1;
            const std::size_t right = left + 1;
            std::size_t best = i;

            if (left < size_ && higher(data_[left].first, data_[best].first)) best = left;
            if (right < size_ && higher(data_[right].first, data_[best].first)) best = right;
            if (best == i) break;

            std::swap(data_[i], data_[best]);
            i = best;
        }
    }
};
