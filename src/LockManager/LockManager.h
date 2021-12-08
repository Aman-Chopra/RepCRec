/**
    LockManager.h
    Purpose: To create the manager which handles all the locks of a particular site
    @author Ishita Jaisia (ij2056)
    @version 1.1 29/11/21
*/

#ifndef LOCKMANAGER_H
#define LOCKMANAGER_H

#include <iostream>
#include <string>
#include <map>
#include "../Lock/Lock.h"

using namespace std;

#define TOTAL_VARIABLES 20

enum Acquire_lock
{
    CAN_ACQUIRE_LOCK,
    CANNOT_ACQUIRE_LOCK,
    CAN_ACQUIRE_IF_WAIT_QUEUE_EMPTY,
    CAN_ACQUIRE_LOCK_SAME_WRITE_TRANSACTION
};

class LockManager
{
    map<int, Lock *> lock_table;

public:
    LockManager();
    int canAcquireReadLock(int variable, int transaction_number);
    int canAcquireWriteLock(int variable, int transaction_number);
    void acquireLock(int variable, int transaction_number, Lock_type type);
    set<int> getAllTransactionLocks(int variable);
    void releaseAllLocks();
    void releaseAllTransactionLocks(int transaction_number);
};

#endif