/**
    Transaction.cpp
    Purpose: To create the transaction objects and its operations
    @author Aman Chopra (ac8511)
    @version 1.1 21/11/21
*/

#include "Transaction.h"

using namespace std;

Transaction::Transaction(Transaction_type transaction_type, Commands command, int site_number, int transaction_number, int operation_variable, double operation_value)
{
    this->transaction_type = transaction_type;
    this->command = command;
    this->site_number = site_number;
    this->transaction_number = transaction_number;
    this->operation_variable = operation_variable;
    this->operation_value = operation_value;
}
