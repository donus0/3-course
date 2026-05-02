#pragma once

#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

template <class Key, class Value, class Compare = std::less<Key>>
class BST {
public:
    struct Node {
        Key key;
        Value value;
        Node* left = nullptr;
        Node* right = nullptr;

        Node(const Key& k, const Value& v) : key(k), value(v) {}
        Node(Key&& k, Value&& v) : key(std::move(k)), value(std::move(v)) {}
    };

    BST() = default; // создание пустого BST
    explicit BST(Compare comp) : comp_(std::move(comp)) {}

    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    BST(BST&& other) noexcept : root_(other.root_), comp_(std::move(other.comp_)), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }

    BST& operator=(BST&& other) noexcept {
        if (this == &other) return *this;
        clear();
        root_ = other.root_;
        comp_ = std::move(other.comp_);
        size_ = other.size_;
        other.root_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    ~BST() { clear(); } // удаление BST

    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    void clear() {
        destroy(root_);
        root_ = nullptr;
        size_ = 0;
    }

    // добавление значения по ключу: если ключ уже есть — обновляем value
    void insert(const Key& key, const Value& value) { insert_impl(root_, key, value); }
    void insert(Key&& key, Value&& value) { insert_impl(root_, std::move(key), std::move(value)); }

    // поиск значения по ключу
    [[nodiscard]] Value* find(const Key& key) { return find_impl(root_, key); }
    [[nodiscard]] const Value* find(const Key& key) const { return find_impl(root_, key); }

    // удаление значения по ключу: true если удалили, false если ключа не было
    bool erase(const Key& key) { return erase_impl(root_, key); }

    // обходы BST (префиксный, инфиксный, постфиксный)
    template <class Visitor>
    void traverse_preorder(Visitor&& visit) const {
        preorder_impl(root_, std::forward<Visitor>(visit));
    }

    template <class Visitor>
    void traverse_inorder(Visitor&& visit) const {
        inorder_impl(root_, std::forward<Visitor>(visit));
    }

    template <class Visitor>
    void traverse_postorder(Visitor&& visit) const {
        postorder_impl(root_, std::forward<Visitor>(visit));
    }

    // удобные версии, возвращающие список (ключ, значение)
    [[nodiscard]] std::vector<std::pair<Key, Value>> to_vector_preorder() const {
        std::vector<std::pair<Key, Value>> out;
        out.reserve(size_);
        traverse_preorder([&](const Key& k, const Value& v) { out.emplace_back(k, v); });
        return out;
    }

    [[nodiscard]] std::vector<std::pair<Key, Value>> to_vector_inorder() const {
        std::vector<std::pair<Key, Value>> out;
        out.reserve(size_);
        traverse_inorder([&](const Key& k, const Value& v) { out.emplace_back(k, v); });
        return out;
    }

    [[nodiscard]] std::vector<std::pair<Key, Value>> to_vector_postorder() const {
        std::vector<std::pair<Key, Value>> out;
        out.reserve(size_);
        traverse_postorder([&](const Key& k, const Value& v) { out.emplace_back(k, v); });
        return out;
    }

private:
    Node* root_ = nullptr;
    Compare comp_{};
    std::size_t size_ = 0;

    static void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    bool keys_equal(const Key& a, const Key& b) const {
        return !comp_(a, b) && !comp_(b, a);
    }

    template <class K, class V>
    void insert_impl(Node*& cur, K&& key, V&& value) {
        if (!cur) {
            cur = new Node(std::forward<K>(key), std::forward<V>(value));
            ++size_;
            return;
        }

        if (keys_equal(key, cur->key)) {
            cur->value = std::forward<V>(value);
            return;
        }

        if (comp_(key, cur->key)) {
            insert_impl(cur->left, std::forward<K>(key), std::forward<V>(value));
        } else {
            insert_impl(cur->right, std::forward<K>(key), std::forward<V>(value));
        }
    }

    static Value* find_impl(Node* cur, const Key& key, const Compare& comp) {
        while (cur) {
            const bool eq = !comp(key, cur->key) && !comp(cur->key, key);
            if (eq) return &cur->value;
            cur = comp(key, cur->key) ? cur->left : cur->right;
        }
        return nullptr;
    }

    Value* find_impl(Node* cur, const Key& key) { return find_impl(cur, key, comp_); }
    const Value* find_impl(Node* cur, const Key& key) const { return find_impl(cur, key, comp_); }

    // удаление узла, с перестройкой дерева через ссылку на указатель
    bool erase_impl(Node*& cur, const Key& key) {
        if (!cur) return false;

        if (keys_equal(key, cur->key)) {
            Node* victim = cur;

            if (!cur->left) {
                cur = cur->right;
                delete victim;
                --size_;
                return true;
            }
            if (!cur->right) {
                cur = cur->left;
                delete victim;
                --size_;
                return true;
            }

            // два ребёнка: берём inorder-successor (минимум в правом поддереве)
            Node*& succ_ref = min_node_ref(cur->right);
            Node* succ = succ_ref;

            // переносим данные succ в cur, затем удаляем succ (у succ не может быть left)
            cur->key = std::move(succ->key);
            cur->value = std::move(succ->value);
            succ_ref = succ->right;
            delete succ;
            --size_;
            return true;
        }

        if (comp_(key, cur->key)) return erase_impl(cur->left, key);
        return erase_impl(cur->right, key);
    }

    static Node*& min_node_ref(Node*& cur) {
        while (cur->left) cur = cur->left;
        return cur;
    }

    template <class Visitor>
    static void preorder_impl(const Node* n, Visitor&& visit) {
        if (!n) return;
        visit(n->key, n->value);
        preorder_impl(n->left, std::forward<Visitor>(visit));
        preorder_impl(n->right, std::forward<Visitor>(visit));
    }

    template <class Visitor>
    static void inorder_impl(const Node* n, Visitor&& visit) {
        if (!n) return;
        inorder_impl(n->left, std::forward<Visitor>(visit));
        visit(n->key, n->value);
        inorder_impl(n->right, std::forward<Visitor>(visit));
    }

    template <class Visitor>
    static void postorder_impl(const Node* n, Visitor&& visit) {
        if (!n) return;
        postorder_impl(n->left, std::forward<Visitor>(visit));
        postorder_impl(n->right, std::forward<Visitor>(visit));
        visit(n->key, n->value);
    }
};

