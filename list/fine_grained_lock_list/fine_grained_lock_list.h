#ifndef _FINE_GRAINED_LOCK_LIST_H
#define _FINE_GRAINED_LOCK_LIST_H

#include <iostream>
#include <vector>
#include <assert.h>
#include <pthread.h>

struct Node {
    long val;
    Node* next;
    pthread_mutex_t mutex;

    Node() {
        this->val = 0;
        this->next = NULL;
        pthread_mutex_init(&mutex, NULL);
    }
    Node(long val, Node* next) {
        this->val = val;
        this->next = next;
        pthread_mutex_init(&mutex, NULL);
    }
    ~Node() {
        pthread_mutex_destroy(&mutex);
    }

    void lock() {
        assert(pthread_mutex_lock(&mutex) == 0);
    }
    void unlock() {
        assert(pthread_mutex_unlock(&mutex) == 0);
    }
};

class FineGrainedLockList {
public:
    FineGrainedLockList();
    ~FineGrainedLockList();

    /**************** Interface ****************/
    // Thread safe
    bool add(const long val);
    //bool rm(const long val);
    //bool contains(const long val);

    /**************** Test ****************/
    // Not thread safe
    std::vector<long> vectorize();

private:
    // _head is an empty node,
    // _head->val is invalid
    Node* head_;
};

#endif