/**
    IOManager.cpp
    Purpose: To parse the input file and create appropriate transaction objects
    @author Aman Chopra (ac8511)
    @version 1.1 19/11/21
*/

#include "IOManager.h"

using namespace std;

IOManager::IOManager()
{
}

/**
 * Reads the input file
 *
 * @param fileName the input test file to be read, vector of transaction objects
 * @return void
 * @sideEffects - None
 */
void IOManager::readFile(string fileName, vector<Transaction *> &transactions)
{
    ifstream inputFile(fileName);
    string line = "";
    vector<string> tokens;
    if (inputFile.fail())
    {
        cerr << "Unable to open file for reading." << endl;
        exit(1);
    }
    if (inputFile.peek() == ifstream::traits_type::eof())
    {
        exit(1);
    }
    Transaction_type transaction_type = NONE;
    Commands command = NO;
    vector<int> read_only_transactions;
    int site_number = 0;
    int transaction_number = 0;
    int operation_variable = 0;
    double operation_value = 0;
    while (getline(inputFile, line))
    {
        int idx = line.find("//");
        if (idx != string::npos)
        {
            line = line.substr(0, idx);
        }
        int length = line.length();
        bool blank = true;
        for (int i = 0; i < length; i++)
        {
            int c = line[i];
            if (!isspace(c))
            {
                blank = false;
                break;
            }
        }
        if (blank || line.empty())
        {
            continue;
        }
        stringstream stream(line);
        transaction_type = NONE;
        command = NO;
        site_number = 0;
        transaction_number = 0;
        operation_variable = 0;
        operation_value = 0;
        if (line.rfind("beginRO") == 0)
        {
            command = BEGINRO;
            transaction_type = NONE;
            transaction_number = getTransactionNumber(line);
            read_only_transactions.push_back(transaction_number);
        }
        else if (line.rfind("begin") == 0)
        {
            command = BEGIN;
            transaction_type = NONE;
            transaction_number = getTransactionNumber(line);
        }
        else if (line.rfind("end") == 0)
        {
            command = END;
            transaction_type = NONE;
            transaction_number = getTransactionNumber(line);
        }
        else if (line.rfind("fail") == 0)
        {
            command = FAIL;
            transaction_type = NONE;
            site_number = getSiteNumber(line);
        }
        else if (line.rfind("dump") == 0)
        {
            command = DUMP;
            transaction_type = NONE;
        }
        else if (line.rfind("recover") == 0)
        {
            command = RECOVER;
            transaction_type = NONE;
            site_number = getSiteNumber(line);
        }
        else
        {
            command = NO;
            if (line[0] == 'R')
            {
                vector<int> transaction_details = getReadTransactionDetails(line);
                transaction_number = transaction_details[0];
                if (find(read_only_transactions.begin(), read_only_transactions.end(), transaction_number) != read_only_transactions.end())
                    transaction_type = READONLY;
                else
                    transaction_type = READ;
                operation_variable = transaction_details[1];
            }
            else
            {
                transaction_type = WRITE;
                vector<int> transaction_details = getWriteTransactionDetails(line);
                transaction_number = transaction_details[0];
                operation_variable = transaction_details[1];
                operation_value = transaction_details[2];
            }
        }
        Transaction *transactionObj = new Transaction(transaction_type, command, site_number, transaction_number, operation_variable, operation_value);
        transactions.push_back(transactionObj);
    }
}

/**
 * Gets the transaction number from a line
 *
 * @param line
 * @return int
 * @sideEffects - None
 */
int IOManager::getTransactionNumber(string line)
{
    int start = line.find("(", 0);
    int end = line.find(")", 0);
    int transaction_number = stoi(line.substr(start + 2, end - start + 1));
    return transaction_number;
}

/**
 * Gets the site number from a line
 *
 * @param line
 * @return int
 * @sideEffects - None
 */
int IOManager::getSiteNumber(string line)
{
    int start = line.find("(", 0);
    int end = line.find(")", 0);
    int site_number = stoi(line.substr(start + 1, end - start + 1));
    return site_number;
}

/**
 * Gets the detail of a write transaction
 *
 * @param line
 * @return vector of integers
 * @sideEffects - None
 */
vector<int> IOManager::getWriteTransactionDetails(string line)
{
    vector<int> transaction_details;
    int start = line.find("(", 0);
    int end = line.find(")", 0);
    string details = line.substr(start + 1, end - start - 1);
    stringstream s_stream(details);
    int count = 0;
    int value = 0;
    while (s_stream.good())
    {
        count++;
        string substr;
        getline(s_stream, substr, ',');
        if (substr[0] == ' ')
            substr = substr.substr(1, substr.size());
        if (count == 3)
            value = stoi(substr);
        else
            value = stoi(substr.substr(1, substr.size()));
        transaction_details.push_back(value);
    }
    return transaction_details;
}

/**
 * Gets the detail of a read transaction
 *
 * @param line
 * @return vector of integers
 * @sideEffects - None
 */
vector<int> IOManager::getReadTransactionDetails(string line)
{
    vector<int> transaction_details;
    int start = line.find("(", 0);
    int end = line.find(")", 0);
    string details = line.substr(start + 1, end - start - 1);
    stringstream s_stream(details);
    int value = 0;
    while (s_stream.good())
    {
        string substr;
        getline(s_stream, substr, ',');
        if (substr[0] == ' ')
            substr = substr.substr(1, substr.size());
        value = stoi(substr.substr(1, substr.size()));
        transaction_details.push_back(value);
    }
    return transaction_details;
}