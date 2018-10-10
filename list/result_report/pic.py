import sys
import matplotlib.pyplot as plt

x = [1, 5, 10, 20]

# add performance
#coarse = [0.3, 1.8, 4.0, 8.7]
#fine = [1.7, 2.2, 5.3, 14.1]
#lock_free = [1.1, 1.3, 1.9, 3.1]
#lock_free_rcu = [1.2, 1.3, 1.7, 3.2]
#title = 'The performance of add'
#fname = 'Add_to_list_performance.png'

# delete performance
#coarse = [0.001204, 0.01505, 0.02744, 0.049104]
#fine = [0.002461, 0.029858, 0.043798, 0.101315]
#lock_free = [0.002609, 0.006219, 0.009167, 0.007794]
#lock_free_rcu = [0.058134, 0.261741, 0.457155, 0.935205]
#title = 'The performance of delete'
#fname = 'Delete_to_list_performance.png'

# hybird performance
x = [5, 10, 15]
coarse = [0.4399, 0.9230, 1.2607]
fine = [0.4334, 1.0120, 1.6584]
lock_free = [0.2863, 0.3872, 0.4797]
lock_free_rcu = [0.2906, 0.3610, 0.4648]
title = 'The performance of mixed operation'
fname = 'mixed_op_to_list_performance.png'

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
