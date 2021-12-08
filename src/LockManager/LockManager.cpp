/**
    LockManager.cpp
    Purpose: To create the manager which handles all the locks of a particular site
    @author Ishita Jaisia (ij2056)
    @version 1.1 29/11/21
*/

#include "LockManager.h"

using namespace std;

LockManager::LockManager()
{
}

/**
 * Acquires a lock of a particular variable and transaction number
 *
 * @param variable, transaction number, lock type
 * @return void
 * @sideEffects - lock table
 */
void LockManager::acquireLock(int variable, int transaction_number, Lock_type type)
{
    if (lock_table.find(variable) == lock_table.end())
    {
        set<int> transactions;
        transactions.insert(transaction_number);
        Lock *lock = new Lock(type, transactions);
        lock_table[variable] = lock;
    }
    else
    {
        Lock *lock = lock_table[variable];
        lock->transactions.insert(transaction_number);
        lock->type = type;
    }
}

/**
 * Checks if a read lock can be acquired on a particular variable and transaction number
 *
 * @param variable, transaction number
 * @return int
 * @sideEffects - lock table
 */
int LockManager::canAcquireReadLock(int variable, int transaction_number)
{
    if (lock_table.find(variable) == lock_table.end())
    {
        return CAN_ACQUIRE_LOCK;
    }
    else
    {
        Lock *lock = lock_table[variable];
        set<int> transactions = lock->transactions;
        if (transactions.size() == 0)
            return CAN_ACQUIRE_LOCK;
        if (lock->type == READ_LOCK)
            return CAN_ACQUIRE_IF_WAIT_QUEUE_EMPTY;
        else if (lock->type == WRITE_LOCK && (lock->transactions.find(transaction_number) != lock->transactions.end()))
            return CAN_ACQUIRE_LOCK_SAME_WRITE_TRANSACTION;
        else
            return CANNOT_ACQUIRE_LOCK;
    }
    return CANNOT_ACQUIRE_LOCK;
}

/**
 * Checks if a write lock can be acquired on a particular variable and transaction number
 *
 * @param variable, transaction number
 * @return int
 * @sideEffects - lock table
 */
int LockManager::canAcquireWriteLock(int variable, int transaction_number)
{
    if (lock_table.find(variable) == lock_table.end())
    {
        return CAN_ACQUIRE_LOCK; // can take lock
    }
    else
    {
        Lock *lock = lock_table[variable];
        set<int> transactions = lock->transactions;
        if (transactions.size() == 0)
            return CAN_ACQUIRE_LOCK;
        if (lock->type == READ_LOCK && lock->transactions.size() == 1 && (lock->transactions.find(transaction_number) != lock->transactions.end()))
            return CAN_ACQUIRE_IF_WAIT_QUEUE_EMPTY;
        else if (lock->type == WRITE_LOCK && (lock->transactions.find(transaction_number) != lock->transactions.end()))
            return CAN_ACQUIRE_LOCK;
        else
            return CANNOT_ACQUIRE_LOCK; // can not take lock
    }
    return CANNOT_ACQUIRE_LOCK;
}

/**
 * Release all locks of a site
 *
 * @param void
 * @return void
 * @sideEffects - lock table
 */
void LockManager::releaseAllLocks()
{
    lock_table.clear();
}

/**
 * Release all locks of a transaction 
 *
 * @param transaction number
 * @return void
 * @sideEffects - lock table
 */
void LockManager::releaseAllTransactionLocks(int transaction_number)
{
    map<int, Lock *>::iterator it;
    for (it = lock_table.begin(); it != lock_table.end(); it++)
    {
        Lock *lock = it->second;
        set<int> transactions = lock->transactions;
        transactions.erase(transaction_number);
        lock->transactions = transactions;
    }
}

/**
 * Get all transactions which holds a lock on a particular variable
 *
 * @param variable
 * @return set of transactions
 * @sideEffects - None
 */
set<int> LockManager::getAllTransactionLocks(int variable)
{
    map<int, Lock *>::iterator it;
    set<int> result_transactions;
    Lock *lock = lock_table[variable];
    set<int> transactions = lock->transactions;
    result_transactions.insert(transactions.begin(), transactions.end());
    return result_transactions;
}