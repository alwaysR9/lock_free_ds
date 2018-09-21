#ifndef _LOCK_FREE_LIST_H
#define _LOCK_FREE_LIST_H

#include <iostream>
#include <vector>
#include <atomic>

class Node {
public:
    Node() {}
    Node(long val, Node* next);
    ~Node() {}

    bool is_mark();
    void mark();
    Node* get_next();

    long val;
    std::atomic<Node*> next_;
};

/*
* In concurrent mode, multithreads call rm() function,
* some threads will just do delete logically, **so
* after all threads finish, there maybe some logical 
* deleted nodes in the list.** For example:
*   Head->A->B->C->D->NULL
*   timeline 1: thread[0] marks B as deleted
*   timeline 2: thread[1] marks A as deleted
*   timeline 3: thread[0] finds A has been deleted,
*               it returns without delete B physically
*   timeline 4: thread[1] delete A physically
*   After thread[0] and thread[1] finished, the list is:
*   Head->B(logical deleted)->C->D->NULL
* So in any time, the list may contains logical deleted
* nodes.
*/
class LockFreeList {
public:
    LockFreeList();
    ~LockFreeList();

    /**************** Interface ****************/
    // Thread safe
    bool add(const long val);
    bool rm(const long val);
    bool contains(const long val);

    /**************** Test ****************/
    // Not thread safe
    std::vector<long> vectorize();
private:
    Node* head_;
};

#endif