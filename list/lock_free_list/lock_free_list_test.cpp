#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <sys/time.h>
#include <atomic>

#include "lock_free_list.h"

// Test Node class
//int main() {
//    
//    Node x(100, NULL);
//    std::cout << "is mark: " << x.is_mark() << std::endl;    
//    
//    x.mark();
//    std::cout << "after mark: " << x.is_mark() << std::endl;
//    std::cout << "get next: " << x.get_next() << std::endl;
//
//    return 0;
//}

struct ThreadArgv {
    LockFreeList* pl;
    int b, e;
    ThreadArgv() {}
    ThreadArgv(LockFreeList* pl, int b, int e) {
        this->pl = pl;
        this->b = b;
        this->e = e;
    }
    void init(LockFreeList* pl, int b, int e) {
        this->pl = pl;
        this->b = b;
        this->e = e;
    }
};

void TEST_CORRECTNESS_SINGLE_THREAD() {
    LockFreeList l;
    std::vector<long> v;

    l.add(1), l.add(3), l.add(2), l.add(3);
    v = l.vectorize();
    assert (v.size() == 3);
    assert (v[0] == 1);
    assert (v[1] == 2);
    assert (v[2] == 3);

    l.rm(1), l.rm(2), l.rm(2);
    v = l.vectorize();
    assert (v.size() == 1);
    assert (v[0] == 3);

    bool res1 = l.contains(3);
    bool res2 = l.contains(1);
    assert(res1 == true);
    assert(res2 == false);

    std::cout << "Test single thread correctness successfully" << std::endl;

    std::cout << "--------------------------" << std::endl;
}

void* test_add(void* argv) {
    LockFreeList* pl = ((ThreadArgv*) argv)->pl;
    int b = ((ThreadArgv*) argv)->b;
    int e = ((ThreadArgv*) argv)->e;
    assert (b != e);
    long dir = 1;
    if (b > e) {
        dir = -1;
    }
    for (int i = b; ; i += dir) {
        if (dir > 0) {
            if (i > e) break;
        }
        if (dir < 0) {
            if (i < e) break;
        }
        pl->add((long)i);
    }
    return NULL;
}

void* test_rm(void* argv) {
    LockFreeList* pl = ((ThreadArgv*) argv)->pl;
    int b = ((ThreadArgv*) argv)->b;
    int e = ((ThreadArgv*) argv)->e;
    assert (b != e);
    long dir = 1;
    if (b > e) {
        dir = -1;
    }
    for (int i = b; ; i += dir) {
        if (dir > 0) {
            if (i > e) break;
        }
        if (dir < 0) {
            if (i < e) break;
        }
        pl->rm((long)i);
    }
    return NULL;
}

void* test_contains(void* argv) {
    LockFreeList* pl = ((ThreadArgv*) argv)->pl;
    int b = ((ThreadArgv*) argv)->b;
    int e = ((ThreadArgv*) argv)->e;
    assert (b != e);
    long dir = 1;
    if (b > e) {
        dir = -1;
    }
    for (int i = b; ; i += dir) {
        if (dir > 0) {
            if (i > e) break;
        }
        if (dir < 0) {
            if (i < e) break;
        }
        pl->contains((long)i);
    }
    return NULL;
}

bool validate_permutations(const std::vector<long> & v) {
    return (v.size() == 0) ||
        (v.size() == 1 && (v[0] == 1 || v[0] == 2)) ||
        (v.size() == 2 && (v[0] == 1 && v[1] == 2));
}

void Test_multi_thread_add() {
    LockFreeList l;
    std::vector<long> v;

    pthread_t tid[4];
    ThreadArgv argv1 = ThreadArgv(&l, 1, 10000);
    ThreadArgv argv2 = ThreadArgv(&l, 10000, 1);
    ThreadArgv argv3 = ThreadArgv(&l, 1000, 8000);
    ThreadArgv argv4 = ThreadArgv(&l, 5000, 1);
    pthread_create(&tid[0], NULL, test_add, (void*)&argv1); 
    pthread_create(&tid[1], NULL, test_add, (void*)&argv2); 
    pthread_create(&tid[2], NULL, test_add, (void*)&argv3); 
    pthread_create(&tid[3], NULL, test_add, (void*)&argv4); 
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);
 
    v = l.vectorize();
    assert(v.size() == 10000);
    assert(v[0] == 1);
    assert(v[9999] == 10000);
}

void Test_multi_thread_rm() {
    LockFreeList l;
    std::vector<long> v;

    for (int i = 0; i < 10000; ++ i) {
        l.add((long)(i+1));
    }

    pthread_t tid[4];
    ThreadArgv argv1 = ThreadArgv(&l, 1, 5000);
    ThreadArgv argv2 = ThreadArgv(&l, 5000, 1);
    ThreadArgv argv3 = ThreadArgv(&l, 2000, 4000);
    ThreadArgv argv4 = ThreadArgv(&l, 4500, 100);
    pthread_create(&tid[0], NULL, test_rm, (void*)&argv1); 
    pthread_create(&tid[1], NULL, test_rm, (void*)&argv2); 
    pthread_create(&tid[2], NULL, test_rm, (void*)&argv3); 
    pthread_create(&tid[3], NULL, test_rm, (void*)&argv4); 
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);
 
    v = l.vectorize();
    assert(v.size() == 5000);
    assert(v[0] == 5001);
    assert(v[4999] == 10000);
}

void Test_multi_thread_add_and_rm_small() {
    LockFreeList l;
    std::vector<long> v;

    pthread_t tid[4];
    ThreadArgv argv1 = ThreadArgv(&l, 1, 2);
    ThreadArgv argv2 = ThreadArgv(&l, 2, 1);
    pthread_create(&tid[0], NULL, test_add, (void*)&argv1); 
    pthread_create(&tid[1], NULL, test_rm, (void*)&argv2); 
    pthread_create(&tid[2], NULL, test_add, (void*)&argv2); 
    pthread_create(&tid[3], NULL, test_rm, (void*)&argv1); 
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_join(tid[3], NULL);
 
    v = l.vectorize();
    assert(validate_permutations(v));
}

void Test_multi_thread_add_and_rm_big() {
    LockFreeList l;

    for (int i = 1; i <= 10000; ++ i) {
        l.add((long) i);
    }

    int n_add_thread = 3;
    int n_rm_thread = 3;
    int n_contains_thread = 2;
    int n_thread = n_add_thread + n_rm_thread + n_contains_thread;

    pthread_t* tid = new pthread_t[n_thread];
    ThreadArgv* argv = new ThreadArgv[n_thread];
    argv[0].init(&l, 1, 10000);
    argv[1].init(&l, 3000, 1);
    argv[2].init(&l, 6000, 3500);
    argv[3].init(&l, 2010, 8999);
    argv[4].init(&l, 3011, 7917);
    argv[5].init(&l, 7138, 1234);
    argv[6].init(&l, 10000, 1);
    argv[7].init(&l, 9216, 4289);

    for (int i = 0; i < n_thread; ++ i) {
        if (i < n_add_thread) {
            pthread_create(&tid[i], NULL, test_add, (void*)&argv[i]); 
        } else if (i >= n_add_thread && i < n_add_thread + n_rm_thread) {
            pthread_create(&tid[i], NULL, test_rm, (void*)&argv[i]); 
        } else {
            pthread_create(&tid[i], NULL, test_contains, (void*)&argv[i]); 
        }
    }

    for (int i = 0; i < n_thread; ++ i) {
        pthread_join(tid[i], NULL);
    }

    delete [] tid;
    delete [] argv;
}

void TEST_CORRECTNESS_MULTI_THREAD() {
    Test_multi_thread_add();
    std::cout << "Test multi thread add successfully" << std::endl;

    Test_multi_thread_rm();
    std::cout << "Test multi thread rm successfully" << std::endl;

    Test_multi_thread_add_and_rm_small();
    std::cout << "Test multi thread add & rm small successfully" << std::endl;
    Test_multi_thread_add_and_rm_big();
    std::cout << "Test multi thread add & rm big successfully" << std::endl;

    std::cout << "--------------------------" << std::endl;
}

double time_diff(const timeval & b, const timeval & e) {
    return (e.tv_sec - b.tv_sec) + (e.tv_usec - b.tv_usec)*1.0 / 1000000.0;
}

void Test_performance_add(const int n_thread) {
    LockFreeList l;
    std::vector<long> v;

    pthread_t* tid = new pthread_t[n_thread];
    ThreadArgv argv = ThreadArgv(&l, 1, 10000);

    timeval begin;
    timeval end;
    gettimeofday(&begin, NULL);

    for (int i = 0; i < n_thread; ++ i) {
        pthread_create(&tid[i], NULL, test_add, (void*)&argv); 
    }
    for (int i = 0; i < n_thread; ++ i) {
        pthread_join(tid[i], NULL);
    }

    gettimeofday(&end, NULL);
    std::cout << "Test performance: add() with " << n_thread;
    std::cout << " threads, consuming " << time_diff(begin, end) << " s" << std::endl;

    delete [] tid;
}

void Test_performance_rm(const int n_thread) {
    LockFreeList l;
    std::vector<long> v;

    for (int i = 1; i <= 10000; ++ i) {
        l.add((long) i);
    }

    pthread_t* tid = new pthread_t[n_thread];
    ThreadArgv argv = ThreadArgv(&l, 1, 10000);

    timeval begin;
    timeval end;
    gettimeofday(&begin, NULL);

    for (int i = 0; i < n_thread; ++ i) {
        pthread_create(&tid[i], NULL, test_rm, (void*)&argv); 
    }
    for (int i = 0; i < n_thread; ++ i) {
        pthread_join(tid[i], NULL);
    }

    gettimeofday(&end, NULL);
    std::cout << "Test performance: rm() with " << n_thread;
    std::cout << " threads, consuming " << time_diff(begin, end) << " s" << std::endl;

    delete [] tid;
}

void Test_performance_contains(const int n_thread) {
    LockFreeList l;
    std::vector<long> v;

    pthread_t* tid = new pthread_t[n_thread];
    ThreadArgv argv = ThreadArgv(&l, 1, 10000);

    for (int i = 0; i < 10000; ++ i) {
        l.add((long)i);
    }

    timeval begin;
    timeval end;
    gettimeofday(&begin, NULL);

    for (int i = 0; i < n_thread; ++ i) {
        pthread_create(&tid[i], NULL, test_contains, (void*)&argv); 
    }
    for (int i = 0; i < n_thread; ++ i) {
        pthread_join(tid[i], NULL);
    }

    gettimeofday(&end, NULL);
    std::cout << "Test performance: contains() with " << n_thread;
    std::cout << " threads, consuming " << time_diff(begin, end) << " s" << std::endl;

    delete [] tid;
}

void Test_performance_multi_op(const int n_add_thread,
                               const int n_rm_thread,
                               const int n_contains_thread) {
    LockFreeList l;
    std::vector<long> v;

    int n_thread = n_add_thread + n_rm_thread + n_contains_thread;
    pthread_t* tid = new pthread_t[n_thread];
    ThreadArgv* argv = new ThreadArgv[3];

    argv[0].init(&l, 1, 10000);
    argv[1].init(&l, 10000, 1);
    argv[2].init(&l, 10000, 1);

    for (int i = 0; i < 10000; ++ i) {
        l.add((long)i);
    }

    timeval begin;
    timeval end;
    gettimeofday(&begin, NULL);

    for (int i = 0; i < n_thread; ++ i) {
        if (i < n_add_thread) {
            pthread_create(&tid[i], NULL, test_add, (void*)&argv[0]);
        } else if (i >= n_add_thread && i < n_add_thread + n_rm_thread) {
            pthread_create(&tid[i], NULL, test_rm, (void*)&argv[1]);
        } else {
            pthread_create(&tid[i], NULL, test_contains, (void*)&argv[2]); 
        }
    }
    for (int i = 0; i < n_thread; ++ i) {
        pthread_join(tid[i], NULL);
    }

    gettimeofday(&end, NULL);
    std::cout << "Test performance: multi op with " << n_thread << "thread, ";
    std::cout << " add_threads: " << n_add_thread << ",";
    std::cout << " rm_threads: " << n_rm_thread << ",";
    std::cout << " contains_threads: " << n_contains_thread << ",";
    std::cout << " consuming " << time_diff(begin, end) << " s" << std::endl;

    delete [] tid;
    delete [] argv;
}

void TEST_PERFORMANCE() {
    Test_performance_add(1);
    Test_performance_add(5);
    Test_performance_add(10);
    Test_performance_add(20);
    Test_performance_add(30);
    std::cout << "Test add performence successfully" << std::endl;

    Test_performance_rm(1);
    Test_performance_rm(5);
    Test_performance_rm(10);
    Test_performance_rm(20);
    Test_performance_rm(30);
    std::cout << "Test rm performence successfully" << std::endl;

    Test_performance_contains(1);
    Test_performance_contains(5);
    Test_performance_contains(10);
    Test_performance_contains(20);
    Test_performance_contains(30);
    std::cout << "Test contains performence successfully" << std::endl;

    Test_performance_multi_op(1+1, 0, 8);
    Test_performance_multi_op(4+4, 0, 2);

    Test_performance_multi_op(2+2, 0, 16);
    Test_performance_multi_op(8+8, 0, 4);

    Test_performance_multi_op(3+3, 0, 24);
    Test_performance_multi_op(12+12, 0, 6);
    std::cout << "Test multi op performence successfully" << std::endl;

    std::cout << "--------------------------" << std::endl;
}

int main() {
    
    TEST_CORRECTNESS_SINGLE_THREAD();
    TEST_CORRECTNESS_MULTI_THREAD();
    TEST_PERFORMANCE();

    /*LockFreeList l;

    pthread_t tid[4];
    ThreadArgv argv1 = ThreadArgv(&l, 1, 2);
    pthread_create(&tid[0], NULL, test_rm, (void*)&argv1);
    pthread_join(tid[0], NULL);

    std::vector<long> v = l.vectorize();
    for (int i = 0; i < v.size(); ++ i) {
        std::cout << v[i] << std::endl;
    }*/

    return 0;
}