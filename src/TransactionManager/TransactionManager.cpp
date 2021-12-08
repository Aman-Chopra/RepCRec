/**
    TransactionManager.cpp
    Purpose: The Transaction Manager class which communicates with different entities like Sites and IOManager and manages Transactions
    @author Aman Chopra (ac8511)
    @version 1.1 21/11/21
*/

#include "TransactionManager.h"

using namespace std;

TransactionManager::TransactionManager()
{
    tick = 0;
}

/**
 * Initialises the Transaction Manager
 *
 * @param fileName the input test file to be read
 * @return void
 * @sideEffects - invokes IOManager and Deadlock objects
 */
void TransactionManager::start(string fileName)
{
    initialsieSites();
    fillSitesData();
    initialsieDeadlockManager();
    readFile(fileName);
    execute();
}

/**
 * Initialises all the sites
 *
 * @param void
 * @return void
 * @sideEffects - the sites vector
 */
void TransactionManager::initialsieSites()
{
    for (int i = 0; i < TOTAL_SITES; i++)
    {
        Site *site = new Site(i + 1);
        sites.push_back(site);
    }
}

/**
 * Fills the data of all the sites
 *
 * @param void
 * @return void
 * @sideEffects - the sites vector
 */
void TransactionManager::fillSitesData()
{
    for (int i = 0; i < TOTAL_SITES; i++)
    {
        sites[i]->fillData();
    }
}

/**
 * Initialises the deadlock manager
 *
 * @param void
 * @return void
 * @sideEffects - the deadlock object
 */
void TransactionManager::initialsieDeadlockManager()
{
    DeadlockManager = new Deadlock();
}

/**
 * Print the database of all the sites
 *
 * @param void
 * @return void
 * @sideEffects - None
 */
void TransactionManager::dumpAll()
{
    cout << "Printing the data of all the sites:" << endl;
    for (int i = 0; i < TOTAL_SITES; i++)
    {
        sites[i]->dump(tick);
    }
}

/**
 * Reads the input file
 *
 * @param fileName
 * @return void
 * @sideEffects - the IOObject
 */
void TransactionManager::readFile(string fileName)
{
    IOObj = new IOManager();
    IOObj->readFile(fileName, transactions);
}

/**
 * Removes the transaction from the wait queue
 *
 * @param transaction number
 * @return void
 * @sideEffects - the lock wait queue
 */
void TransactionManager::removeTransactionFromWaitQueue(int transaction_number)
{
    int size = lockWaitQueue.size();
    for (int i = 0; i < size; i++)
    {
        Transaction *transaction = lockWaitQueue[i].first;
        if (transaction->transaction_number == transaction_number)
            lockWaitQueue.erase(lockWaitQueue.begin() + i);
    }
}

/**
 * Checks for deadlock
 *
 * @param void
 * @return bool
 * @sideEffects - the aborted transactions vector and the lock wait queue
 */
bool TransactionManager::checkForDeadlock()
{
    set<int> deadlock_transactions = DeadlockManager->checkForDeadlock();
    if (deadlock_transactions.size() != 0)
    {
        cout << "Deadlock detected" << endl;
        int transaction = getYoungest(deadlock_transactions);
        DeadlockManager->removeAllEdges(transaction);
        abortedTransactions.insert(transaction);
        cout << "Aborting youngest transaction T" << transaction << endl;
        string reason = "Youngest transaction to avoid deadlock.";
        for (int site_no = 0; site_no < TOTAL_SITES; site_no++)
        {
            sites[site_no]->releaseAllTransactionLocks(transaction);
        }
        abort(transaction, reason);
        removeTransactionFromWaitQueue(transaction);
        return true;
    }
    return false;
}

/**
 * Checks if the transactions from the wait queue can be skipped
 *
 * @param variable, transaction_number, the lock type, size
 * @return void
 * @sideEffects - None
 */
set<int> TransactionManager::canSkipLockWaitQueue(int operation_variable, int transaction_number, Lock_type type, int size)
{
    bool ans = true;
    set<int> edges;
    for (int i = 0; i < size; i++)
    {
        Transaction *transaction = lockWaitQueue[i].first;
        if (type == READ_LOCK && transaction->transaction_type == WRITE && transaction->operation_variable == operation_variable)
            edges.insert(transaction->transaction_number);
        else if (type == WRITE_LOCK && (transaction->transaction_type == WRITE || transaction->transaction_type == READ) && transaction->operation_variable == operation_variable)
            edges.insert(transaction->transaction_number);
    }
    return edges;
}

/**
 * Checks if some transactions from the wait queue can be executed
 *
 * @param void
 * @return bool
 * @sideEffects - None
 */
bool TransactionManager::checkWaitQueue()
{
    int size = lockWaitQueue.size();
    bool isSomethingCleared = false;
    for (int i = 0; i < size; i++)
    {
        Transaction *transaction = lockWaitQueue[i].first;
        if (transaction->transaction_type == READONLY)
        {
            int transaction_number = transaction->transaction_number;
            int operation_variable = transaction->operation_variable;
            bool wait = true;
            int index = 0;

            int begin_time = transactionBeginTimes[transaction_number];

            for (int j = 0; j < TOTAL_SITES; j++)
            {
                bool isUnique = sites[j]->isVariableUnique(operation_variable);
                if (!sites[j]->isUp() || !sites[j]->isVariablePresent(operation_variable))
                    continue;
                else
                {
                    if (isUnique || lastFailedTime.find(sites[j]) == lastFailedTime.end())
                    {
                        int value = sites[j]->getValue(operation_variable, begin_time);
                        cout << "x" << operation_variable << ": " << value << endl;
                        wait = false;
                        index = i;
                        break;
                    }
                    else if (!isUnique && sites[j]->stale(operation_variable))
                    {
                        continue;
                    }
                    else
                    {
                        int down_time = lastFailedTime[sites[j]];
                        int lastCommittedTime = sites[j]->getLastCommittedTime(operation_variable, begin_time);
                        if (down_time > lastCommittedTime && down_time < begin_time)
                        {
                            wait = true;
                        }
                        else
                        {
                            int value = sites[j]->getValue(operation_variable, begin_time);
                            cout << "x" << operation_variable << ": " << value << endl;
                            wait = false;
                            index = i;
                            break;
                        }
                    }
                }
            }
            if (!wait)
            {
                // Remove from the wait queue
                isSomethingCleared = true;
                lockWaitQueue.erase(lockWaitQueue.begin() + index);
                break;
            }
        }

        else if (transaction->transaction_type == READ)
        {
            int transaction_number = transaction->transaction_number;
            int operation_variable = transaction->operation_variable;
            int index = 0;

            bool wait = true;
            int check_index = i;
            set<int> edges = canSkipLockWaitQueue(operation_variable, transaction_number, READ_LOCK, check_index);
            if (edges.size() != 0)
            {
                continue;
            }

            for (int j = 0; j < TOTAL_SITES; j++)
            {
                bool isUnique = sites[j]->isVariableUnique(operation_variable);
                if (!sites[j]->isUp() || !sites[j]->isVariablePresent(operation_variable))
                    continue;
                else
                {
                    int status = sites[j]->canAcquireReadLock(operation_variable, transaction_number);
                    if (status == CANNOT_ACQUIRE_LOCK)
                        continue;
                    if (!isUnique && sites[j]->stale(operation_variable))
                    {
                        continue;
                    }
                    sites[j]->acquireLock(operation_variable, transaction_number, READ_LOCK);
                    if (siteAccessList.find(j + 1) == siteAccessList.end())
                    {
                        set<int> transactions;
                        transactions.insert(transaction_number);
                        siteAccessList[j + 1] = transactions;
                    }
                    else
                    {
                        set<int> transactions = siteAccessList[j];
                        transactions.insert(transaction_number);
                        siteAccessList[j + 1] = transactions;
                    }
                    int value = sites[j]->getValue(operation_variable, tick);
                    cout << "x" << operation_variable << ": " << value << endl;
                    wait = false;
                    index = i;
                    break;
                }
            }
            if (!wait)
            {
                isSomethingCleared = true;
                lockWaitQueue.erase(lockWaitQueue.begin() + index);
                break;
            }
        }

        else if (transaction->transaction_type == WRITE)
        {
            int transaction_number = transaction->transaction_number;
            int operation_variable = transaction->operation_variable;
            int operation_value = transaction->operation_value;
            int index = 0;

            set<int> dependency_transactions;
            bool canLockAllSites = true;

            int check_index = i;
            set<int> edges = canSkipLockWaitQueue(operation_variable, transaction_number, WRITE_LOCK, check_index);
            if (edges.size() != 0)
            {
                dependency_transactions.insert(edges.begin(), edges.end());
            }
            for (int j = 0; j < TOTAL_SITES; j++)
            {
                if (!sites[j]->isUp() || !sites[j]->isVariablePresent(operation_variable))
                    continue;
                else
                {
                    int status = sites[j]->canAcquireWriteLock(operation_variable, transaction_number);
                    if (status == CANNOT_ACQUIRE_LOCK)
                    {
                        // It cannot acquire lock in this site
                        canLockAllSites = false;
                    }
                    else if (status == CAN_ACQUIRE_IF_WAIT_QUEUE_EMPTY)
                    {
                        // It can get the lock provided wait queue is empty or it can skip it
                        set<int> edges = canSkipLockWaitQueue(operation_variable, transaction_number, WRITE_LOCK, check_index);
                        if (edges.size() != 0)
                        {
                            canLockAllSites = false;
                        }
                    }
                    else
                    {
                        // It can acquire lock in this site
                    }
                }
            }
            bool access = 0;
            if (canLockAllSites)
            {
                for (int j = 0; j < TOTAL_SITES; j++)
                {
                    if (!sites[j]->isUp() || !sites[j]->isVariablePresent(operation_variable))
                        continue;
                    access = 1;
                    sites[j]->acquireLock(operation_variable, transaction_number, WRITE_LOCK);
                    bool status = sites[j]->setCache(operation_variable, operation_value, tick);
                    if (siteAccessList.find(j + 1) == siteAccessList.end())
                    {
                        set<int> transactions;
                        transactions.insert(transaction_number);
                        siteAccessList[j + 1] = transactions;
                    }
                    else
                    {
                        set<int> transactions = siteAccessList[j];
                        transactions.insert(transaction_number);
                        siteAccessList[j + 1] = transactions;
                    }
                    if (writeTransactionAccessVariables.find(transaction_number) == writeTransactionAccessVariables.end())
                    {
                        set<int> access_variables;
                        access_variables.insert(operation_variable);
                        writeTransactionAccessVariables[transaction_number] = access_variables;
                    }
                    else
                    {
                        set<int> access_variables = writeTransactionAccessVariables[transaction_number];
                        access_variables.insert(operation_variable);
                        writeTransactionAccessVariables[transaction_number] = access_variables;
                    }
                }
                if (access == 1)
                {
                    isSomethingCleared = true;
                    index = i;
                    lockWaitQueue.erase(lockWaitQueue.begin() + index);
                    break;
                }
            }
            else
            {
                // Will still be in the lock queue
            }
        }
    }
    return isSomethingCleared;
}

/**
 * Execute all the transactions
 *
 * @param void
 * @return void
 * @sideEffects - None
 */
void TransactionManager::execute()
{
    for (int i = 0; i < transactions.size(); i++)
    {
        tick++;
        while (true)
        {
            bool isDeadlock = checkForDeadlock();
            if (!isDeadlock)
                break;
        }
        while (true)
        {
            bool isSomethingCleared = checkWaitQueue();
            if (!isSomethingCleared)
                break;
        }
        if (transactions[i]->command == FAIL)
        {
            int site_number = transactions[i]->site_number;
            sites[site_number - 1]->releaseAllLocks();
            sites[site_number - 1]->fail();
            if (siteAccessList.find(site_number) != siteAccessList.end())
            {
                set<int> transactions = siteAccessList[site_number];
                set<int>::iterator it;
                for (it = transactions.begin(); it != transactions.end(); it++)
                {
                    abortedTransactions.insert(*it);
                }
                siteAccessList.erase(site_number);
            }
            lastFailedTime[sites[site_number - 1]] = tick;
            cout << "Site " << site_number << " goes down." << endl;
        }
        else if (transactions[i]->command == RECOVER)
        {
            int site_number = transactions[i]->site_number;
            sites[site_number - 1]->recover();
            cout << "Site " << site_number << " recovers." << endl;
        }
        else if (transactions[i]->command == DUMP)
        {
            dumpAll();
        }
        else if (transactions[i]->command == BEGIN)
        {
            int transaction_number = transactions[i]->transaction_number;
            transactionBeginTimes[transaction_number] = tick;
            cout << "Transaction T" << transaction_number << " begins." << endl;
        }
        else if (transactions[i]->command == BEGINRO)
        {
            int transaction_number = transactions[i]->transaction_number;
            transactionBeginTimes[transactions[i]->transaction_number] = tick;
            cout << "Read only transaction T" << transaction_number << " begins." << endl;
        }
        else if (transactions[i]->command == END)
        {
            int transaction_number = transactions[i]->transaction_number;

            if (canCommit(transaction_number))
                commit(transaction_number);
            else
            {
                string reason = "Accessed a site which went down.";
                abort(transaction_number, reason);
            }
            transactionBeginTimes.erase(transactions[i]->transaction_number);
            removeTransactionFromWaitQueue(transaction_number);
            for (int site_no = 0; site_no < TOTAL_SITES; site_no++)
            {
                sites[site_no]->releaseAllTransactionLocks(transaction_number);
            }
            DeadlockManager->removeAllEdges(transactions[i]->transaction_number);
        }
        else
        {
            if (transactions[i]->transaction_type == READONLY)
            {
                int transaction_number = transactions[i]->transaction_number;
                int operation_variable = transactions[i]->operation_variable;
                bool wait = true;
                int status = 0;
                int begin_time = transactionBeginTimes[transaction_number];

                for (int i = 0; i < TOTAL_SITES; i++)
                {
                    bool isUnique = sites[i]->isVariableUnique(operation_variable);
                    if (!sites[i]->isUp() || !sites[i]->isVariablePresent(operation_variable))
                    {
                        status = SITE_IS_DOWN;
                        continue;
                    }
                    else
                    {
                        if (isUnique || lastFailedTime.find(sites[i]) == lastFailedTime.end())
                        {
                            int value = sites[i]->getValue(operation_variable, begin_time);
                            cout << "x" << operation_variable << ": " << value << endl;
                            wait = false;
                            break;
                        }
                        else if (!isUnique && sites[i]->stale(operation_variable))
                        {
                            status = STALE_DATA;
                            continue;
                        }
                        else
                        {
                            // not unique and not stale
                            int down_time = lastFailedTime[sites[i]];
                            int lastCommittedTime = sites[i]->getLastCommittedTime(operation_variable, begin_time);
                            if (down_time > lastCommittedTime && down_time < begin_time)
                            {
                                status = SITE_WENT_DOWN_BEFORE_RO;
                                wait = true;
                            }
                            else
                            {
                                int value = sites[i]->getValue(operation_variable, begin_time);
                                cout << "x" << operation_variable << ": " << value << endl;
                                wait = false;
                                break;
                            }
                        }
                    }
                }
                if (wait)
                {
                    cout << "Transaction T" << transactions[i]->transaction_number << " going to the wait queue as " << show_reason[status] << endl;
                    lockWaitQueue.push_back(make_pair(transactions[i], WAITING_SITE_UP));
                }
            }
            else if (transactions[i]->transaction_type == READ)
            {
                int transaction_number = transactions[i]->transaction_number;
                int operation_variable = transactions[i]->operation_variable;
                int status_reason = 0;
                bool wait = true;

                set<int> dependency_transactions;
                int size = lockWaitQueue.size();
                set<int> edges = canSkipLockWaitQueue(operation_variable, transaction_number, READ_LOCK, size);

                if (edges.size() != 0)
                {
                    dependency_transactions.insert(edges.begin(), edges.end());
                }
                string reason = "";
                // still check sites it can get lock actually w1 wait w2 r1
                for (int i = 0; i < TOTAL_SITES; i++)
                {
                    bool isUnique = sites[i]->isVariableUnique(operation_variable);
                    if (!sites[i]->isUp() || !sites[i]->isVariablePresent(operation_variable))
                        continue; // if cannot acquire lock, add edges to deadlock
                    else
                    {
                        if (!isUnique && sites[i]->stale(operation_variable))
                        {
                            reason = "the Site contains Stale data.";
                            status_reason = STALE_DATA;
                            continue;
                        }
                        int status = sites[i]->canAcquireReadLock(operation_variable, transaction_number);
                        if (status == CANNOT_ACQUIRE_LOCK)
                        {
                            // Cannot acquire lock
                            status_reason = CANNOT_GET_LOCK_ON_SITE;
                            reason = "it cannot acquire the lock";
                            set<int> transactions = sites[i]->getAllTransactionLocks(operation_variable);
                            dependency_transactions.insert(transactions.begin(), transactions.end());
                            continue;
                        }
                        else
                        {
                            if (status == CAN_ACQUIRE_IF_WAIT_QUEUE_EMPTY && edges.size() != 0)
                            {
                                reason = "it cannot acquire the lock";
                                status_reason = CANNOT_GET_LOCK_ON_SITE;
                                continue;
                            }
                            else
                            {
                                dependency_transactions.clear();
                                sites[i]->acquireLock(operation_variable, transaction_number, READ_LOCK);
                                cout << "Transaction T" << transaction_number << " locking site " << (i + 1) << endl;
                                if (siteAccessList.find(i + 1) == siteAccessList.end())
                                {
                                    set<int> transactions;
                                    transactions.insert(transaction_number);
                                    siteAccessList[i + 1] = transactions;
                                }
                                else
                                {
                                    set<int> transactions = siteAccessList[i];
                                    transactions.insert(transaction_number);
                                    siteAccessList[i + 1] = transactions;
                                }
                                int value = sites[i]->getValue(operation_variable, tick);
                                cout << "x" << operation_variable << ": " << value << endl;
                                wait = false;
                                break;
                            }
                        }
                    }
                }
                if (wait)
                {
                    // cout << "Transaction T" << transactions[i]->transaction_number << " going to the wait queue as " << reason << endl;
                    cout << "Transaction T" << transactions[i]->transaction_number << " going to the wait queue as " << show_reason[status_reason] << endl;
                    DeadlockManager->addEdge(transaction_number, dependency_transactions);
                    lockWaitQueue.push_back(make_pair(transactions[i], WAITING_LOCK));
                }
            }
            else if (transactions[i]->transaction_type == WRITE)
            {
                int transaction_number = transactions[i]->transaction_number;
                int operation_variable = transactions[i]->operation_variable;
                int operation_value = transactions[i]->operation_value;
                set<int> dependency_transactions;
                bool canLockAllSites = true;
                int reason = 0;
                int size = lockWaitQueue.size();
                set<int> edges = canSkipLockWaitQueue(operation_variable, transaction_number, WRITE_LOCK, size);
                if (edges.size() != 0)
                {
                    reason = SOMETHING_IN_WAIT_QUEUE;
                    dependency_transactions.insert(edges.begin(), edges.end());
                }

                for (int i = 0; i < TOTAL_SITES; i++)
                {
                    if (!sites[i]->isUp() || !sites[i]->isVariablePresent(operation_variable))
                        continue;
                    else
                    {
                        int status = sites[i]->canAcquireWriteLock(operation_variable, transaction_number);
                        if (status == CANNOT_ACQUIRE_LOCK)
                        {
                            reason = CANNOT_GET_LOCK_ON_SITE;
                            set<int> transactions = sites[i]->getAllTransactionLocks(operation_variable);
                            dependency_transactions.insert(transactions.begin(), transactions.end());
                            canLockAllSites = false;
                        }
                        else if (status == CAN_ACQUIRE_IF_WAIT_QUEUE_EMPTY)
                        {
                            // It can get the lock provided wait queue is empty or it can skip it
                            int size = lockWaitQueue.size();
                            set<int> edges = canSkipLockWaitQueue(operation_variable, transaction_number, WRITE_LOCK, size);
                            if (edges.size() != 0)
                            {
                                reason = CANNOT_GET_LOCK_ON_SITE;
                                dependency_transactions.insert(edges.begin(), edges.end());
                                canLockAllSites = false;
                            }
                        }
                        else
                        {
                            // It can acquire lock in this site
                        }
                    }
                }
                bool access = 0;

                if (canLockAllSites && dependency_transactions.size() == 0)
                {
                    reason = SITE_IS_DOWN;
                    for (int i = 0; i < TOTAL_SITES; i++)
                    {
                        if (!sites[i]->isUp() || !sites[i]->isVariablePresent(operation_variable))
                            continue;
                        cout << "Transaction T" << transaction_number << " locking site " << (i + 1) << endl;
                        access = 1;
                        sites[i]->acquireLock(operation_variable, transaction_number, WRITE_LOCK);
                        bool status = sites[i]->setCache(operation_variable, operation_value, tick);
                        if (siteAccessList.find(i + 1) == siteAccessList.end())
                        {
                            set<int> transactions;
                            transactions.insert(transaction_number);
                            siteAccessList[i + 1] = transactions;
                        }
                        else
                        {
                            set<int> transactions = siteAccessList[i];
                            transactions.insert(transaction_number);
                            siteAccessList[i + 1] = transactions;
                        }
                        if (writeTransactionAccessVariables.find(transaction_number) == writeTransactionAccessVariables.end())
                        {
                            set<int> access_variables;
                            access_variables.insert(operation_variable);
                            writeTransactionAccessVariables[transaction_number] = access_variables;
                        }
                        else
                        {
                            set<int> access_variables = writeTransactionAccessVariables[transaction_number];
                            access_variables.insert(operation_variable);
                            writeTransactionAccessVariables[transaction_number] = access_variables;
                        }
                    }
                }
                if (access == 0)
                {
                    cout << "Transaction T" << transactions[i]->transaction_number << " going to the wait queue as " << show_reason[reason] << endl;
                    DeadlockManager->addEdge(transaction_number, dependency_transactions);
                    lockWaitQueue.push_back(make_pair(transactions[i], WAITING_LOCK));
                }
            }
            else
            {
                // Do Nothing
            }
        }
    }
}

/**
 * Gets the youngest transaction to avoid deadlock
 *
 * @param set of transactions
 * @return the transaction number
 * @sideEffects - None
 */
int TransactionManager::getYoungest(set<int> transactions)
{
    map<int, int>::iterator it;
    int max_ = INT_MIN;
    int youngestTransaction;
    for (it = transactionBeginTimes.begin(); it != transactionBeginTimes.end(); it++)
    {
        if (transactions.find(it->first) != transactions.end() && it->second > max_)
        {
            max_ = it->second;
            youngestTransaction = it->first;
        }
    }
    return youngestTransaction;
}

/**
 * Checks if the transaction can be committed
 *
 * @param transaction
 * @return bool
 * @sideEffects - None
 */
bool TransactionManager::canCommit(int transaction)
{
    if (abortedTransactions.find(transaction) != abortedTransactions.end())
        return false;
    return true;
}

/**
 * Commit a transaction
 *
 * @param transaction
 * @return void
 * @sideEffects - Write transaction's access variables
 */
void TransactionManager::commit(int transaction)
{
    // Commit cache
    set<int> access_variables = writeTransactionAccessVariables[transaction];
    set<int>::iterator it;
    for (it = access_variables.begin(); it != access_variables.end(); it++)
    {
        int operation_variable = *it;
        for (int i = 0; i < TOTAL_SITES; i++)
        {
            if (sites[i]->isUp() && sites[i]->isVariablePresent(operation_variable))
            {
                sites[i]->commitCache(operation_variable);
            }
        }
    }
    cout << "Transaction T" << transaction << " commits." << endl;
    writeTransactionAccessVariables.erase(transaction);
    for (int i = 1; i <= TOTAL_SITES; i++)
    {
        if (siteAccessList.find(i) != siteAccessList.end())
        {
            set<int> transactions = siteAccessList[i];
            transactions.erase(transaction);
            siteAccessList[i] = transactions;
        }
    }
}

/**
 * Abort the transaction
 *
 * @param transaction, abort reason
 * @return void
 * @sideEffects - list of aborted transactions
 */
void TransactionManager::abort(int transaction, string reason)
{
    cout << "Transaction T" << transaction << " aborts. ";
    cout << "Reason: " << reason << endl;
    abortedTransactions.erase(transaction);
}