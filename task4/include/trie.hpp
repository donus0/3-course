#pragma once

#include <array>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// Trie (префиксное дерево) по строковым ключам.
// Алфавит задаётся строкой допустимых символов; каждый символ отображается в индекс [0..sigma-1].
template <class Value>
class Trie {
public:
    Trie() = default;

    explicit Trie(std::string alphabet) : alphabet_(std::move(alphabet)) {
        if (alphabet_.empty()) throw std::invalid_argument("Trie: alphabet must be non-empty");
        build_map();
        nodes_.push_back(Node(sigma())); // root
    }

    [[nodiscard]] std::size_t sigma() const { return alphabet_.size(); }

    void clear() {
        ensure_ready();
        nodes_.clear();
        nodes_.push_back(Node(sigma()));
    }

    // Добавление/обновление значения по ключу.
    void insert(const std::string& key, const Value& value) { insert_impl(key, value); }
    void insert(const std::string& key, Value&& value) { insert_impl(key, std::move(value)); }

    // Поиск: возвращает указатель на значение или nullptr.
    [[nodiscard]] Value* find(const std::string& key) {
        const std::size_t idx = find_node_index(key);
        if (idx == npos) return nullptr;
        auto& opt = nodes_[idx].value;
        return opt ? &*opt : nullptr;
    }
    [[nodiscard]] const Value* find(const std::string& key) const {
        const std::size_t idx = find_node_index(key);
        if (idx == npos) return nullptr;
        const auto& opt = nodes_[idx].value;
        return opt ? &*opt : nullptr;
    }

    // Удаление значения по ключу: true если удалили, false если ключа не было.
    bool erase(const std::string& key) {
        ensure_ready();
        if (key.empty()) {
            if (!nodes_[0].value) return false;
            nodes_[0].value.reset();
            return true;
        }

        std::vector<std::pair<std::size_t, std::size_t>> path; // (node_index, edge_char_index)
        path.reserve(key.size());

        std::size_t cur = 0;
        for (unsigned char ch : key) {
            const std::size_t ci = char_index(ch);
            const std::size_t nxt = nodes_[cur].next[ci];
            if (nxt == npos) return false;
            path.emplace_back(cur, ci);
            cur = nxt;
        }

        if (!nodes_[cur].value) return false;
        nodes_[cur].value.reset();

        // Подчищаем хвост: пока узел пуст (нет value и нет детей), отцепляем его от родителя.
        for (std::size_t k = path.size(); k > 0; --k) {
            const auto [parent, edge] = path[k - 1];
            const std::size_t child = nodes_[parent].next[edge];
            if (child == npos) break;
            if (nodes_[child].value || nodes_[child].has_children()) break;
            nodes_[parent].next[edge] = npos;
        }

        return true;
    }

private:
    static constexpr std::size_t npos = static_cast<std::size_t>(-1);

    struct Node {
        std::vector<std::size_t> next; // indices in nodes_ or npos
        std::optional<Value> value;

        explicit Node(std::size_t sigma) : next(sigma, npos) {}

        [[nodiscard]] bool has_children() const {
            for (std::size_t i : next) {
                if (i != npos) return true;
            }
            return false;
        }
    };

    std::string alphabet_;
    std::array<int, 256> map_{}; // char -> index in alphabet_, -1 if not allowed
    std::vector<Node> nodes_;

    void ensure_ready() const {
        if (alphabet_.empty() || nodes_.empty()) {
            throw std::logic_error("Trie: not configured (call Trie(alphabet))");
        }
    }

    void build_map() {
        map_.fill(-1);
        for (std::size_t i = 0; i < alphabet_.size(); ++i) {
            const unsigned char c = static_cast<unsigned char>(alphabet_[i]);
            if (map_[c] != -1) throw std::invalid_argument("Trie: alphabet contains duplicate characters");
            map_[c] = static_cast<int>(i);
        }
    }

    [[nodiscard]] std::size_t char_index(unsigned char ch) const {
        const int idx = map_[ch];
        if (idx < 0) throw std::invalid_argument("Trie: key contains character not in alphabet");
        return static_cast<std::size_t>(idx);
    }

    [[nodiscard]] std::size_t find_node_index(const std::string& key) const {
        ensure_ready();
        std::size_t cur = 0;
        for (unsigned char ch : key) {
            const std::size_t ci = char_index(ch);
            const std::size_t nxt = nodes_[cur].next[ci];
            if (nxt == npos) return npos;
            cur = nxt;
        }
        return cur;
    }

    template <class V>
    void insert_impl(const std::string& key, V&& value) {
        ensure_ready();
        std::size_t cur = 0;
        for (unsigned char ch : key) {
            const std::size_t ci = char_index(ch);
            std::size_t nxt = nodes_[cur].next[ci];
            if (nxt == npos) {
                nxt = nodes_.size();
                nodes_.push_back(Node(sigma()));
                nodes_[cur].next[ci] = nxt;
            }
            cur = nxt;
        }
        nodes_[cur].value = std::forward<V>(value);
    }
};

