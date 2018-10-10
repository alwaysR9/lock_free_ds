#include <assert.h>

#include "rcu.h"

RCU::RCU() {
    epoch_ = 0;
    is_over_.store(false);
    should_over_.store(false);
    pthread_mutex_init(&mutex_, NULL);
}
RCU::~RCU() {
    // kill the background thread
    is_over_.store(true);
    pthread_join(bg_tid_, NULL);
    // reclaim resource
    while (true) {
        if (should_over_.load()) {
            // reclaim resource
            threads_.clear();
            thread_index_.clear();
            for (std::list<NodeItem>::iterator i = nodes_.begin();
                 i != nodes_.end(); ++ i) {
                delete i->node;
            }
            nodes_.clear();
            node_hash_.clear();
            pthread_mutex_destroy(&mutex_);
            assert(this->get_thread_queue_size() == 0);
            assert(this->get_thread_index_size() == 0);
            assert(this->get_resource_queue_size() == 0);
            break;
        } else {
            usleep(1000 * 10);
        }
    }
}

void RCU::run_bg_reclaim_thread() {
    while (!is_over_.load()) {
        usleep(SleepInterval);
        ++ epoch_;

        assert(pthread_mutex_lock(&mutex_) == 0);
        //print_ds();

        std::list<ThreadItem>::iterator earlist_thread = threads_.begin();

        for (std::list<NodeItem>::iterator node_item = nodes_.begin();
             node_item != nodes_.end(); ) {
            if (earlist_thread != threads_.end()) {
                if (node_item->epoch < earlist_thread->epoch) {
                    //std::cout << "[ rm node: " << node_item->node->val << " ]";
                    //std::cout << "[ node_epoch: " << node_item->epoch << ", thread_epoch: " << earlist_thread->epoch << " ]" << std::endl;
                    delete node_item->node;
                    node_hash_.erase(node_item->node);
                    node_item = nodes_.erase(node_item);
                } else {
                    break;
                }
            } else {
                //std::cout << "[[ there is no threads now, rm node " << node_item->node->val << " ]]" << std::endl;
                delete node_item->node;
                node_hash_.erase(node_item->node);
                node_item = nodes_.erase(node_item);
            }
        }
        assert(pthread_mutex_unlock(&mutex_) == 0);
    }
    should_over_.store(true);
}

void RCU::start_bg_reclaim_thread() {
    assert(pthread_create(&bg_tid_, NULL, &RCU::run_bg_reclaim_thread_wrapper, this) == 0);
}

void RCU::add_thread(const unsigned int tid) {
    assert(pthread_mutex_lock(&mutex_) == 0);
    threads_.push_back(ThreadItem(tid, epoch_));
    
    std::list<ThreadItem>::iterator new_thread_item = threads_.end();
    -- new_thread_item;
    thread_index_[new_thread_item->thread_id] = new_thread_item;
    assert(pthread_mutex_unlock(&mutex_) == 0);
}

void RCU::rm_thread(const unsigned int tid) {
    assert(pthread_mutex_lock(&mutex_) == 0);
    assert(thread_index_.find(tid) != thread_index_.end());
    std::list<ThreadItem>::iterator del_thread_item = thread_index_[tid];
    threads_.erase(del_thread_item);
    thread_index_.erase(tid);
    assert(pthread_mutex_unlock(&mutex_) == 0);
}

void RCU::add_reclaim_resource(Node* node) {
    assert(pthread_mutex_lock(&mutex_) == 0);
    if (node_hash_.find(node) == node_hash_.end()) {
        nodes_.push_back(NodeItem(node, epoch_));
        node_hash_.insert(node);
    }
    assert(pthread_mutex_unlock(&mutex_) == 0);
}

// for debug
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

unsigned long int RCU::get_thread_queue_size() {
    return threads_.size();
}
unsigned long int RCU::get_thread_index_size() {
    return thread_index_.size();
}
unsigned long int RCU::get_resource_queue_size() {
    return nodes_.size();
}