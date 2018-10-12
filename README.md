Lock-Free List
--------------

对比了单机多核环境下，支持多线程操作的数据结构，采用**不同的同步方式**，对性能的影响。数据结构采用**有序链表实现的集合**，支持插入，删除，查询，它们均是线程安全的。对比了以下同步方式：<br>
  * 粗力度锁（coarse-grained lock）
  * 细力度锁（fine-grained lock）
  * 无锁（lock-free）
  * 无锁 + 垃圾回收（lock-free + rcu）

#### 实验机器
8核，支持8线程

#### ============================= 添加元素 ==============================
![The Performance of Add](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/Add_to_list_performance.png)<br>

#### ============================= 删除元素 ==============================
![The Performance of Delete](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/Delete_to_list_performance.png)<br>

#### ====================== 40%插入，40%查询，20%删除 ======================
![The Performance of 40% Add, 40% Loop up, 20% Delete](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/mixed_op_to_list_performance.png)<br>

#### =============================== 总结 ================================
  1. 在并发情境下，完成相同的操作：
      - 无锁链表，运行时间最少; 可以充分利用多核(CPU利用率可达800%)，并行度最高。
      - 粗粒度锁链表，运行时间较长; 只能利用一个核(CPU占用率100%)。
      - 细粒度锁链表，运行时间最长(甚至超过粗粒度锁链表，这是频繁的锁操作造成的); 可以利用多核，但不充分(CPU利用率最多约400%)。
  2. 对于无锁链表，进行垃圾回收：
      - 垃圾回收带来的性能损失，相比于加锁链表的运行耗时非常小。
      - 垃圾回收的主要时间开销在STL list操作和保证垃圾回收线程安全的全局锁。
  3. 测试
      - 包括：并发情境的正确性测试，性能测试，内存泄漏检查

#### ============================ 重要参考资料 =============================
多种同步方式介绍(包括无锁数据结构的垃圾回收方法)：<br>
https://people.eecs.berkeley.edu/~stephentu/presentations/workshop.pdf<br>
无锁链表：<br>
https://www.cl.cam.ac.uk/research/srg/netos/papers/2001-caslists.pdf<br>
无锁数据结构的**批量**垃圾回收方法：<br>
https://en.wikipedia.org/wiki/Read-copy-update<br>