/**
    Deadlock.h
    Purpose: To create wait for graph and check deadlock
    @author Ishita Jaisia (ij2056)
    @version 1.1 24/11/21
*/

#ifndef DEADLOCK_H
#define DEADLOCK_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <set>

using namespace std;

const int number_of_transactions = 100;

class Deadlock
{
public:
    int vertices;
    list<int> *adj;
    set<int> deadlock_transactions;
    Deadlock();
    void addEdge(int source, set<int> transactions);
    void removeAllEdges(int source);
    bool checkForCycleUtil(int vertices, bool visited[], bool *rs);
    set<int> checkForDeadlock();
};

#endif