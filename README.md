Lock-Free List
--------------

## 背景
随内存容量的增大，机器核心数增多，内存型数据库开始得到关注。<br>
与磁盘数据库的不同在于，锁操作的代价高昂。<br>
本项目使用链表实现的Set（支持添加，删除，查询），模拟内存型数据库的索引，对比了多种同步方式的性能。<br>
包含以下几种同步方式：<br>
  * 粗力度锁（coarse_grained_lock_list）
    - 任何一次操作前，使用Mutex进行保护，操作完全结束后，释放Mutex
  * 细力度锁（fine_grained_lock_list）
    - 在遍历list时，仅对当前访问的节点和前趋使用Mutex保护
  * 无锁（lock_free_list）
    - 由于增加，删除操作，仅需要修改前趋的next指针，使用原子操作修改前趋的next指针，移除所有锁操作
  * 无锁+垃圾回收（lock_free_rcu_list）
    - 原子操作删除的节点，需要进行内存释放（垃圾回收），在对比了引用计数和批量回收两种方法后，使用批量回收方式

## 效果对比
#### 实验机器配置
8核心，支持8个线程，32GB内存

#### 添加操作
![The Performance of Add](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/Add_to_list_performance.png)
链表初始化为空，开启多线程，每个线程插入10000个互异元素，结果说明：<br>
随着线程数量增加，无锁链表耗时变化不大（scaling能力最强）; 粗粒度锁耗时与线程数量成反比; 虽然细粒度锁的并发程度明显高于粗粒度锁（细粒度锁CPU使用率400%，粗粒度锁CPU使用率100%），但是细粒度锁耗时超过粗粒度锁（可能由于频繁的加锁，解锁导致）。<br>
#### 删除操作
![The Performance of Delete](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/Delete_to_list_performance.png)
链表初始化包含10000个互异元素，开始多线程，每个线程对链表中的10000个元素进行删除操作，结果说明：<br>
除了带有垃圾回收的同步方式外，其余同步方式性能基本相同; 由于进行垃圾回收时，所有线程遭阻塞，另外垃圾回收还涉及额外的锁操作作为overhead，所以无锁+垃圾回收方式在删除元素时，性能最低。<br>
#### 40%添加，40%查询，20%删除
![The Performance of 40% Add, 40% Loop up, 20% Delete](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/mixed_op_to_list_performance.png)