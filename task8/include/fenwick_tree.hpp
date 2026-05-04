#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>


template <class T, class Op, class Inv>
class FenwickTree {
public:
    FenwickTree() = default;

    FenwickTree(std::size_t n, Op op, Inv inv, T identity)
        : n_(n), op_(std::move(op)), inv_(std::move(inv)), id_(std::move(identity)) {
        if (n_ == 0) throw std::invalid_argument("FenwickTree: n must be > 0");
        bit_.assign(n_ + 1, id_);   // 1-based
        vals_.assign(n_, id_);      // храним текущие значения для set(i,x)
    }

    [[nodiscard]] std::size_t size() const { return n_; }

    // Обновление i-го элемента (0-based): a[i] = value.
    void set(std::size_t i, const T& value) { set_impl(i, value); }
    void set(std::size_t i, T&& value) { set_impl(i, std::move(value)); }

    // prefix fold на [0..r] включительно.
    [[nodiscard]] T prefix(std::size_t r) const {
        ensure_ready();
        if (r >= n_) throw std::out_of_range("FenwickTree: index out of range");
        T res = id_;
        std::size_t idx = r + 1; // to 1-based
        while (idx > 0) {
            res = op_(res, bit_[idx]);
            idx -= idx & (~idx + 1); // lowbit
        }
        return res;
    }

    // fold на [l..r] включительно.
    [[nodiscard]] T fold(std::size_t l, std::size_t r) const {
        ensure_ready();
        if (l > r || r >= n_) throw std::out_of_range("FenwickTree: invalid range");
        const T pr = prefix(r);
        if (l == 0) return pr;
        const T plm1 = prefix(l - 1);
        return op_(pr, inv_(plm1));
    }

private:
    std::size_t n_ = 0;
    Op op_{};
    Inv inv_{};
    T id_{};
    std::vector<T> bit_;   // дерево Фенвика, 1-based
    std::vector<T> vals_;  // текущее значение массива

    void ensure_ready() const {
        if (n_ == 0 || bit_.empty() || vals_.empty()) {
            throw std::logic_error("FenwickTree: not configured (call FenwickTree(n, op, inv, id))");
        }
    }

    static std::size_t lowbit(std::size_t x) { return x & (~x + 1); }

    void add_delta(std::size_t i, const T& delta) {
        std::size_t idx = i + 1;
        while (idx <= n_) {
            bit_[idx] = op_(bit_[idx], delta);
            idx += lowbit(idx);
        }
    }

    template <class V>
    void set_impl(std::size_t i, V&& value) {
        ensure_ready();
        if (i >= n_) throw std::out_of_range("FenwickTree: index out of range");

        // delta = value - old (в терминах группы: op(value, inv(old)))
        const T delta = op_(static_cast<T>(value), inv_(vals_[i]));
        vals_[i] = std::forward<V>(value);
        add_delta(i, delta);
    }
};

