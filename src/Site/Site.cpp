/**
    Site.cpp
    Purpose: To create the manager which handles a particular site
    @author Ishita Jaisia (ij2056)
    @version 1.1 1/12/21
*/

#include "Site.h"

Site::Site(int siteNumber)
{
    this->siteNumber = siteNumber;
    siteUp = true;
    lockManager = new LockManager();
}

/**
 * Fills the data of a particular site
 *
 * @param void
 * @return void
 * @sideEffects - data map
 */
void Site::fillData()
{
    int index = 0;
    int variable = 0;
    int value = 0;
    for (index = 1; index <= TOTAL_VARIABLES; index++)
    {
        if (index % 2 == 0)
        {
            isStale[index] = false;
            value = 10 * index;
            variable = index;
            map<int, int> history;
            history[0] = value;
            data[variable] = history;
        }
        else if ((1 + (index % 10)) == siteNumber)
        {
            value = 10 * index;
            variable = index;
            map<int, int> history;
            history[0] = value;
            data[variable] = history;
        }
        else
        {
            continue;
        }
    }
}

/**
 * Prints the data of a particular site
 *
 * @param tick
 * @return void
 * @sideEffects - None
 */
void Site::dump(int tick)
{
    map<int, map<int, int> >::iterator it;
    cout << "site " << siteNumber << " - ";
    for (it = data.begin(); it != data.end(); it++)
    {
        map<int, int> history = it->second;
        map<int, int>::iterator itr;
        int value = 0;
        if (history.find(tick) != history.end())
        {
            value = history[tick];
        }
        else
        {
            itr = history.lower_bound(tick);
            itr--;
            value = itr->second;
        }
        cout << "x" << it->first << ": " << value << " ";
    }
    cout << endl;
}

/**
 * Returns if a site is up
 *
 * @param void
 * @return bool
 * @sideEffects - None
 */
bool Site::isUp()
{
    return siteUp;
}

/**
 * Site goes down functionality
 *
 * @param void
 * @return void
 * @sideEffects - None
 */
void Site::fail()
{
    siteUp = false;
    for (int index = 1; index <= TOTAL_VARIABLES; index++)
    {
        if (index % 2 == 0)
        {
            isStale[index] = true;
        }
    }
    cache.clear();
}

/**
 * Recovers a site
 *
 * @param void
 * @return void
 * @sideEffects - None
 */
void Site::recover()
{
    siteUp = true;
}

/**
 * Checks if a variable is present on a site
 *
 * @param int
 * @return bool
 * @sideEffects - None
 */
bool Site::isVariablePresent(int variable)
{
    if (data.find(variable) != data.end())
        return true;
    return false;
}

/**
 * Checks if a variable is unique on a site
 *
 * @param int
 * @return bool
 * @sideEffects - None
 */
bool Site::isVariableUnique(int variable)
{
    return isVariablePresent(variable) && (variable % 2 != 0);
}

/**
 * Checks if a read lock on a variable and transaction can be acquired
 *
 * @param variable, transaction_number
 * @return int
 * @sideEffects - None
 */
int Site::canAcquireReadLock(int variable, int transaction_number)
{
    return lockManager->canAcquireReadLock(variable, transaction_number);
}

/**
 * Checks if a write lock on a variable and transaction can be acquired
 *
 * @param variable, transaction_number
 * @return int
 * @sideEffects - None
 */
int Site::canAcquireWriteLock(int variable, int transaction_number)
{
    return lockManager->canAcquireWriteLock(variable, transaction_number);
}

/**
 * Acquires a lock of lock type on a variable and transaction 
 *
 * @param variable, transaction_number, lock type
 * @return int
 * @sideEffects - None
 */
bool Site::acquireLock(int variable, int transaction_number, Lock_type type)
{
    lockManager->acquireLock(variable, transaction_number, type);
    return true;
}

/**
 * Get all transaction locks on a particular variable
 *
 * @param variable
 * @return set of transactions
 * @sideEffects - None
 */
set<int> Site::getAllTransactionLocks(int variable)
{
    set<int> transactions = lockManager->getAllTransactionLocks(variable);
    return transactions;
}

/**
 * Release all locks of a site
 *
 * @param void
 * @return void
 * @sideEffects - None
 */
void Site::releaseAllLocks()
{
    lockManager->releaseAllLocks();
}

/**
 * Release all locks of a particular transaction on a site
 *
 * @param transaction_number
 * @return bool
 * @sideEffects - None
 */
bool Site::releaseAllTransactionLocks(int transaction_number)
{
    lockManager->releaseAllTransactionLocks(transaction_number);
    return true;
}

/**
 * Get value of a particular variable
 *
 * @param variable, time
 * @return int
 * @sideEffects - None
 */
int Site::getValue(int variable, int tick)
{
    map<int, int> history = data[variable];
    map<int, int>::iterator it;
    it = history.lower_bound(tick);
    it--;
    int value = it->second;
    return value;
}

/**
 * Set value of a particular variable
 *
 * @param variable, value, time
 * @return bool
 * @sideEffects - data map
 */
bool Site::setValue(int variable, int value, int tick)
{
    map<int, int> history = data[variable];
    history.insert(make_pair(tick, value));
    data[variable] = history;
    isStale[variable] = false;
    return true;
}

/**
 * Set cache of a particular variable
 *
 * @param variable, value, time
 * @return bool
 * @sideEffects - cache
 */
bool Site::setCache(int variable, int value, int tick)
{
    if (cache.find(variable) == cache.end())
    {
        map<int, int> history;
        history.insert(make_pair(tick, value));
        cache[variable] = history;
    }
    else
    {
        map<int, int> history = cache[variable];
        history.insert(make_pair(tick, value));
        cache[variable] = history;
    }
    return true;
}

/**
 * Clear the cache
 *
 * @param void
 * @return void
 * @sideEffects - cache
 */
void Site::clearCache()
{
    cache.clear();
}

/**
 * Commits cache of a particular variable to the data map
 *
 * @param variable
 * @return void
 * @sideEffects - data map
 */
void Site::commitCache(int variable)
{
    map<int, map<int, int> >::iterator it;
    for (it = cache.begin(); it != cache.end(); it++)
    {
        int var = it->first;
        if (var == variable)
        {
            map<int, int> history = it->second;
            int tick = (--history.end())->first;
            int value = history[tick];
            cout << "Committing variable x" << var << " on site " <<  siteNumber << " with value " << value << endl;
            setValue(var, value, tick);
        }
    }
}

/**
 * Get the last committed time of a particular variable
 *
 * @param variable, begin_time
 * @return int
 * @sideEffects - None
 */
int Site::getLastCommittedTime(int variable, int begin_time)
{
    map<int, int> history = data[variable];
    map<int, int>::iterator it;
    it = history.lower_bound(begin_time);
    it--;
    int lastCommitTime = it->first;
    return lastCommitTime;
}

/**
 * Check if a variable can be read after the site recovers
 *
 * @param variable
 * @return bool
 * @sideEffects - None
 */
bool Site::stale(int variable)
{
    return isStale[variable];
}