/*
* Please use tools to check whether
* memory leak exists
*/
#ifndef _RCU_H
#define _RCU_H

#include <list>
#include <map>
#include <atomic>
#include <unistd.h> // usleep
#include <pthread.h>

#include "list_node.h"

struct ThreadItem {
    unsigned int thread_id;
    unsigned int epoch;
    ThreadItem() {
        thread_id = -1;
        epoch = -1;
    }
    ThreadItem(const unsigned int tid, const unsigned int epoch) {
        this->thread_id = tid;
        this->epoch = epoch;
    }
};

struct NodeItem {
    Node* node;
    unsigned int epoch;
    NodeItem() {
        node = NULL;
        epoch = -1;
    }
    NodeItem(Node* node, unsigned int epoch) {
        this->node = node;
        this->epoch = epoch;
    }
};

// head --> ... --> tail
// oldest --> ... --> newest
class RCU {
public:
    RCU();
    ~RCU();

    /*----------- interface ------------*/
    // not thread safe
    void start_bg_reclaim_thread();

    /*----------- interface ------------*/
    // thread safe
    void add_thread(const unsigned int tid);
    void rm_thread(const unsigned int tid);
    void add_reclaim_resource(Node* node);       

public:
    // do resource reclaiming
    void run_bg_reclaim_thread();
    static void* run_bg_reclaim_thread_wrapper(void* argv) {
        ((RCU*)argv)->run_bg_reclaim_thread();
        return NULL;
    }

    // debug
    // not thread safe
    void print_ds();

    // threads that running now
    std::list<ThreadItem> threads_;
    std::map<unsigned int, std::list<ThreadItem>::iterator> thread_index_;

    // nodes that need to be reclaimed
    std::list<NodeItem> nodes_;

    std::atomic<unsigned int> epoch_; // bug

    // this mutex save all data
    pthread_mutex_t mutex_;

    const unsigned int SleepInterval = 1000 * 50;
};

#endif