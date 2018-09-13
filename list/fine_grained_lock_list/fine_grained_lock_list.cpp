#include "fine_grained_lock_list.h"

FineGrainedLockList::FineGrainedLockList() {
    head_ = new Node();
}

FineGrainedLockList::~FineGrainedLockList() {

}

bool FineGrainedLockList::add(const long val) {
    Node* pre = head_;
    pre->lock();
    while (pre->next) {
        Node* cur = pre->next;
        cur->lock();
        if (val == cur->val) {
            cur->unlock();
            pre->unlock();
            return false;
        }
        if (val < cur->val) {
            Node* node = new Node(val, cur);
            pre->next = node;
            cur->unlock();
            pre->unlock();
            return true;
        }
        pre->unlock();
        pre = cur;
    }

    Node* node = new Node(val, NULL);
    pre->next = node;
    pre->unlock();
    return true;
}

std::vector<long> FineGrainedLockList::vectorize() {
    std::vector<long> v;
    Node* p = head_;
    while (p->next) {
        v.push_back(p->next->val);
        p = p->next;
    }
    return v;
}