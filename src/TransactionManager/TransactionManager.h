/**
    TransactionManager.h
    Purpose: The Transaction Manager class which communicates with different entities like Sites and IOManager and manages Transactions
    @author Aman Chopra (ac8511)
    @version 1.1 21/11/21
*/

#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../Site/Site.h"
#include "../Transaction/Transaction.h"
#include "../IOManager/IOManager.h"
#include "../Deadlock/Deadlock.h"

#define TOTAL_SITES 10

using namespace std;

enum Waiting_reason
{
    WAITING_LOCK,
    WAITING_SITE_UP,
    WAITING_SITE_WRITE
};

enum Print_reason
{
    SITE_IS_DOWN,
    STALE_DATA,
    SITE_WENT_DOWN_BEFORE_RO,
    CANNOT_GET_LOCK_ON_SITE,
    SOMETHING_IN_WAIT_QUEUE
};

class TransactionManager
{

    vector<Site *> sites;
    vector<Transaction *> transactions;
    map<Site *, int> lastFailedTime;
    map<int, int> transactionBeginTimes;
    map<int, set<int> > siteAccessList;
    map<int, set<int> > writeTransactionAccessVariables;
    vector<pair<Transaction *, Waiting_reason> > lockWaitQueue;
    set<int> abortedTransactions;
    string show_reason[5] = {"the site is down", "the data is stale as there has been no writes after site recovered",
                   "the site went down before RO began and after the last committed value of variable", "it cannot get the lock", "there is already something in the wait queue"};
    int tick;

    Deadlock *DeadlockManager;
    IOManager *IOObj;

public:
    TransactionManager();
    void start(string fileName);
    void initialsieSites();
    void fillSitesData();
    void initialsieDeadlockManager();
    void dumpAll();
    void readFile(string fileName);
    set<int> canSkipLockWaitQueue(int operation_variable, int transaction_number, Lock_type type, int size);
    void removeTransactionFromWaitQueue(int transaction_number);
    void execute();
    int getYoungest(set<int> transactions);
    void addWaitForEdge(int T1, int T2);
    bool checkForDeadlock();
    bool checkWaitQueue();
    bool canCommit(int transaction);
    void commit(int transaction);
    void abort(int transaction, string reason);
};

#endif