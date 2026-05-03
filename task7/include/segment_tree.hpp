#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

// Segment Tree для моноида:
// - ассоциативная операция op(a,b)
// - нейтральный элемент id
// Поддерживает:
// - point update: set(i, x)
// - range query: fold(l, r) для [l..r] включительно
template <class T, class Op>
class SegmentTree {
public:
    SegmentTree() = default;

    SegmentTree(std::size_t n, Op op, T identity) : n_(n), op_(std::move(op)), id_(std::move(identity)) {
        if (n_ == 0) throw std::invalid_argument("SegmentTree: n must be > 0");
        size_ = 1;
        while (size_ < n_) size_ <<= 1;
        tree_.assign(2 * size_, id_);
    }

    [[nodiscard]] std::size_t size() const { return n_; }

    // Обновление i-го элемента (0-based).
    void set(std::size_t i, const T& value) { set_impl(i, value); }
    void set(std::size_t i, T&& value) { set_impl(i, std::move(value)); }

    // fold на отрезке [l..r] включительно (0-based).
    [[nodiscard]] T fold(std::size_t l, std::size_t r) const {
        ensure_ready();
        if (l > r || r >= n_) throw std::out_of_range("SegmentTree: invalid range");
        return fold_impl(1, 0, size_ - 1, l, r);
    }

private:
    std::size_t n_ = 0;     // логический размер
    std::size_t size_ = 0;  // степень двойки >= n_
    Op op_{};
    T id_{};
    std::vector<T> tree_; // 1-indexed: [1..2*size_-1]

    void ensure_ready() const {
        if (n_ == 0 || size_ == 0 || tree_.empty()) {
            throw std::logic_error("SegmentTree: not configured (call SegmentTree(n, op, id))");
        }
    }

    template <class V>
    void set_impl(std::size_t i, V&& value) {
        ensure_ready();
        if (i >= n_) throw std::out_of_range("SegmentTree: index out of range");
        std::size_t pos = size_ + i;
        tree_[pos] = std::forward<V>(value);
        while (pos > 1) {
            pos >>= 1;
            tree_[pos] = op_(tree_[pos << 1], tree_[(pos << 1) | 1]);
        }
    }

    T fold_impl(std::size_t v, std::size_t tl, std::size_t tr, std::size_t l, std::size_t r) const {
        if (l > r) return id_;
        if (l == tl && r == tr) return tree_[v];
        const std::size_t tm = (tl + tr) >> 1;
        const T left = fold_impl(v << 1, tl, tm, l, (r < tm ? r : tm));
        const T right = fold_impl((v << 1) | 1, tm + 1, tr, (l > tm + 1 ? l : tm + 1), r);
        return op_(left, right);
    }
};

