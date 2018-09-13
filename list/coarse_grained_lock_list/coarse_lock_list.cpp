#include "coarse_lock_list.h"

struct Node {
    long val;
    Node* next;
    Node() {
        val = 0;
        next = NULL;
    }
    Node(long val, Node* next) {
        this->val = val;
        this->next = next;
    }
};

CoarseLockList::CoarseLockList() {
    _head = new Node();
    pthread_mutex_init(&_mutex, NULL);
}

CoarseLockList::~CoarseLockList() {
    pthread_mutex_destroy(&_mutex);

    Node* cur = _head;
    while (cur != NULL) {
        Node* next = cur->next;
        delete cur;
        cur = next;
    } 
}

std::vector<long> CoarseLockList::vectorize() {
    std::vector<long> v;
    Node* cur = _head->next;
    while (cur != NULL) {
        v.push_back(cur->val);
        cur = cur->next;
    } 
    return v;
}

bool CoarseLockList::add(const long val) {
    pthread_mutex_lock(&_mutex);
    Node** cur = &(_head->next);
    while (*cur != NULL && (*cur)->val < val) {
        cur = &((*cur)->next);
    }

    bool succ = true;
    if (*cur == NULL || (*cur)->val > val) {
        Node* node = new Node(val, *cur);
        *cur = node;
    } else {
        succ = false;
    }
    pthread_mutex_unlock(&_mutex);

    return succ;
}

bool CoarseLockList::rm(const long val) {
    pthread_mutex_lock(&_mutex);
    Node** cur = &(_head->next);
    while (*cur != NULL && (*cur)->val != val) {
        cur = &((*cur)->next);
    }
    
    bool succ = true;
    if (*cur == NULL) {
        succ = false;
    } else {
        Node* node = *cur;
        *cur = (*cur)->next;
        delete node;
    }
    pthread_mutex_unlock(&_mutex);
    
    return succ;
}

bool CoarseLockList::contains(const long val) {
    pthread_mutex_lock(&_mutex);
    Node** cur = &(_head->next);
    while (*cur != NULL && (*cur)->val != val) {
        cur = &((*cur)->next);
    }
    
    bool succ = true;
    if (*cur == NULL) {
        succ = false;
    }
    pthread_mutex_unlock(&_mutex);
    
    return succ;
}