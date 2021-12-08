/**
    Lock.cpp
    Purpose: To create the lock class
    @author Ishita Jaisia (ij2056)
    @version 1.1 28/11/21
*/

#include "Lock.h"

Lock::Lock(Lock_type type, set<int> transactions)
{
    this->type = type;
    this->transactions = transactions;
}