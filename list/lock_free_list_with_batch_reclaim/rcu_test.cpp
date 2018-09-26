#include <pthread.h>
#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

#include "list_node.h"
#include "rcu.h"

struct ThreadArgv {
    Node* node; 
    RCU* rcu;
    pthread_mutex_t* mutex;
    ThreadArgv() {
        node = NULL;
        rcu = NULL;
        mutex = NULL;
    }
    void init(Node* node, RCU* rcu, pthread_mutex_t* mutex) {
        this->node = node;
        this->rcu = rcu;
        this->mutex = mutex;
    }
};

void* del_thread(void* argv) {
    ThreadArgv* t_argv = (ThreadArgv*) argv;
    unsigned int tid = (unsigned int) pthread_self();
    assert(pthread_mutex_lock(t_argv->mutex) == 0);
    int n = rand() % 1000;
    assert(pthread_mutex_unlock(t_argv->mutex) == 0);

    usleep(n*1000);
    t_argv->rcu->add_thread(tid);
    usleep(n*1000);
    t_argv->rcu->add_reclaim_resource(t_argv->node);
    usleep(n*1000);
    t_argv->rcu->rm_thread(tid);
}

void* norm_thread(void* argv) {
    ThreadArgv* t_argv = (ThreadArgv*) argv;
    unsigned int tid = (unsigned int) pthread_self();
    assert(pthread_mutex_lock(t_argv->mutex) == 0);
    int n = rand() % 1000;
    assert(pthread_mutex_unlock(t_argv->mutex) == 0);

    usleep(n*1000);
    t_argv->rcu->add_thread(tid);
    usleep(n*1000);
    t_argv->rcu->rm_thread(tid);
}

void TEST(const int n_del_thread, const int n_norm_thread) {
    RCU rcu;
    rcu.start_bg_reclaim_thread();

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    ThreadArgv* argv1 = new ThreadArgv[n_del_thread];
    for (int i = 0; i < n_del_thread; ++ i) {
        argv1[i].init(new Node((long)(i+1), NULL), &rcu, &mutex);
    }
    ThreadArgv* argv2 = new ThreadArgv[n_norm_thread];
    for (int i = 0; i < n_norm_thread; ++ i) {
        argv2[i].init(NULL, &rcu, &mutex);
    }

    // create delete thread
    pthread_t* tid1 = new pthread_t[n_del_thread];
    for (int i = 0; i < n_del_thread; ++ i) {
        assert(pthread_create(&tid1[i], NULL, del_thread, (void*)&argv1[i]) == 0);
    }

    // create empty thread
    pthread_t* tid2 = new pthread_t[n_norm_thread];
    for (int i = 0; i < n_norm_thread; ++ i) {
        assert(pthread_create(&tid2[i], NULL, norm_thread, (void*)&argv2[i]) == 0);
    }

    usleep(1000 * 5000);

    for (int i = 0; i < n_norm_thread; ++ i) {
        assert(pthread_join(tid2[i], NULL) == 0);
    }
    for (int i = 0; i < n_del_thread; ++ i) {
        assert(pthread_join(tid1[i], NULL) == 0);
    }

    delete [] argv1;
    delete [] tid1;
    delete [] argv2;
    delete [] tid2;
    pthread_mutex_destroy(&mutex);

    assert(rcu.threads_.size() == 0);
    assert(rcu.thread_index_.size() == 0);
    assert(rcu.nodes_.size() == 0);
    std::cout << "SUCCESS" << std::endl;
}

int main() {
    srand(time(NULL));
    TEST(100, 100);
    return 0;
}