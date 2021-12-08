/**
    Transaction.h
    Purpose: To create the transaction objects and its operations
    @author Aman Chopra (ac8511)
    @version 1.1 21/11/21
*/

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <limits.h>
#include <string>
#include <vector>

using namespace std;

enum Transaction_type
{
    READ,
    WRITE,
    READONLY,
    NONE
};
enum Commands
{
    BEGIN,
    BEGINRO,
    FAIL,
    RECOVER,
    DUMP,
    END,
    NO
};

class Transaction
{
public:
    Transaction_type transaction_type;
    Commands command;
    int site_number;
    int transaction_number;
    int operation_variable;
    double operation_value;

    Transaction(Transaction_type transaction_type, Commands command, int site_number = 0, int transaction_number = 0, int operation_variable = 0, double operation_value = INT_MIN);
};

#endif