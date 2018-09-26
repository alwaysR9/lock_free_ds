#include "list_node.h"

Node::Node(long val, Node* next) {
    this->val = val;
    this->next_ = next;
}

bool Node::is_mark() {
    return (unsigned long)next_.load() & (unsigned long)0x1;
}

void Node::mark() {
    next_ = (Node*) ((unsigned long)next_.load() | (unsigned long)0x1);
}

Node* Node::get_next() {
    return (Node*) ((unsigned long)next_.load() & ~(unsigned long)0x1);
}