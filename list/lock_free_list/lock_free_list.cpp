#include "lock_free_list.h"

Node::Node(long val, Node* next) {
    this->val = val;
    this->next_ = next;
}

bool Node::is_mark() {
    return (long)next_ & (long)0x1;
}

void Node::mark() {
    next_ = (Node*) ((long)next_ | (long)0x1);
}

Node* Node::get_next() {
    return (Node*) ((long)next_ & ~(long)0x1);
}

LockFreeList::LockFreeList() {
    head_ = new Node(0, NULL);
}

LockFreeList::~LockFreeList() {

}

bool LockFreeList::add(const long val) {
    
}