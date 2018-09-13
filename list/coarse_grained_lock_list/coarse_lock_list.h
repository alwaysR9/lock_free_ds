#ifndef _COARSE_LOCK_LIST_H
#define _COARSE_LOCK_LIST_H

#include <vector>
#include <pthread.h>

struct Node;

// Elements sorted from smallest to largest
class CoarseLockList {
public:
    CoarseLockList();
    ~CoarseLockList();

    /**************** Interface ****************/
    // Thread safe
    bool add(const long val);
    bool rm(const long val);
    bool contains(const long val);

    /**************** Test ****************/
    // Not thread safe
    std::vector<long> vectorize();

private:
    // _head is an empty node,
    // _head->val is invalid
    Node* _head;
    pthread_mutex_t _mutex;
};

#endif