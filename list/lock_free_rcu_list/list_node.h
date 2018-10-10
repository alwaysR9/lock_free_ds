#ifndef _LIST_NODE_H
#define _LIST_NODE_H

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

#endif