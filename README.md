Lock-Free List
--------------

### 背景
随内存容量的增大，机器核心数增多，内存型数据库开始得到关注。与磁盘数据库的不同在于，锁操作相比内存操作代价较高。<br>
本项目使用链表实现的Set，模拟内存数据库索引，对比了多种同步方式的性能。<br>
包含以下几种同步方式，参考https://people.eecs.berkeley.edu/~stephentu/presentations/workshop.pdf：<br>
    - 粗力度锁（coarse_grained_lock_list）<br>
    - 细力度锁（fine_grained_lock_list）<br>
    - 无锁（lock_free_list）<br>
    - 无锁+垃圾回收（lock_free_rcu_list）<br>

### 效果对比