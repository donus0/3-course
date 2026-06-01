#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>

// Биномиальная max-куча.
template <class Priority, class Value, class Compare = std::less<Priority>>
class BinomialHeap {
public:
    using priority_type = Priority;
    using value_type = Value;

    struct Node {
        Priority priority;
        Value value;
        Node* parent = nullptr;
        Node* child = nullptr;
        Node* sibling = nullptr;
        int degree = 0;

        Node(const Priority& p, const Value& v) : priority(p), value(v) {}
        Node(Priority&& p, Value&& v) : priority(std::move(p)), value(std::move(v)) {}
    };

    BinomialHeap() = default; // создание пустой кучи
    explicit BinomialHeap(Compare comp) : comp_(std::move(comp)) {}

    BinomialHeap(const BinomialHeap&) = delete;
    BinomialHeap& operator=(const BinomialHeap&) = delete;

    BinomialHeap(BinomialHeap&& other) noexcept
        : head_(other.head_), comp_(std::move(other.comp_)), size_(other.size_) {
        other.head_ = nullptr;
        other.size_ = 0;
    }

    BinomialHeap& operator=(BinomialHeap&& other) noexcept {
        if (this == &other) return *this;
        clear();
        head_ = other.head_;
        comp_ = std::move(other.comp_);
        size_ = other.size_;
        other.head_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    ~BinomialHeap() { clear(); }

    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] bool empty() const { return size_ == 0; }

    void clear() {
        destroy_list(head_);
        head_ = nullptr;
        size_ = 0;
    }

    // Добавление значения по его приоритету.
    void insert(const Priority& priority, const Value& value) {
        BinomialHeap one(comp_);
        one.head_ = new Node(priority, value);
        one.size_ = 1;
        meld(std::move(one));
    }

    void insert(const Priority& priority, Value&& value) {
        BinomialHeap one(comp_);
        one.head_ = new Node(priority, std::move(value));
        one.size_ = 1;
        meld(std::move(one));
    }

    void insert(Priority&& priority, const Value& value) {
        BinomialHeap one(comp_);
        one.head_ = new Node(std::move(priority), value);
        one.size_ = 1;
        meld(std::move(one));
    }

    void insert(Priority&& priority, Value&& value) {
        BinomialHeap one(comp_);
        one.head_ = new Node(std::move(priority), std::move(value));
        one.size_ = 1;
        meld(std::move(one));
    }

    // Поиск значения с наибольшим приоритетом (без удаления).
    [[nodiscard]] std::optional<std::pair<Priority, Value>> peek() const {
        const Node* max_root = find_max_root();
        if (!max_root) return std::nullopt;
        return std::make_pair(max_root->priority, max_root->value);
    }

    // Удаление значения с наибольшим приоритетом.
    [[nodiscard]] std::optional<std::pair<Priority, Value>> extract_max() {
        Node* max_root = find_max_root();
        if (!max_root) return std::nullopt;

        unlink_root(max_root);
        auto result = std::make_pair(std::move(max_root->priority), std::move(max_root->value));

        Node* child_head = reverse_children(max_root);
        delete max_root;

        // Дочерние деревья уже учтены в size_; объединяем только списки корней.
        head_ = consolidate(merge_by_degree(head_, child_head));
        return result;
    }

    // Слияние двух куч с разрушением исходных куч.
    static BinomialHeap merge(BinomialHeap&& a, BinomialHeap&& b) {
        a.meld(std::move(b));
        return std::move(a);
    }

    void meld(BinomialHeap&& other) {
        if (this == &other) return;
        head_ = consolidate(merge_by_degree(head_, other.head_));
        size_ += other.size_;
        other.head_ = nullptr;
        other.size_ = 0;
    }

private:
    Node* head_ = nullptr;
    Compare comp_{};
    std::size_t size_ = 0;

    [[nodiscard]] bool higher(const Priority& a, const Priority& b) const { return !comp_(a, b); }

    [[nodiscard]] Node* find_max_root() const {
        Node* best = nullptr;
        for (Node* cur = head_; cur; cur = cur->sibling) {
            if (!best || higher(cur->priority, best->priority)) best = cur;
        }
        return best;
    }

    void unlink_root(Node* root) {
        Node** slot = &head_;
        while (*slot && *slot != root) slot = &(*slot)->sibling;
        if (*slot) {
            *slot = root->sibling;
            root->sibling = nullptr;
        }
        --size_;
    }

    [[nodiscard]] Node* reverse_children(Node* root) {
        Node* new_head = nullptr;
        Node* child = root->child;
        while (child) {
            Node* next = child->sibling;
            child->sibling = new_head;
            child->parent = nullptr;
            new_head = child;
            child = next;
        }
        root->child = nullptr;
        return new_head;
    }

    [[nodiscard]] static Node* merge_by_degree(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;

        Node* head = nullptr;
        Node** tail = &head;

        while (a && b) {
            if (a->degree <= b->degree) {
                *tail = a;
                a = a->sibling;
            } else {
                *tail = b;
                b = b->sibling;
            }
            tail = &(*tail)->sibling;
        }
        *tail = a ? a : b;
        return head;
    }

    void link_child(Node* parent, Node* child) {
        child->parent = parent;
        child->sibling = parent->child;
        parent->child = child;
        ++parent->degree;
    }

    [[nodiscard]] Node* consolidate(Node* head) {
        if (!head) return nullptr;

        Node* prev = nullptr;
        Node* curr = head;
        Node* next = curr->sibling;

        while (next) {
            if (curr->degree != next->degree ||
                (next->sibling && next->sibling->degree == curr->degree)) {
                prev = curr;
                curr = next;
            } else if (higher(curr->priority, next->priority)) {
                link_child(curr, next);
                next = next->sibling;
                curr->sibling = next;
            } else {
                link_child(next, curr);
                if (prev) {
                    prev->sibling = next;
                } else {
                    head = next;
                }
                curr = next;
                next = curr->sibling;
            }
            next = curr->sibling;
        }
        return head;
    }

    void destroy_tree(Node* node) {
        while (node) {
            Node* sibling = node->sibling;
            destroy_tree(node->child);
            delete node;
            node = sibling;
        }
    }

    void destroy_list(Node* root) {
        while (root) {
            Node* next = root->sibling;
            destroy_tree(root);
            root = next;
        }
    }
};
