# makefile

# uncomment the version of the compiler that you are using
#clang++ is for Mac OS 
#C++ = clang++ -std=c++11
# g++ is for most Linux
C++ = g++ -std=c++11

all: dataserver client test_suite

clean: 
	rm *.o

test_suite: sync_test_mutex sync_test_mg

mutex: mutex.hpp mutex.cpp
	$(C++) -c -g mutex.cpp

mutex_guard: mutex_guard.hpp mutex_guard.cpp mutex.o
	$(C++) -c -g mutex_guard.cpp mutex.o

sync_test_mg: mutex.o mutex_guard.o
	$(C++) -DMUTEX_GUARD_TEST -o sync_test_mg sync_test.cpp mutex.o mutex_guard.o -lpthread

sync_test_mutex: mutex.o 
	$(C++) -DMUTEX_TEST -o sync_test_mutex sync_test.cpp mutex.o -lpthread

reqchannel.o: reqchannel.hpp reqchannel.cpp
	$(C++) -c -g reqchannel.cpp

dataserver: dataserver.cpp reqchannel.o 
	$(C++) -o dataserver dataserver.cpp reqchannel.o -lpthread

client: client.cpp reqchannel.o
	$(C++) -o client client.cpp reqchannel.o
