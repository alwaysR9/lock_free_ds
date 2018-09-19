#include "lock_free_list.h"

Node::Node(long val, Node* next) {
    this->val = val;
    this->next_ = next;
}

bool Node::is_mark() {
    return (unsigned long)next_.load() & (unsigned long)0x1;
}

void Node::mark() {
    next_ = (Node*) ((unsigned long)next_.load() | (unsigned long)0x1);
}

Node* Node::get_next() {
    return (Node*) ((unsigned long)next_.load() & ~(unsigned long)0x1);
}

LockFreeList::LockFreeList() {
    head_ = new Node(0, NULL);
}

LockFreeList::~LockFreeList() {

}

bool LockFreeList::add(const long val) {
    // insert node between pre and cur->val
    Node* pre = head_;
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            Node* next = cur->get_next();
            if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
                std::cout << "marked node has been deleted by other thread during Tranverse" << std::endl;
            }
            cur = pre->next_;
            continue;
        }
        if (val <= cur->val) {
            break;
        }
        pre = cur;
        cur = pre->next_;
    }

    if (cur == NULL) {
        Node* node = new Node(val, NULL);
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, node)) {
            std::cout << "insert node failed_x " << val << std::endl;
            return false;
        }
        return true;
    }

    if (val == cur->val) {
        return false;
    } else { // val < cur->val
        Node* node = new Node(val, cur);
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, node)) {
            std::cout << "insert node failed_y " << val << std::endl;
            return false;
        }
        return true;
    }
}

bool LockFreeList::rm(const long val) {
    // delete node with val == cur
    Node* pre = head_;
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            Node* next = cur->get_next();
            if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
                std::cout << "marked node has been deleted by other thread during Tranverse" << std::endl;
            }
            cur = pre->next_;
            continue;
        }
        if (val <= cur->val) {
            break;
        }
        pre = cur;
        cur = pre->next_;
    }

    if (cur == NULL) {
        return false;
    }

    if (val == cur->val) {
        cur->mark();
        Node* next = cur->get_next();
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
            std::cout << "marked node has been deleted by other thread" << std::endl;
        }
        return true;
    } else { // val < cur->val
        return false;
    }
}

bool LockFreeList::contains(const long val) {
    Node* pre = head_;
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            Node* next = cur->get_next();
            if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
                std::cout << "marked node has been deleted by other thread during Tranverse" << std::endl;
            }
            cur = pre->next_;
            continue;
        }
        if (val <= cur->val) {
            break;
        }
        pre = cur;
        cur = pre->next_.load();
    }

    if (cur == NULL) {
        return false;
    }

    if (val == cur->val) {
        return true;
    } else {
        return false;
    }
}

std::vector<long> LockFreeList::vectorize() {
    std::vector<long> v;
    
    Node* cur = head_->next_;
    while (cur) {
        v.push_back(cur->val);
        cur = cur->get_next();
    }
    return v;
}