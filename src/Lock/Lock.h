/**
    Lock.h
    Purpose: To create the lock class
    @author Ishita Jaisia (ij2056)
    @version 1.1 28/11/21
*/

#ifndef LOCK_H
#define LOCK_H

#include <iostream>
#include <set>

using namespace std;

#define TOTAL_VARIABLES 20

enum Lock_type
{
    READ_LOCK,
    WRITE_LOCK,
    NO_LOCK
};

class Lock
{
public:
    Lock_type type;
    set<int> transactions;
    Lock(Lock_type type, set<int> transactions);
};

#endif