#ifndef _FINE_GRAINED_LOCK_LIST_H
#define _FINE_GRAINED_LOCK_LIST_H

#include <iostream>
#include <vector>
#include <assert.h>
#include <pthread.h>

extern int MUTEX_TYPE;

class MyMutex {
public:
    MyMutex() {};
    virtual ~MyMutex() {};

    //*********** interface ***********//
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

class Mutex : public MyMutex {
public:
    Mutex();
    virtual ~Mutex();

    virtual void lock();
    virtual void unlock();
private:
    pthread_mutex_t mu;
};

class SpinLock : public MyMutex {
public:
    SpinLock();
    virtual ~SpinLock();

    virtual void lock();
    virtual void unlock();
private:
    pthread_spinlock_t mu;
};

class Node {
public:
    long val;
    Node* next;
    MyMutex* mutex;

    Node();
    Node(long val, Node* next, int mutex_type);
    ~Node();

    void lock();
    void unlock();
};

class FineGrainedLockList {
public:
    FineGrainedLockList();
    ~FineGrainedLockList();

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
    Node* head_;
};

#endif