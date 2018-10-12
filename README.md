Lock-Free List
--------------

对比了单机多核环境下，支持多线程操作的数据结构，采用**不同的同步方式**，对性能的影响。数据结构采用**有序链表实现的集合**，支持插入，删除，查询，它们均是线程安全的。对比了以下同步方式：<br>
  * 粗力度锁（coarse_grained lock）
  * 细力度锁（fine_grained lock）
  * 无锁（lock_free）
  * 无锁 + 垃圾回收（lock_free + rcu）

## 效果对比
#### 实验机器配置
8核，支持8线程

#### 添加操作
![The Performance of Add](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/Add_to_list_performance.png)<br>
**链表初始化为空，开启多线程，每个线程插入10000个互异元素。**<br>
随着线程数量增加，**无锁（lock-free, lock-free-rcu）**耗时并没有显著增加，在多于10个线程时，CPU利用率可以达到接近800%，**充分的利用了多核，并发程度最高**; **粗粒度锁(coarse-grained)**耗时与线程数量成正比，CPU利用率约100%，**仅有一个核心在工作**; 虽然**细粒度锁(fine-grained)**的并发程度明显高于粗粒度锁（CPU使用率约400%），但**频繁的加锁，解锁**导致耗时甚至多于粗粒度锁。<br>
#### 删除操作
![The Performance of Delete](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/Delete_to_list_performance.png)<br>
**链表初始化包含10000个元素，开启多线程，每个线程对链表中的10000个元素进行删除操作。**<br>
无锁（lock-free, lock-free-rcu）耗时最少，并且，**垃圾回收（lock-free-rcu）**产生的**性能损耗**相对加锁**较小**。<br>
#### 40%插入，40%查询，20%删除
![The Performance of 40% Add, 40% Loop up, 20% Delete](https://github.com/alwaysR9/lock_free_ds/blob/master/list/result_report/mixed_op_to_list_performance.png)<br>
链表初始化包含10000个元素，开启多线程，每个线程执行一类操作，共三类操作：随机插入10000个元素，随机删除10000个元素，随机查询10000个元素。插入，查询，删除的线程比例为40%:40%:20%。<br>