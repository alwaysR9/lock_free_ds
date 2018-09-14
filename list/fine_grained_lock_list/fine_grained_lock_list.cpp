#include "fine_grained_lock_list.h"

int MUTEX_TYPE = 1;

//------------------- lock -------------------//
Mutex::Mutex() {
    pthread_mutex_init(&this->mu, NULL);
}
Mutex::~Mutex() {
    pthread_mutex_destroy(&this->mu);
}
void Mutex::lock() {
    assert(pthread_mutex_lock(&this->mu) == 0);
}
void Mutex::unlock() {
    assert(pthread_mutex_unlock(&this->mu) == 0);
}

SpinLock::SpinLock() {
    pthread_spin_init(&this->mu, PTHREAD_PROCESS_PRIVATE);
}
SpinLock::~SpinLock() {
    pthread_spin_destroy(&this->mu);
}
void SpinLock::lock() {
    assert(pthread_spin_lock(&this->mu) == 0);
}
void SpinLock::unlock() {
    assert(pthread_spin_unlock(&this->mu) == 0);
}

//--------------------- Node ---------------------//
Node::Node() {}
Node::Node(long val, Node* next, int mutex_type) {
    this->val = val;
    this->next = next;
    if (mutex_type == 0) {
        this->mutex = new Mutex();
    } else if (mutex_type == 1){
        this->mutex = new SpinLock();
    }
}
Node::~Node() {
    delete this->mutex;
}
void Node::lock() {
    this->mutex->lock();
}
void Node::unlock() {
    this->mutex->unlock();
}

//--------------------- list ---------------------//
FineGrainedLockList::FineGrainedLockList() {
    head_ = new Node(0, NULL, MUTEX_TYPE);
}

FineGrainedLockList::~FineGrainedLockList() {
    while (head_->next) {
        Node* node = head_->next;
        head_->next = node->next;
        delete node;
    }
    delete head_;
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
            Node* node = new Node(val, cur, MUTEX_TYPE);
            pre->next = node;
            cur->unlock();
            pre->unlock();
            return true;
        }
        pre->unlock();
        pre = cur;
    }

    Node* node = new Node(val, NULL, MUTEX_TYPE);
    pre->next = node;
    pre->unlock();
    return true;
}

bool FineGrainedLockList::rm(const long val) {
    Node* pre = head_;
    pre->lock();
    while (pre->next) {
        Node* cur = pre->next;
        cur->lock();
        if (val == cur->val) {
            pre->next = cur->next;
            cur->unlock();
            delete cur;
            pre->unlock();
            return true;
        }
        if (val < cur->val) {
            cur->unlock();
            pre->unlock();
            return false;
        }
        pre->unlock();
        pre = cur;
    }
    pre->unlock();
    return false;
}

bool FineGrainedLockList::contains(const long val) {
    Node* pre = head_;
    pre->lock();
    while (pre->next) {
        Node* cur = pre->next;
        cur->lock();
        pre->unlock(); // it is safe
        if (val == cur->val) {
            cur->unlock();
            return true;
        }
        if (val > cur->val) {
            pre = cur;
        } else {
            cur->unlock();
            return false;
        }
    }
    pre->unlock();
    return false;
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