#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

// B-дерево минимальной степени T (T >= 2).
// В узле: максимум 2T-1 ключей и 2T детей.
template <class Key, class Value, std::size_t T = 2, class Compare = std::less<Key>>
class BTree {
    static_assert(T >= 2, "BTree minimal degree T must be >= 2");

public:
    struct Node {
        bool leaf = true;
        std::vector<Key> keys;
        std::vector<Value> values;
        std::vector<Node*> children; // size = keys.size()+1 for internal nodes

        explicit Node(bool is_leaf) : leaf(is_leaf) {
            keys.reserve(2 * T - 1);
            values.reserve(2 * T - 1);
            children.reserve(2 * T);
        }
    };

    BTree() = default; // создание пустого B-дерева
    explicit BTree(Compare comp) : comp_(std::move(comp)) {}

    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    BTree(BTree&& other) noexcept : root_(other.root_), comp_(std::move(other.comp_)), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    BTree& operator=(BTree&& other) noexcept {
        if (this == &other) return *this;
        clear();
        root_ = other.root_;
        comp_ = std::move(other.comp_);
        size_ = other.size_;
        other.root_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    ~BTree() { clear(); } // удаление B-дерева

    [[nodiscard]] bool empty() const { return size_ == 0; }
    [[nodiscard]] std::size_t size() const { return size_; }

    void clear() {
        destroy(root_);
        root_ = nullptr;
        size_ = 0;
    }

    // поиск значения по ключу
    [[nodiscard]] Value* find(const Key& key) { return find_impl(root_, key); }
    [[nodiscard]] const Value* find(const Key& key) const { return find_impl(root_, key); }

    // добавление значения по ключу (если ключ уже есть — обновляем значение)
    void insert(const Key& key, const Value& value) { insert_impl(key, value); }
    void insert(Key&& key, Value&& value) { insert_impl(std::move(key), std::move(value)); }

    // удаление значения по ключу: true если удалили, false если ключа не было
    bool erase(const Key& key) {
        if (!root_) return false;
        const bool removed = erase_from_node(root_, key);
        if (root_ && !root_->leaf && root_->keys.empty()) {
            Node* old = root_;
            root_ = root_->children[0];
            old->children.clear();
            delete old;
        }
        if (removed) --size_;
        return removed;
    }

    // инфиксный обход (по возрастанию согласно Compare)
    template <class Visitor>
    void traverse_inorder(Visitor&& visit) const {
        inorder_impl(root_, std::forward<Visitor>(visit));
    }

    [[nodiscard]] std::vector<std::pair<Key, Value>> to_vector_inorder() const {
        std::vector<std::pair<Key, Value>> out;
        out.reserve(size_);
        traverse_inorder([&](const Key& k, const Value& v) { out.emplace_back(k, v); });
        return out;
    }

private:
    Node* root_ = nullptr;
    Compare comp_{};
    std::size_t size_ = 0;

    bool keys_equal(const Key& a, const Key& b) const { return !comp_(a, b) && !comp_(b, a); }

    static void destroy(Node* n) {
        if (!n) return;
        for (Node* c : n->children) destroy(c);
        delete n;
    }

    // возвращает индекс первого ключа >= key (lower_bound по компаратору)
    std::size_t lower_bound_keys(const Node* n, const Key& key) const {
        std::size_t i = 0;
        while (i < n->keys.size() && comp_(n->keys[i], key)) ++i;
        return i;
    }

    Value* find_impl(Node* n, const Key& key) const {
        while (n) {
            const std::size_t i = lower_bound_keys(n, key);
            if (i < n->keys.size() && keys_equal(n->keys[i], key)) return const_cast<Value*>(&n->values[i]);
            if (n->leaf) return nullptr;
            n = n->children[i];
        }
        return nullptr;
    }

    template <class K, class V>
    void insert_impl(K&& key, V&& value) {
        if (!root_) {
            root_ = new Node(true);
            root_->keys.emplace_back(std::forward<K>(key));
            root_->values.emplace_back(std::forward<V>(value));
            size_ = 1;
            return;
        }

        // если ключ уже есть — обновим (без изменения size_)
        if (Value* existing = find(std::as_const(key))) {
            *existing = std::forward<V>(value);
            return;
        }

        if (root_->keys.size() == 2 * T - 1) {
            Node* s = new Node(false);
            s->children.push_back(root_);
            split_child(s, 0);
            root_ = s;
        }

        insert_non_full(root_, std::forward<K>(key), std::forward<V>(value));
        ++size_;
    }

    void split_child(Node* parent, std::size_t idx) {
        Node* y = parent->children[idx];
        Node* z = new Node(y->leaf);

        // y: [0..2T-2] ключей, переносим правую половину в z
        // median = T-1 (0-based)
        const std::size_t median = T - 1;

        // keys/values to z: positions [T .. 2T-2] (count T-1)
        for (std::size_t j = 0; j < T - 1; ++j) {
            z->keys.push_back(std::move(y->keys[median + 1 + j]));
            z->values.push_back(std::move(y->values[median + 1 + j]));
        }

        if (!y->leaf) {
            for (std::size_t j = 0; j < T; ++j) {
                z->children.push_back(y->children[median + 1 + j]);
            }
            y->children.resize(median + 1);
        }

        Key mid_key = std::move(y->keys[median]);
        Value mid_val = std::move(y->values[median]);

        y->keys.resize(median);
        y->values.resize(median);

        parent->keys.insert(parent->keys.begin() + static_cast<std::ptrdiff_t>(idx), std::move(mid_key));
        parent->values.insert(parent->values.begin() + static_cast<std::ptrdiff_t>(idx), std::move(mid_val));
        parent->children.insert(parent->children.begin() + static_cast<std::ptrdiff_t>(idx + 1), z);
    }

    template <class K, class V>
    void insert_non_full(Node* x, K&& key, V&& value) {
        std::size_t i = lower_bound_keys(x, key);

        if (x->leaf) {
            x->keys.insert(x->keys.begin() + static_cast<std::ptrdiff_t>(i), std::forward<K>(key));
            x->values.insert(x->values.begin() + static_cast<std::ptrdiff_t>(i), std::forward<V>(value));
            return;
        }

        // идём в ребёнка i, перед спуском гарантируем, что он не полный
        if (x->children[i]->keys.size() == 2 * T - 1) {
            split_child(x, i);
            // после split median ушёл в x->keys[i]
            if (comp_(x->keys[i], key)) ++i;
        }
        insert_non_full(x->children[i], std::forward<K>(key), std::forward<V>(value));
    }

    template <class Visitor>
    static void inorder_impl(const Node* n, Visitor&& visit) {
        if (!n) return;
        const std::size_t m = n->keys.size();
        if (n->leaf) {
            for (std::size_t i = 0; i < m; ++i) visit(n->keys[i], n->values[i]);
            return;
        }
        for (std::size_t i = 0; i < m; ++i) {
            inorder_impl(n->children[i], std::forward<Visitor>(visit));
            visit(n->keys[i], n->values[i]);
        }
        inorder_impl(n->children[m], std::forward<Visitor>(visit));
    }

    // ---- Deletion (CLRS-style) ----

    bool erase_from_node(Node* x, const Key& key) {
        const std::size_t idx = lower_bound_keys(x, key);

        if (idx < x->keys.size() && keys_equal(x->keys[idx], key)) {
            if (x->leaf) {
                x->keys.erase(x->keys.begin() + static_cast<std::ptrdiff_t>(idx));
                x->values.erase(x->values.begin() + static_cast<std::ptrdiff_t>(idx));
                return true;
            }
            return erase_from_internal(x, idx);
        }

        if (x->leaf) return false;

        // ключа в x нет — спускаемся в ребёнка idx, но гарантируем, что у него >= T ключей
        Node* child = x->children[idx];
        if (child->keys.size() == T - 1) fill_child(x, idx);

        // после fill структура могла измениться
        const std::size_t next_idx = (idx > x->keys.size()) ? x->keys.size() : idx;
        return erase_from_node(x->children[next_idx], key);
    }

    bool erase_from_internal(Node* x, std::size_t idx) {
        Node* left = x->children[idx];
        Node* right = x->children[idx + 1];

        if (left->keys.size() >= T) {
            auto [pk, pv] = get_predecessor(left);
            x->keys[idx] = std::move(pk);
            x->values[idx] = std::move(pv);
            return erase_from_node(left, x->keys[idx]);
        }

        if (right->keys.size() >= T) {
            auto [sk, sv] = get_successor(right);
            x->keys[idx] = std::move(sk);
            x->values[idx] = std::move(sv);
            return erase_from_node(right, x->keys[idx]);
        }

        // оба по T-1: сливаем и удаляем из слитого
        merge_children(x, idx);
        return erase_from_node(left, x->keys[idx]); // key теперь внутри merged (left)
    }

    std::pair<Key, Value> get_predecessor(Node* x) const {
        while (!x->leaf) x = x->children.back();
        return {x->keys.back(), x->values.back()};
    }

    std::pair<Key, Value> get_successor(Node* x) const {
        while (!x->leaf) x = x->children.front();
        return {x->keys.front(), x->values.front()};
    }

    void fill_child(Node* parent, std::size_t idx) {
        // пытаемся занять у соседей, иначе merge
        if (idx > 0 && parent->children[idx - 1]->keys.size() >= T) {
            borrow_from_prev(parent, idx);
        } else if (idx + 1 < parent->children.size() && parent->children[idx + 1]->keys.size() >= T) {
            borrow_from_next(parent, idx);
        } else {
            if (idx + 1 < parent->children.size()) {
                merge_children(parent, idx);
            } else {
                merge_children(parent, idx - 1);
            }
        }
    }

    void borrow_from_prev(Node* parent, std::size_t idx) {
        Node* child = parent->children[idx];
        Node* sib = parent->children[idx - 1];

        // сдвигаем ребёнка: родительский ключ[idx-1] идёт в начало child,
        // последний ключ sib поднимается в родителя
        child->keys.insert(child->keys.begin(), std::move(parent->keys[idx - 1]));
        child->values.insert(child->values.begin(), std::move(parent->values[idx - 1]));

        parent->keys[idx - 1] = std::move(sib->keys.back());
        parent->values[idx - 1] = std::move(sib->values.back());
        sib->keys.pop_back();
        sib->values.pop_back();

        if (!sib->leaf) {
            child->children.insert(child->children.begin(), sib->children.back());
            sib->children.pop_back();
        }
    }

    void borrow_from_next(Node* parent, std::size_t idx) {
        Node* child = parent->children[idx];
        Node* sib = parent->children[idx + 1];

        child->keys.push_back(std::move(parent->keys[idx]));
        child->values.push_back(std::move(parent->values[idx]));

        parent->keys[idx] = std::move(sib->keys.front());
        parent->values[idx] = std::move(sib->values.front());
        sib->keys.erase(sib->keys.begin());
        sib->values.erase(sib->values.begin());

        if (!sib->leaf) {
            child->children.push_back(sib->children.front());
            sib->children.erase(sib->children.begin());
        }
    }

    void merge_children(Node* parent, std::size_t idx) {
        Node* left = parent->children[idx];
        Node* right = parent->children[idx + 1];

        // left получает разделяющий ключ из parent
        left->keys.push_back(std::move(parent->keys[idx]));
        left->values.push_back(std::move(parent->values[idx]));

        // + все ключи/значения right
        for (std::size_t i = 0; i < right->keys.size(); ++i) {
            left->keys.push_back(std::move(right->keys[i]));
            left->values.push_back(std::move(right->values[i]));
        }

        if (!left->leaf) {
            for (Node* c : right->children) left->children.push_back(c);
        }

        parent->keys.erase(parent->keys.begin() + static_cast<std::ptrdiff_t>(idx));
        parent->values.erase(parent->values.begin() + static_cast<std::ptrdiff_t>(idx));
        parent->children.erase(parent->children.begin() + static_cast<std::ptrdiff_t>(idx + 1));

        right->children.clear(); // дети перенесены (не удалять рекурсивно)
        delete right;
    }
};

