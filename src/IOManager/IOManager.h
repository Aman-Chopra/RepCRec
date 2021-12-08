/**
    IOManager.h
    Purpose: To parse the input file and create appropriate transaction objects
    @author Aman Chopra (ac8511)
    @version 1.1 19/11/21
*/

#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include "../Transaction/Transaction.h"

using namespace std;

class IOManager
{

public:
    IOManager();
    void readFile(string fileName, vector<Transaction *> &transaction);
    int getTransactionNumber(string line);
    int getSiteNumber(string line);
    vector<int> getReadTransactionDetails(string line);
    vector<int> getWriteTransactionDetails(string line);
};

#endif