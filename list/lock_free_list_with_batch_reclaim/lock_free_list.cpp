#include "lock_free_list.h"

#include <assert.h>

LockFreeList::LockFreeList() {
    head_ = new Node(-1, NULL);

    rcu_ = new RCU();
    rcu_->start_bg_reclaim_thread();
}

LockFreeList::~LockFreeList() {
    while (head_->get_next() != NULL) {
        Node* node = head_->get_next();
        head_->next_ = node->get_next();
        delete node;
    }
    delete head_;

    rcu_->kill_bg_reclaim_thread();
}

bool LockFreeList::add(const long val) {
    // insert node between pre and cur->val
    unsigned int tid = pthread_self();
    rcu_->add_thread(tid);

    Node* pre = head_;
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            Node* next = cur->get_next();
            if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
                if (pre->is_mark()) {
                    rcu_->rm_thread(tid);
                    return false;
                }
            }
            // remove cur node from list logically,
            // so add cur node for batch reclaimming
            rcu_->add_reclaim_resource(cur);
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
            rcu_->rm_thread(tid);
            return false;
        }
        rcu_->rm_thread(tid);
        return true;
    }

    if (val == cur->val) {
        rcu_->rm_thread(tid);
        return false;
    } else { // val < cur->val
        Node* node = new Node(val, cur);
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, node)) {
            delete node;
            rcu_->rm_thread(tid);
            return false;
        }
        rcu_->rm_thread(tid);
        return true;
    }
}

bool LockFreeList::rm(const long val) {
    // delete node with val == cur->val
    unsigned int tid = pthread_self();
    rcu_->add_thread(tid);

    Node* pre = head_;
    Node* cur = head_->next_;

    while (cur != NULL) {
        if (cur->is_mark()) {
            Node* next = cur->get_next();
            if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
                // pre node has been delete logically
                if (pre->is_mark()) {
                    rcu_->rm_thread(tid);
                    return false;
                }
                // cur node has been delete physically
                // need do nothing
            }
            // remove cur node from list logically,
            // so add cur node for batch reclaimming
            rcu_->add_reclaim_resource(cur);
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
        rcu_->rm_thread(tid);
        return false;
    }

    if (val == cur->val) {
        cur->mark();
        Node* next = cur->get_next();
        if (!std::atomic_compare_exchange_strong(&pre->next_, &cur, next)) {
            // cur node has been removed from list logically
        }
        // remove cur node from list logically,
        // so add cur node for batch reclaimming
        rcu_->add_reclaim_resource(cur);
        rcu_->rm_thread(tid);
        return true;
    } else { // val < cur->val
        rcu_->rm_thread(tid);
        return false;
    }
}

bool LockFreeList::contains(const long val) {
    unsigned int tid = pthread_self();
    rcu_->add_thread(tid);

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
        rcu_->rm_thread(tid);
        return false;
    }

    if (val == cur->val) {
        rcu_->rm_thread(tid);
        return true;
    } else {
        rcu_->rm_thread(tid);
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