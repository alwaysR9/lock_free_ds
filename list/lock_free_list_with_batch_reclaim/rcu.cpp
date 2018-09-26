#include <assert.h>

#include "rcu.h"

RCU::RCU() {
    epoch_ = 0;
    pthread_mutex_init(&mutex_, NULL);
}
RCU::~RCU() {
    pthread_mutex_destroy(&mutex_);
}

void RCU::print_ds() {
    if (threads_.size() > 0) {
        std::cout << "Alive Threads: ";
        for (std::list<ThreadItem>::iterator i = threads_.begin();
             i != threads_.end(); ++ i) {
            std::cout << "tid = " << i->thread_id << ": epoch = " << i->epoch << "\t"; 
        }
        std::cout << std::endl;
    }

    if (nodes_.size() > 0) {
        std::cout << "Need Reclaim Node: ";
        for (std::list<NodeItem>::iterator i = nodes_.begin();
             i != nodes_.end(); ++ i) {
            std::cout << "node = " << i->node->val << ": epoch = " << i->epoch << "\t";
        }
        std::cout << std::endl;
    }
}

void RCU::run_bg_reclaim_thread() {
    while (true) {
        usleep(SleepInterval);
        ++ epoch_;

        assert(pthread_mutex_lock(&mutex_) == 0);
        //print_ds();

        std::list<ThreadItem>::iterator earlist_thread = threads_.begin();

        for (std::list<NodeItem>::iterator node_item = nodes_.begin();
             node_item != nodes_.end(); ) {
            if (earlist_thread != threads_.end()) {
                if (node_item->epoch < earlist_thread->epoch) {
                    std::cout << "[ rm node: " << node_item->node->val << " ]";
                    std::cout << "[ node_epoch: " << node_item->epoch << ", thread_epoch: " << earlist_thread->epoch << " ]" << std::endl;
                    delete node_item->node;
                    node_item = nodes_.erase(node_item);
                } else {
                    break;
                }
            } else {
                std::cout << "[[ there is no threads now, rm node " << node_item->node->val << " ]]" << std::endl;
                delete node_item->node;
                node_item = nodes_.erase(node_item);
            }
        }
        assert(pthread_mutex_unlock(&mutex_) == 0);
    }
}

void RCU::start_bg_reclaim_thread() {
    pthread_t tid;
    assert(pthread_create(&tid, NULL, &RCU::run_bg_reclaim_thread_wrapper, this) == 0);
    std::cout << "Start background reclaim thread" << std::endl;
}

void RCU::add_thread(const unsigned int tid) {
    assert(pthread_mutex_lock(&mutex_) == 0);
    threads_.push_back(ThreadItem(tid, epoch_));
    
    std::list<ThreadItem>::iterator new_thread_item = threads_.end();
    -- new_thread_item;
    thread_index_[new_thread_item->thread_id] = new_thread_item;
    //std::cout << "Begin a new thread" << std::endl;
    assert(pthread_mutex_unlock(&mutex_) == 0);
}

void RCU::rm_thread(const unsigned int tid) {
    assert(pthread_mutex_lock(&mutex_) == 0);
    assert(thread_index_.find(tid) != thread_index_.end());
    std::list<ThreadItem>::iterator del_thread_item = thread_index_[tid];
    threads_.erase(del_thread_item);
    thread_index_.erase(tid);
    //std::cout << "leave a thread" << std::endl;
    assert(pthread_mutex_unlock(&mutex_) == 0);
}

void RCU::add_reclaim_resource(Node* node) {
    assert(pthread_mutex_lock(&mutex_) == 0);
    nodes_.push_back(NodeItem(node, epoch_));
    //std::cout << "declare a node as need to be deleted: " << node->val << std::endl;
    assert(pthread_mutex_unlock(&mutex_) == 0);
}