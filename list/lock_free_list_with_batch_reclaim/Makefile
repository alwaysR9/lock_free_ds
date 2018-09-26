x : lock_free_list_test.cpp lock_free_list.cpp lock_free_list.h list_node.cpp list_node.h
	g++ -std=c++11 -g -o ./build/x lock_free_list_test.cpp \
		lock_free_list.cpp lock_free_list.h \
		list_node.cpp list_node.h \
		-lpthread

rcu : rcu_test.cpp rcu.cpp rcu.h list_node.cpp list_node.h
	g++ -std=c++11 -g -o ./build/rcu rcu_test.cpp \
		rcu.cpp rcu.h \
		list_node.cpp list_node.h \
		-lpthread

clean :
	rm ./build/*