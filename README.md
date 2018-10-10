Lock-Free List
--------------

### 背景
随内存容量的增大，机器核心数增多，内存型数据库开始得到关注。与磁盘数据库的不同在于，锁操作相对内存操作耗时很高。
本项目对比了多种同步方式在内存操作下的性能，使用链表实现的Set模拟内存型数据库索引。
包含以下几种同步方式，参考https://people.eecs.berkeley.edu/~stephentu/presentations/workshop.pdf：<br>
    - 粗力度锁（coarse_grained_lock_list）
    - 细力度锁（fine_grained_lock_list）
    - 无锁（lock_free_list）
    - 无锁+垃圾回收（lock_free_rcu_list）

### 效果对比