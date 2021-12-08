CXX = clang++
QMAKE_CXXFLAGS += -std=c+1y

all: clean TransactionManager.o Transaction.o LockManager.o IOManager.o Deadlock.o Site.o Lock.o 
	mkdir -p build
	g++ -std=c++1y src/main.cpp TransactionManager.o Transaction.o LockManager.o IOManager.o Deadlock.o Site.o Lock.o -o build/RepCRec
	rm -f *.o

TransactionManager.o: src/TransactionManager/TransactionManager.h src/TransactionManager/TransactionManager.cpp
	g++ -std=c++1y -c src/TransactionManager/TransactionManager.cpp

Transaction.o: src/Transaction/Transaction.h src/Transaction/Transaction.cpp
	g++ -std=c++1y -c src/Transaction/Transaction.cpp

LockManager.o: src/LockManager/LockManager.h src/LockManager/LockManager.cpp
	g++ -std=c++1y -c src/LockManager/LockManager.cpp

IOManager.o: src/IOManager/IOManager.h src/IOManager/IOManager.cpp
	g++ -std=c++1y -c src/IOManager/IOManager.cpp

Deadlock.o: src/Deadlock/Deadlock.h src/Deadlock/Deadlock.cpp
	g++ -std=c++1y -c src/Deadlock/Deadlock.cpp

Site.o: src/Site/Site.h src/Site/Site.cpp src/Lock/Lock.h
	g++ -std=c++1y -c src/Site/Site.cpp

Lock.o: src/Lock/Lock.h src/Lock/Lock.cpp
	g++ -std=c++1y -c src/Lock/Lock.cpp

clean:
	rm -f *.o
	rm -rf build

	