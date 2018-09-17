#ifndef _LOCK_FREE_LIST_H
#define _LOCK_FREE_LIST_H

class Node {
public:
    Node() {}
    Node(long val, Node* next);
    ~Node() {}

    bool is_mark();
    void mark();
    Node* get_next();
private:
    long val;
    Node* next_;
};

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