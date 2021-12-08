/**
    Site.h
    Purpose: To create the manager which handles a particular site
    @author Ishita Jaisia (ij2056)
    @version 1.1 1/12/21
*/

#ifndef SITE_H
#define SITE_H

#include <iostream>
#include <string>
#include <map>
#include <set>
#include "../LockManager/LockManager.h"

#define TOTAL_VARIABLES 20

using namespace std;

class Site
{
    int siteNumber;
    bool siteUp;
    map<int, map<int, int> > data;
    map<int, map<int, int> > cache;
    map<int, bool> isStale;
    LockManager *lockManager;

public:
    Site();
    Site(int siteNumber);
    void fillData();
    void dump(int tick);
    bool isUp();
    void fail();
    void recover();
    bool stale(int variable);
    bool isVariablePresent(int variable);
    bool isVariableUnique(int variable);
    int canAcquireReadLock(int variable, int transaction_number);
    int canAcquireWriteLock(int variable, int transaction_number);
    bool acquireLock(int variable, int transaction_number, Lock_type type);
    set<int> getAllTransactionLocks(int variable);
    void releaseAllLocks();
    bool releaseAllTransactionLocks(int transaction_number);
    int getValue(int variable, int tick);
    bool setValue(int variable, int value, int tick);
    bool setCache(int variable, int value, int tick);
    void clearCache();
    void commitCache(int variable);
    int getLastCommittedTime(int variable, int begin_time);
};

#endif