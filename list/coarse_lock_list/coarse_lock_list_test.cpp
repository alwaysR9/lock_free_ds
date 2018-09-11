#include <iostream>
#include <assert.h>
#include <sys/time.h>

#include "coarse_lock_list.h"

struct ThreadArgv {
    CoarseLockList* pl;
    int b, e;
    ThreadArgv() {}
    ThreadArgv(CoarseLockList* pl, int b, int e) {
        this->pl = pl;
        this->b = b;
        this->e = e;
    }
};

void TEST_CORRECTNESS_SINGLE_THREAD() {
    CoarseLockList l;
    std::vector<long> v;

    l.add(1), l.add(2), l.add(3), l.add(3);
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
    CoarseLockList* pl = ((ThreadArgv*) argv)->pl;
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
    CoarseLockList* pl = ((ThreadArgv*) argv)->pl;
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
    CoarseLockList* pl = ((ThreadArgv*) argv)->pl;
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
        (v.size() == 1 && (v[0] == 1 || v[1] == 2)) ||
        (v.size() == 2 && (v[0] == 1 && v[1] == 2));
}

void Test_multi_thread_add() {
    CoarseLockList l;
    std::vector<long> v;

    pthread_t tid[2];
    ThreadArgv argv1 = ThreadArgv(&l, 1, 100);
    ThreadArgv argv2 = ThreadArgv(&l, 100, 1);
    pthread_create(&tid[0], NULL, test_add, (void*)&argv1); 
    pthread_create(&tid[1], NULL, test_add, (void*)&argv2); 
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
 
    v = l.vectorize();
    assert(v.size() == 100);
    assert(v[0] == 1);
    assert(v[99] == 100);
}

void Test_multi_thread_rm() {
    CoarseLockList l;
    std::vector<long> v;

    for (int i = 0; i < 100; ++ i) {
        l.add((long)(i+1));
    }

    pthread_t tid[2];
    ThreadArgv argv1 = ThreadArgv(&l, 1, 50);
    ThreadArgv argv2 = ThreadArgv(&l, 50, 1);
    pthread_create(&tid[0], NULL, test_rm, (void*)&argv1); 
    pthread_create(&tid[1], NULL, test_rm, (void*)&argv2); 
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
 
    v = l.vectorize();
    assert(v.size() == 50);
    assert(v[0] == 51);
    assert(v[49] == 100);
}

void Test_multi_thread_add_and_rm() {
    CoarseLockList l;
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

void TEST_CORRECTNESS_MULTI_THREAD() {
    Test_multi_thread_add();
    std::cout << "Test multi thread add successfuly" << std::endl;

    Test_multi_thread_rm();
    std::cout << "Test multi thread rm successfuly" << std::endl;

    Test_multi_thread_add_and_rm();
    std::cout << "Test multi thread add & rm successfuly" << std::endl;

    std::cout << "--------------------------" << std::endl;
}

double time_diff(const timeval & b, const timeval & e) {
    return (e.tv_sec - b.tv_sec) + (e.tv_usec - b.tv_usec)*1.0 / 1000000.0;
}

void Test_performance_add(const int n_thread) {
    CoarseLockList l;
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

void Test_performance_contains(const int n_thread) {
    CoarseLockList l;
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

void TEST_PERFORMANCE() {
    Test_performance_add(1);
    Test_performance_add(10);
    Test_performance_add(20);
    Test_performance_add(30);
    std::cout << "Test add performence successfully" << std::endl;

    Test_performance_contains(1);
    Test_performance_contains(10);
    Test_performance_contains(20);
    Test_performance_contains(30);
    std::cout << "Test contains performence successfully" << std::endl;

    std::cout << "--------------------------" << std::endl;
}

int main() {
    
    TEST_CORRECTNESS_SINGLE_THREAD();
    TEST_CORRECTNESS_MULTI_THREAD();
    TEST_PERFORMANCE();

    /*CoarseLockList l;

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