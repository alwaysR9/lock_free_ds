import sys
import matplotlib.pyplot as plt

x = [1, 5, 10, 20]

# add performance
coarse = [0.3, 1.8, 4.0, 8.7]
fine = [1.7, 2.2, 5.3, 14.1]
lock_free = [1.19833, 1.28093, 1.81707, 3.13864]
lock_free_rcu = [1.20253, 1.3221, 1.99714, 3.31791]
title = 'The performance of add'
fname = 'Add_to_list_performance.png'

# delete performance
#coarse = [0.332658, 0.946826, 1.65139, 4.7045]
#fine = [1.68914, 2.69389, 4.9484, 12.8524]
#lock_free = [1.1877, 1.17583, 1.17385, 2.52614]
#lock_free_rcu = [1.25902, 1.25339, 1.3765, 3.09896]
#title = 'The performance of delete'
#fname = 'Delete_to_list_performance.png'

# hybird performance
#x = [5, 10, 15]
#coarse = [0.4399, 0.9230, 1.2607]
#fine = [0.4334, 1.0120, 1.6584]
#lock_free = [0.2863, 0.3872, 0.4797]
#lock_free_rcu = [0.2906, 0.3610, 0.4648]
#title = 'The performance of mixed operation'
#fname = 'mixed_op_to_list_performance.png'

plt.plot(x, coarse, marker='o', label='coarse-grained')
plt.plot(x, fine, marker='o', label='fine-grained')
plt.plot(x, lock_free, marker='o', label='lock-free')
plt.plot(x, lock_free_rcu, marker='o', label='lock-free-rcu')
plt.legend(bbox_to_anchor=(1, 1), loc=1, borderaxespad=0.)
plt.xlabel('the number of threads')
plt.ylabel('the time consumed (second)')
plt.title(title)
plt.xticks(x)
plt.savefig(fname)
plt.show()
