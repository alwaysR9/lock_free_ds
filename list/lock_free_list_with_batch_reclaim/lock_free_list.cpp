#include "lock_free_list.h"

#include <assert.h>

LockFreeList::LockFreeList() {
    head_ = new Node(-1, NULL);
}

LockFreeList::~LockFreeList() {
    while (head_->get_next() != NULL) {
        Node* node = head_->get_next();
        head_->next_ = node->get_next();
        delete node;
    }
    delete head_;
}

bool LockFreeList::add(const long val) {
    // insert node between pre and cur->val
    Node* pre = head_;
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            Node* next = cur->get_next();
            if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
                if (pre->is_mark()) {
                    return false;
                }
            }
            cur = pre->get_next();
            continue;
        }
        if (val <= cur->val) {
            break;
        }
        pre = cur;
        cur = pre->get_next();
    }

    if (cur == NULL) {
        Node* node = new Node(val, NULL);
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, node)) {
            delete node;
            return false;
        }
        return true;
    }

    if (val == cur->val) {
        return false;
    } else { // val < cur->val
        Node* node = new Node(val, cur);
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, node)) {
            delete node;
            return false;
        }
        return true;
    }
}

bool LockFreeList::rm(const long val) {
    // delete node with val == cur->val
    Node* pre = head_;
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            Node* next = cur->get_next();
            if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
                // pre node has been delete logically
                if (pre->is_mark()) {
                    return false;
                }
                // cur node has been delete physically
                // need do nothing
            }
            cur = pre->get_next();
            continue;
        }
        if (val <= cur->val) {
            break;
        }
        pre = cur;
        cur = pre->get_next();
    }

    if (cur == NULL) {
        return false;
    }

    if (val == cur->val) {
        cur->mark();
        Node* next = cur->get_next();
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
            //std::cout << "marked node has been deleted by other thread" << std::endl;
        }
        return true;
    } else { // val < cur->val
        return false;
    }
}

bool LockFreeList::contains(const long val) {
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            cur = cur->get_next();
            continue;
        }
        if (val <= cur->val) {
            break;
        }
        cur = cur->get_next();
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
        if (cur->is_mark()) {
            cur = cur->get_next();
            continue;
        }
        v.push_back(cur->val);
        cur = cur->get_next();
    }
    return v;
}